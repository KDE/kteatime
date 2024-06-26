/*
    SPDX-FileCopyrightText: 1998-1999 Matthias Hölzer-Klüpfel <matthias@hoelzer-kluepfel.de>
    SPDX-FileCopyrightText: 2002-2003 Martin Willers <willers@xm-arts.de>
    SPDX-FileCopyrightText: 2007-2009 Stefan Böhmann <kde@hilefoks.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "toplevel.h"

#include <QApplication>
#include <QCommandLineParser>

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>

int main(int argc, char *argv[])
{
    /**
     * Create application first
     */
    QApplication app(argc, argv);
    /**
     * construct about data for KTeaTime
     */
    KAboutData aboutData(QStringLiteral("kteatime"),
                         i18n("KTeaTime"),
                         QStringLiteral(KTEATIME_VERSION),
                         i18n("KDE utility for making a fine cup of tea."),
                         KAboutLicense::GPL,
                         i18n("© 1998-1999, Matthias Hölzer-Klüpfel\n"
                              "© 2002-2003, Martin Willers\n"
                              "© 2007-2010, Stefan Böhmann"),
                         QString(),
                         QStringLiteral("https://apps.kde.org/kteatime"));

    KCrash::initialize();

    aboutData.addAuthor(i18n("Stefan Böhmann"),
                        i18n("Current maintainer"),
                        QStringLiteral("kde@hilefoks.org"),
                        QStringLiteral("http://www.hilefoks.org"),
                        QStringLiteral("hilefoks"));
    aboutData.addAuthor(i18n("Matthias Hölzer-Klüpfel"), QString(), QStringLiteral("matthias@hoelzer-kluepfel.de"));
    aboutData.addAuthor(i18n("Martin Willers"), QString(), QStringLiteral("willers@xm-arts.de"));

    aboutData.addCredit(i18n("Daniel Teske"), i18n("Many patches"), QStringLiteral("teske@bigfoot.com"));

    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    /**
     * register about data
     */
    KAboutData::setApplicationData(aboutData);

    app.setQuitOnLastWindowClosed(false);

    /**
     * Create command line parser and feed it with known options
     */
    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    QCommandLineOption timeOption(QStringList() << QStringLiteral("t") << QStringLiteral("time"), i18n("Start a new tea with this time."), i18n("seconds"));
    parser.addOption(timeOption);
    QCommandLineOption nameOption(QStringList() << QStringLiteral("n") << QStringLiteral("tea-name"),
                                  i18n("Use this name for the tea started with --time."),
                                  i18n("name"));
    parser.addOption(nameOption);

    parser.process(app);
    aboutData.processCommandLine(&parser);

    TopLevel *toplevel = new TopLevel(&aboutData);

    const int time = parser.value(timeOption).toInt();
    if (time > 0) {
        const QString name = parser.value(nameOption);
        const Tea tea(name.isEmpty() ? i18n("Custom Tea") : name, time);
        toplevel->runTea(tea);
    }

    int ret = app.exec();

    delete toplevel;
    return ret;
}
