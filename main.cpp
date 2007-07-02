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

#include <kwindowsystem.h>
#include <kstartupinfo.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include "toplevel.h"
#include <kapplication.h>

static const char description[] =
    I18N_NOOP("KDE utility for making a fine cup of tea");

static const char version[] = "v1.1.0";

int main(int argc, char *argv[])
{
  KAboutData aboutData( "kteatime", 0, ki18n("KTeaTime"),
    version, ki18n(description), KAboutData::License_GPL,
    ki18n("(c) 1998-1999, Matthias Hoelzer-Kluepfel\n(c) 2002-2003, Martin Willers"));
  aboutData.addAuthor(ki18n("Matthias Hoelzer-Kluepfel"),KLocalizedString(), "hoelzer@kde.org");
  aboutData.addAuthor(ki18n("Martin Willers"), KLocalizedString(), "willers@xm-arts.de");
  aboutData.addCredit(ki18n("Daniel Teske"), ki18n("Many patches"), "teske@bigfoot.com");
  KCmdLineArgs::init( argc, argv, &aboutData );

  KApplication app;

  TopLevel toplevel;
  // KWindowSystem::setSystemTrayWindowFor(toplevel.winId(), 0);
  toplevel.show();

  //app.setTopWidget(&toplevel);
  KStartupInfo::appStarted();

  return app.exec();
}
