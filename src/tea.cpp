/*
    SPDX-FileCopyrightText: 2007-2009 Stefan Böhmann <kde@hilefoks.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "tea.h"

#include <KLocalizedString>

Tea::Tea(const QString &name, const unsigned time)
    : m_time(0)
{
    setName(name);
    setTime(time);
}

QString Tea::name() const
{
    return m_name;
}

void Tea::setName(const QString &name)
{
    if (name.isEmpty()) {
        m_name = i18n("Custom Tea");
    } else {
        m_name = name;
    }
}

unsigned Tea::time() const
{
    return m_time;
}

void Tea::setTime(const unsigned time)
{
    m_time = time;
}

QString Tea::timeToString(const bool longdesc) const
{
    return int2time(m_time, longdesc);
}

QString Tea::int2time(const int time, const bool longdesc)
{
    QString str;
    const unsigned min = 60;
    const unsigned hour = 60 * min;
    const unsigned day = 24 * hour;
    const unsigned year = 365 * day;

    if (time / year) {
        if (longdesc) {
            str.append(i18np("%1 year", "%1 years", time / year));
        } else {
            str.append(i18np("%1 a", "%1 a", time / year));
        }
    }

    if ((time % year) / day) {
        if (!str.isEmpty()) {
            str.append(QLatin1Char(' '));
        }

        if (longdesc) {
            str.append(i18np("%1 day", "%1 days", (time % year) / day));
        } else {
            str.append(i18np("%1 d", "%1 d", (time % year) / day));
        }
    }

    if ((time % day) / hour) {
        if (!str.isEmpty()) {
            str.append(QLatin1Char(' '));
        }

        if (longdesc) {
            str.append(i18np("%1 hour", "%1 hours", ((time % day) / hour)));
        } else {
            str.append(i18np("%1 h", "%1 h", ((time % day) / hour)));
        }
    }

    if ((time % hour) / min) {
        if (!str.isEmpty()) {
            str.append(QLatin1Char(' '));
        }

        if (longdesc) {
            str.append(i18np("%1 minute", "%1 minutes", ((time % hour) / min)));
        } else {
            str.append(i18np("%1 min", "%1 min", ((time % hour) / min)));
        }
    }

    if (time % min) {
        if (!str.isEmpty()) {
            str.append(QLatin1Char(' '));
        }

        if (longdesc) {
            str.append(i18np("%1 second", "%1 seconds", time % min));
        } else {
            str.append(i18np("%1 s", "%1 s", time % min));
        }
    }

    return str;
}

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on;
// replace-tabs on; space-indent on; vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:
