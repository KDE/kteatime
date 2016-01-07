/*
 * Copyright 1998-1999 by Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 * Copyright 2002-2003 by Martin Willers <willers@xm-arts.de>
 * Copyright 2007-2009 by Stefan Böhmann <kde@hilefoks.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "toplevel.h"

#include <QApplication>
#include <QCommandLineParser>

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>


int main (int argc, char *argv[])
{
    /**
     * Create application first
     */
    QApplication app(argc, argv);

    /**
     * construct about data for KTeaTime
     */
    KAboutData aboutData( QStringLiteral("kteatime"), i18n("KTeaTime"), QStringLiteral("1.3.0"),
                          i18n( "KDE utility for making a fine cup of tea." ),
                          KAboutLicense::GPL,
                          i18n( "© 1998-1999, Matthias Hoelzer-Kluepfel\n "
                                "© 2002-2003, Martin Willers\n "
                                "© 2007-2010, Stefan Böhmann"
                               )
                        );

    KCrash::initialize();

    aboutData.addAuthor(i18n("Stefan Böhmann"), i18n("Current maintainer"), QStringLiteral("kde@hilefoks.org"), QStringLiteral("http://www.hilefoks.org"), QStringLiteral("hilefoks"));
    aboutData.addAuthor(i18n("Matthias Hoelzer-Kluepfel"), QString(), QStringLiteral("hoelzer@kde.org"));
    aboutData.addAuthor(i18n("Martin Willers"), QString(), QStringLiteral("willers@xm-arts.de"));

    aboutData.addCredit(i18n("Daniel Teske"), i18n("Many patches"), QStringLiteral("teske@bigfoot.com"));

    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    /**
     * right dbus prefix == org.kde.
     */
    aboutData.setOrganizationDomain("kde.org");

    /**
     * register about data
     */
    KAboutData::setApplicationData(aboutData);

    /**
     * take component name and org. name from KAboutData
     */
    app.setApplicationName(aboutData.componentName());
    app.setApplicationDisplayName(aboutData.displayName());
    app.setOrganizationDomain(aboutData.organizationDomain());
    app.setApplicationVersion(aboutData.version());
    app.setQuitOnLastWindowClosed(false);

    /**
     * Create command line parser and feed it with known options
     */
    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.setApplicationDescription(aboutData.shortDescription());
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption timeOption(QStringList() << QStringLiteral("t") << QStringLiteral("time"), i18n("Start a new tea with this time."), i18n("seconds"));
    parser.addOption(timeOption);
    QCommandLineOption nameOption(QStringList() << QStringLiteral("n") << QStringLiteral("name"), i18n("Use this name for the tea started with --time."), i18n("name"));
    parser.addOption(nameOption);

    parser.process(app);
    aboutData.processCommandLine(&parser);

    TopLevel *toplevel=new TopLevel( &aboutData );

    const int time=parser.value(timeOption).toInt();
    if( time > 0 ) {
        const QString name = parser.value(nameOption);
        const Tea tea( name == QString() ? i18n( "Anonymous Tea" ) : name, time );
        toplevel->runTea( tea );
    }

    int ret = app.exec();
    
    delete toplevel;
    return ret;
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
