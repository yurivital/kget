/* This file is part of the KDE project

   Copyright (C) 2004 Dario Massarin <nekkar@libero.it>
   Copyright (C) 2006 Manolo Valdes <nolis71cu@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; version 2
   of the License.
*/

#include <QDomElement>

#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>

#include "transferMultiSegKio.h"

transferMultiSegKio::transferMultiSegKio(TransferGroup * parent, TransferFactory * factory,
                         Scheduler * scheduler, const KUrl & source, const KUrl & dest,
                         const QDomElement * e)
    : Transfer(parent, factory, scheduler, source, dest, e),
      m_copyjob(0)
{

}

void transferMultiSegKio::start()
{
    if(!m_copyjob)
        createJob();

    kDebug() << "transferMultiSegKio::start" << endl;

    setStatus(Job::Running, i18n("Connecting.."), SmallIcon("connect_creating"));
    setTransferChange(Tc_Status, true);
}

void transferMultiSegKio::stop()
{
    if(status() == Stopped)
        return;

    if(m_copyjob)
    {
        m_copyjob->doKill();
        m_copyjob=0;
    }

    kDebug() << "Stop" << endl;
    setStatus(Job::Stopped, i18n("Stopped"), SmallIcon("stop"));
    m_speed = 0;
    setTransferChange(Tc_Status | Tc_Speed, true);
}

int transferMultiSegKio::elapsedTime() const
{
    return -1; //TODO
}

int transferMultiSegKio::remainingTime() const
{
    return -1; //TODO
}

bool transferMultiSegKio::isResumable() const
{
    return true;
}

void transferMultiSegKio::load(QDomElement e)
{
    kDebug(5001) << "TransferMultiSegKio::load" << endl;

    Transfer::load(e);

    struct KIO::MultiSegData d;
    QDomNodeList segments = e.elementsByTagName ("Segment");
    QDomNode node;
    QDomElement segment;
    for( uint i=0 ; i < segments.length () ; ++i )
    {
        node = segments.item(i);
        segment = node.toElement ();
        d.src = KUrl(segment.attribute("Source"));
        d.bytes = segment.attribute("Bytes").toULongLong();
        d.offset = segment.attribute("OffSet").toULongLong();
        kDebug(5001) << "TransferMultiSegKio::load: adding Segment " << i << endl;
        tdata << d;
    }
}

void transferMultiSegKio::save(QDomElement e)
{
    kDebug(5001) << "TransferMultiSegKio::save" << endl;

    Transfer::save(e);

    QDomDocument doc(e.ownerDocument());
    QDomElement segment;
    QList<struct KIO::MultiSegData>::iterator it = tdata.begin();
    QList<struct KIO::MultiSegData>::iterator itEnd = tdata.end();
    for ( ; it!=itEnd ; ++it )
    {
        segment = doc.createElement("Segment");
        e.appendChild(segment);
        segment.setAttribute("Source", (*it).src.url());
        segment.setAttribute("Bytes", (*it).bytes); 
        segment.setAttribute("OffSet", (*it).offset);
     }
}


//NOTE: INTERNAL METHODS

void transferMultiSegKio::createJob()
{
    if(!m_copyjob)
    {
        if(tdata.empty())
        {
        m_copyjob = KIO::MultiSegfile_copy( m_source, m_dest, -1, false, 5);
        }
        else
        {
        m_copyjob = KIO::MultiSegfile_copy( m_source, m_dest, -1, false, tdata);
        }
        connect(m_copyjob, SIGNAL(result(KJob *)), 
                this, SLOT(slotResult(KJob *)));
        connect(m_copyjob, SIGNAL(infoMessage(KJob *, const QString &)), 
                this, SLOT(slotInfoMessage(KJob *, const QString &)));
        connect(m_copyjob, SIGNAL(connected(KIO::Job *)), 
                this, SLOT(slotConnected(KIO::Job *)));
        connect(m_copyjob, SIGNAL(percent(KJob *, unsigned long)), 
                this, SLOT(slotPercent(KJob *, unsigned long)));
        connect(m_copyjob, SIGNAL(totalSize(KJob *, qulonglong)), 
                this, SLOT(slotTotalSize(KJob *, qulonglong)));
        connect(m_copyjob, SIGNAL(processedSize(KJob *, qulonglong)), 
                this, SLOT(slotProcessedSize(KJob *, qulonglong)));
        connect(m_copyjob, SIGNAL(speed(KIO::Job *, unsigned long)), 
                this, SLOT(slotSpeed(KIO::Job *, unsigned long)));
    }
}

void transferMultiSegKio::slotResult( KJob *kioJob )
{
    kDebug() << "slotResult  (" << kioJob->error() << ")" << endl;
    switch (kioJob->error())
    {
        case 0:                            //The download has finished
        case KIO::ERR_FILE_ALREADY_EXIST:  //The file has already been downloaded.
            setStatus(Job::Finished, i18n("Finished"), SmallIcon("ok"));
            m_percent = 100;
            m_speed = 0;
            m_processedSize = m_totalSize;
            setTransferChange(Tc_Percent | Tc_Speed);
            break;
        default:
            //There has been an error
            kDebug() << "--  E R R O R  (" << kioJob->error() << ")--" << endl;
            setStatus(Job::Aborted, i18n("Aborted"), SmallIcon("stop"));
            break;
    }
    // when slotResult gets called, the m_copyjob has already been deleted!
    m_copyjob=0;
    setTransferChange(Tc_Status, true);
}

void transferMultiSegKio::slotInfoMessage( KJob * kioJob, const QString & msg )
{
  Q_UNUSED(kioJob);
    m_log.append(QString(msg));
}

void transferMultiSegKio::slotConnected( KIO::Job * kioJob )
{
//     kDebug() << "CONNECTED" <<endl;

  Q_UNUSED(kioJob);
    setStatus(Job::Running, i18n("Downloading.."), SmallIcon("player_play"));
    setTransferChange(Tc_Status, true);
}

void transferMultiSegKio::slotPercent( KJob * kioJob, unsigned long percent )
{
    kDebug() << "slotPercent" << endl;
    Q_UNUSED(kioJob);
    m_percent = percent;
    setTransferChange(Tc_Percent, true);
}

void transferMultiSegKio::slotTotalSize( KJob *kioJob, qulonglong size )
{
    kDebug() << "slotTotalSize" << endl;

    slotConnected(static_cast<KIO::Job*>(kioJob));

    m_totalSize = size;
    setTransferChange(Tc_TotalSize, true);
}

void transferMultiSegKio::slotProcessedSize( KJob *kioJob, qulonglong size )
{
    kDebug() << "slotProcessedSize" << endl; 

    if(status() != Job::Running)
        slotConnected(static_cast<KIO::Job*>(kioJob));

    m_processedSize = size;
    setTransferChange(Tc_ProcessedSize, true);
}

void transferMultiSegKio::slotSpeed( KIO::Job * kioJob, unsigned long bytes_per_second )
{
//     kDebug() << "slotSpeed" << endl;

    if(status() != Job::Running)
        slotConnected(kioJob);

    m_speed = bytes_per_second;
    setTransferChange(Tc_Speed, true);
}

#include "transferMultiSegKio.moc"
