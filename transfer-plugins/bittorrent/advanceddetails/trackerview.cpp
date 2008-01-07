/** IMPORTANT: please keep this file in sync with ktorrent! ****************/

/***************************************************************************
 *   Copyright (C) 2006-2007 by Joris Guisson, Ivan Vasic                  *
 *   joris.guisson@gmail.com                                               *
 *	 ivasic@gmail.com                                                  *
 *									   *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 ***************************************************************************/
#include "trackerview.h"
#include <interfaces/torrentinterface.h>
#include <interfaces/trackerslist.h>

#include <qdatetime.h>
#include <qstring.h>

#include <klocale.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kglobal.h>

#include <torrent/globals.h>
#include <util/log.h>

using namespace bt;

namespace kt
{
	TrackerView::TrackerView(QWidget *parent)
		: QWidget(parent), tc(0)
	{
		setupUi(this);
		connect(m_add_tracker,SIGNAL(clicked()),this,SLOT(btnAddClicked()));
		connect(m_remove_tracker,SIGNAL(clicked()),this,SLOT(btnRemoveClicked()));
		connect(m_update_tracker,SIGNAL(clicked()),this,SLOT(btnUpdateClicked()));
		connect(m_change_tracker,SIGNAL(clicked()),this,SLOT(btnChangeClicked()));
		connect(m_restore_defaults,SIGNAL(clicked()),this,SLOT(btnRestoreClicked()));

		m_url->setTextFormat(Qt::RichText);
		m_url->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
		m_status->setTextFormat(Qt::RichText);
		m_status->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
		m_next_update->setTextFormat(Qt::RichText);
                m_update_tracker->setIcon(KIcon("system-software-update"));
		m_add_tracker->setIcon(KIcon("list-add"));
                m_remove_tracker->setIcon(KIcon("list-remove"));

		setEnabled(false);
		torrentChanged(0);
	}
	
	TrackerView::~TrackerView()
	{
	}

	void TrackerView::btnAddClicked()
	{
		if(!tc)
			return;
		
		if(tc->getStats().priv_torrent)
		{
			KMessageBox::sorry(0, i18n("Cannot add a tracker to a private torrent."));
			return;
		}
		addTrackerDialog = new KDialog(this);
	        QWidget * widget = new QWidget(this);
		addTrackerWidget.setupUi(widget);
		addTrackerDialog->setMainWidget(widget);
		addTrackerDialog->setButtons(KDialog::Ok | KDialog::Cancel);
		addTrackerDialog->exec();
    
		if (addTrackerDialog->result() != 1)//Dialog not accepted
                        return;

		KUrl url(addTrackerWidget.lineEdit->text());
		if(!url.isValid())
		{
			KMessageBox::error(0, i18n("Malformed URL."));
			return;
		}
			
		m_tracker_list->addItem(addTrackerWidget.lineEdit->text());
		tc->getTrackersList()->addTracker(url,true);
	}

	void TrackerView::btnRemoveClicked()
	{
		QListWidgetItem* current = m_tracker_list->currentItem();
		if(!current)
			return;
		
		KUrl url(current->text());
		if(tc->getTrackersList()->removeTracker(url))
			delete current;
		else
			KMessageBox::sorry(0, i18n("Cannot remove torrent default tracker."));
	}

	void TrackerView::btnChangeClicked()
	{
		QListWidgetItem* current = m_tracker_list->currentItem();
		if(!current)
			return;
		
		KUrl url(current->text());
		tc->getTrackersList()->setTracker(url);
		tc->updateTracker();
	}

	void TrackerView::btnRestoreClicked()
	{
		tc->getTrackersList()->restoreDefault();
		tc->updateTracker();
		
		// update the list of trackers
		m_tracker_list->clear();
		
		const KUrl::List trackers = tc->getTrackersList()->getTrackerURLs();
		if(trackers.empty())
			return;
		
		foreach (KUrl u,trackers)
			m_tracker_list->addItem(u.prettyUrl());
	}

	void TrackerView::btnUpdateClicked()
	{
		if(!tc)
			return;
		
		tc->updateTracker();
	}
	
	void TrackerView::changeTC(TorrentInterface* ti)
	{
		if (tc == ti)
			return;
		
		setEnabled(ti != 0);
		torrentChanged(ti);
		update();
	}
	
	void TrackerView::update()
	{
		if(!tc)
			return;
		
		const TorrentStats & s = tc->getStats();
		if (s.running)
		{
			QTime t;
			t = t.addSecs(tc->getTimeToNextTrackerUpdate());
			m_next_update->setText(t.toString("mm:ss"));
		}
		
		//Update manual annunce button
		m_update_tracker->setEnabled(s.running && tc->announceAllowed());
		// only enable change when we can actually change and the torrent is running
		m_change_tracker->setEnabled(s.running && m_tracker_list->count() > 1);

		m_status->setText("<b>" + s.trackerstatus + "</b>");
		if (tc->getTrackersList())
			m_url->setText("<b>" + tc->getTrackersList()->getTrackerURL().prettyUrl() + "</b>");
		else
			m_url->clear();
		
		m_add_tracker->setEnabled(!tc->getStats().priv_torrent);
	}
	
	void TrackerView::torrentChanged(TorrentInterface* ti)
	{
		tc = ti;
		m_tracker_list->clear();
		if(!tc)
		{
			m_status->clear();
			m_url->clear();
			m_next_update->clear();
			
			m_add_tracker->setEnabled(false);
			m_remove_tracker->setEnabled(false);
			m_restore_defaults->setEnabled(false);
			m_change_tracker->setEnabled(false);
			m_update_tracker->setEnabled(false);
			return;
		}
		
		const TorrentStats & s = tc->getStats();
		
		if (s.priv_torrent)
		{
			m_add_tracker->setEnabled(false);
			m_remove_tracker->setEnabled(false);
			m_restore_defaults->setEnabled(false);
		}
		else
		{
			m_add_tracker->setEnabled(true);
			m_remove_tracker->setEnabled(true);
			m_restore_defaults->setEnabled(true);
		}
		
		const KUrl::List trackers = tc->getTrackersList()->getTrackerURLs();
		if(trackers.empty())
		{
			m_tracker_list->addItem(tc->getTrackersList()->getTrackerURL().prettyUrl());
		}
		else
		{
			foreach (KUrl u,trackers)
				m_tracker_list->addItem(u.prettyUrl());
		}
		
		m_update_tracker->setEnabled(s.running && tc->announceAllowed());
		m_change_tracker->setEnabled(s.running && trackers.count() > 1);
	}
}


#include "trackerview.moc"