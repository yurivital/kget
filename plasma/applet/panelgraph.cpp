/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *
 *   Copyright (C) 2007 by Javier Goday <jgoday@gmail.com>
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "panelgraph.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsWidget>
#include <QProgressBar>

#include <KIcon>
#include <KIconLoader>
#include <KLocale>

PanelGraph::PanelGraph(QGraphicsWidget *parent)
    : TransferGraph(0)
{
    m_layout = static_cast <QGraphicsLinearLayout *> (parent->layout());
    if (m_layout) {
        m_bar = new QProgressBar();
        //m_bar->resize(m_applet->contentSize());
        m_bar->setValue(0);
        m_bar->setStyleSheet("background-color: transparent");
        m_proxyBar = new QGraphicsProxyWidget(parent);
        m_proxyBar->setWidget(m_bar);

        m_layout->addItem(m_proxyBar);
    }

    // create the tooltip of the panel graph

//    m_tooltip.mainText = "KGet active transfers";
  //  m_tooltip.image = KIcon("kget").pixmap(IconSize(KIconLoader::Desktop));
}

PanelGraph::~PanelGraph()
{
    m_proxyBar->setWidget(0);
}

void PanelGraph::setTransfers(const QVariantMap &transfers)
{
    double totalSize = 0;
    double completedSize = 0;
    QString tooltipTransfers;

    TransferGraph::setTransfers(transfers);

    foreach(const QString &key, transfers.keys()) {
        QVariantList attributes = transfers [key].toList();

        // only show the percent of the active transfers
        if(attributes.at(3).toUInt() == 1) {
            totalSize += attributes.at(2).toDouble();
            completedSize += ((attributes.at(1).toDouble() * attributes.at(2).toDouble()) / 100);

            tooltipTransfers += "<font size='small'>" + attributes.at(0).toString() + "</font> ";
            tooltipTransfers += "<font color='green'>" + attributes.at(1).toString() + "% </font> ";
            tooltipTransfers += "<font color='red'>" + KGlobal::locale()->formatByteSize( attributes.at(2).toDouble()) + "</font>";
            tooltipTransfers += "<br/>";
        }
    }

    if(totalSize > 0) {
        m_bar->setValue((int) ((completedSize * 100) / totalSize));
    }
    else {
         m_bar->setValue(0);
    }

    // set the tooltip for the applet with the active transfers
    //m_tooltip.subText = tooltipTransfers;
    //m_applet->setToolTip(m_tooltip);
}