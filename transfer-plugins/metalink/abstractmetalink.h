/* This file is part of the KDE project

   Copyright (C) 2012 by Aish Raj Dahal <dahalaishraj@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#ifndef ABSTRACTMETALINK_H
#define ABSTRACTMETALINK_H


#include <KIO/Job>

#include "core/datasourcefactory.h"
#include "core/transfer.h"

#include "ui/metalinkcreator/metalinker.h"

class AbstractMetalink : public Transfer
{
    Q_OBJECT

    public:
        AbstractMetalink(TransferGroup * parent, TransferFactory * factory,
                         Scheduler * scheduler, const QUrl & src, const QUrl & dest,
                         const QDomElement * e = 0);
        virtual ~AbstractMetalink();

        int remainingTime() const;

        bool repair(const QUrl &file = QUrl());

        /**
         * Move the download to the new destination
         * @param newDirectory is a directory where the download should be stored
         * @returns true if newDestination can be used
         */
        virtual bool setDirectory(const QUrl &newDirectory);

        QHash<QUrl, QPair<bool, int> > availableMirrors(const QUrl &file) const;
        void setAvailableMirrors(const QUrl &file, const QHash<QUrl, QPair<bool, int> > &mirrors);

        /**
         * @param file for which to get the verifier
         * @return Verifier that allows you to add checksums manually verify a file etc.
         */
        virtual Verifier *verifier(const QUrl &file);

        /**
         * @param file for which to get the signature
         * @return Signature that allows you to add signatures and verify them
         */
        virtual Signature *signature(const QUrl &file);

        virtual QList<QUrl> files() const;

        FileModel *fileModel();

    public Q_SLOTS:
        // --- Job virtual functions ---
        virtual void start() = 0;
        virtual void stop();
        virtual void deinit(Transfer::DeleteOptions options) = 0;

    protected Q_SLOTS:
        /**
         * @return true if initialising worked
         * @note false does not mean that an error happened, it could mean, that the user
         * decided to update the metalink
         */
        void fileDlgFinished(int result);

        /**
        * Checks if the ticked (not started yet) files exist already on the hd and asks
        * the user how to proceed in that case. Also calls the according DataSourceFactories
        * setDoDownload(bool) methods.
        */

        void filesSelected();
        void slotUpdateCapabilities();
        void slotDataSourceFactoryChange(Transfer::ChangesFlags change);
        void slotRename(const QUrl &oldUrl, const QUrl &newUrl);
        void slotVerified(bool isVerified);
        virtual void slotSignatureVerified();

    protected :
        /**
         * Starts the type of metalink download
         */
        virtual void startMetalink() = 0;
        void untickAllFiles();
        void recalculateTotalSize(DataSourceFactory *sender);
        void recalculateProcessedSize();
        void recalculateSpeed();
        void updateStatus(DataSourceFactory *sender, bool *changeStatus);

    protected:
        FileModel *m_fileModel;
        int m_currentFiles;
        QHash<QUrl, DataSourceFactory*> m_dataSourceFactory;
        bool m_ready;
        int m_speedCount;
        int m_tempAverageSpeed;
        mutable int m_averageSpeed;
        int m_numFilesSelected; //The number of files that are ticked and should be downloaded
};

#endif
