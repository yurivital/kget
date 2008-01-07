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
#ifndef TRACKERVIEW_H
#define TRACKERVIEW_H

#include "ui_trackerview.h"
#include "ui_btaddtrackerwidget.h"

namespace bt
{
	class TorrentInterface;
	class TorrentFileInterface;
}

namespace kt
{
	
	/**
	 * @author Ivan Vasic <ivan@ktorrent.org>
	 */
	class TrackerView: public QWidget, public Ui_TrackerView
	{
		Q_OBJECT
	public:
		TrackerView(QWidget *parent);			
		virtual ~TrackerView();
			
		void update();
		void changeTC(bt::TorrentInterface* ti);
			
	public slots:
		virtual void btnUpdateClicked();
		virtual void btnRestoreClicked();
		virtual void btnChangeClicked();
		virtual void btnRemoveClicked();
		virtual void btnAddClicked();
			
	private:
		void torrentChanged(bt::TorrentInterface* ti);
			
	private:
		bt::TorrentInterface* tc;
                //Add Tracker Dialog
                KDialog *addTrackerDialog;
		Ui::BTAddTrackerWidget addTrackerWidget;
	};
}
#endif