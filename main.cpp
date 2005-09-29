/*
 *   KTeaTime - A tea timer.
 *
 *   Copyright (C) 1998-1999  Matthias Hoelzer-Kluepfel (hoelzer@kde.org)
 *   Copyright (C) 2002-2003  Martin Willers (willers@xm-arts.de)
 *
 *   With contributions from Daniel Teske <teske@bigfoot.com>, and
 *   Jackson Dunstan <jdunstan@digipen.edu>
 *   (and possibly others, as well)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <kwin.h>
#include <kstartupinfo.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "toplevel.h"


static const char description[] =
    I18N_NOOP("KDE utility for making a fine cup of tea");

static const char version[] = "v1.1.0";

int main(int argc, char *argv[])
{
  KAboutData aboutData( "kteatime", I18N_NOOP("KTeaTime"),
    version, description, KAboutData::License_GPL,
    "(c) 1998-1999, Matthias Hoelzer-Kluepfel\n(c) 2002-2003, Martin Willers");
  aboutData.addAuthor("Matthias Hoelzer-Kluepfel",0, "hoelzer@kde.org");
  aboutData.addAuthor("Martin Willers", 0, "willers@xm-arts.de");
  aboutData.addCredit("Daniel Teske", I18N_NOOP("Many patches"), "teske@bigfoot.com");
  KCmdLineArgs::init( argc, argv, &aboutData );

  KApplication app;

  TopLevel toplevel;
  KWin::setSystemTrayWindowFor(toplevel.winId(), 0);
  toplevel.show();

  app.setTopWidget(&toplevel);
  KStartupInfo::appStarted();

  return app.exec();
}
