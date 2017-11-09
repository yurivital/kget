/* This file is part of the KDE project

   Copyright (C) 2008 Manolo Valdes <nolis71cu@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "mirrorsearchfactory.h"
#include "mirrorsearchtransferdatasource.h"

#include "core/scheduler.h"
#include "core/transfergroup.h"

#include "kget_debug.h"
#include <qdebug.h>

#include <QDomElement>

K_PLUGIN_FACTORY_WITH_JSON(KGetFactory, "kget_mirrorsearchfactory.json", registerPlugin<MirrorSearchFactory>();)

MirrorSearchFactory::MirrorSearchFactory(QObject *parent, const QVariantList &args)
  : TransferFactory(parent, args)
{
}

MirrorSearchFactory::~MirrorSearchFactory()
{
}

TransferDataSource *MirrorSearchFactory::createTransferDataSource(const QUrl &srcUrl, const QDomElement &type, QObject *parent)
{
    qCDebug(KGET_DEBUG);

    if (type.attribute("type") == "search") {
        return new MirrorSearchTransferDataSource(srcUrl, parent);
    }
    return nullptr;
}

#include "mirrorsearchfactory.moc"
