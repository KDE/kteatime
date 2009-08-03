/*
 *   KTeaTime - A tea timer.
 *
 *   Copyright (C) 1998-1999  Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 *   Copyright (C) 2002-2003  Martin Willers <willers@xm-arts.de>
 *   Copyright (c) 2007       Stefan Böhmann <kde@hilefoks.org>
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
 */

#include "toplevel.h"

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>


static const char description[] = I18N_NOOP("KDE utility for making a fine cup of tea.");
static const char version[] = "v1.2.1";


int main (int argc, char *argv[])
{
    KAboutData aboutData( "kteatime", 0, ki18n("KTeaTime"), version, ki18n(description), KAboutData::License_GPL,
        ki18n("(c) 1998-1999, Matthias Hoelzer-Kluepfel\n(c) 2002-2003, Martin Willers\n(c) 2007, Stefan Böhmann"));
    aboutData.addAuthor(ki18n("Matthias Hoelzer-Kluepfel"), KLocalizedString(), "hoelzer@kde.org");
    aboutData.addAuthor(ki18n("Martin Willers"), KLocalizedString(), "willers@xm-arts.de");
    aboutData.addAuthor(ki18n("Stefan Böhmann"), KLocalizedString(), "kde@hilefoks.org");
    aboutData.addCredit(ki18n("Daniel Teske"), ki18n("Many patches"), "teske@bigfoot.com");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KCmdLineOptions options;
    options.add("t");
    options.add("time <seconds>", ki18n("Start a new tea with this time."));
    options.add("n");
    options.add("name <name>", ki18n("Use this name instead of \"Anonymous Tea\" for the tea started with %1.").subs("--time"));

    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    TopLevel *toplevel=new TopLevel(&aboutData);

    if(args->isSet("time")) {
        int time=args->getOption("time").toInt();
        if(time>0) {
	    toplevel->runTea(Tea(args->isSet("name") ? args->getOption("name") : i18n("Anonymous Tea"), time));
        }
    }
    args->clear();

    toplevel->show();

    return app.exec();
}

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
