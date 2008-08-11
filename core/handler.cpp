/* This file is part of the KDE project

   Copyright (C) 2008 Lukas Appelhans

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "handler.h"

#include "scheduler.h"

Handler::Handler(Scheduler * scheduler)
  : m_scheduler(scheduler)
{
}

Handler::~Handler()
{
}