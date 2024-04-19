/*
    SPDX-FileCopyrightText: 2007-2009 Stefan Böhmann <kde@hilefoks.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef TEA_H
#define TEA_H

#include <QString>

/**
 * @short this class represent a tea.
 *
 * @author Stefan Böhmann <kde@hilefoks.org>
 */
class Tea
{
public:
    /**
     * Constructs an tea.
     * @param name the name of this tea.
     * @param time the tea time in seconds.
     */
    explicit Tea(const QString &name = QString(), const unsigned time = 180);

    /**
     * Returns the name of this tea.
     *
     * @return the name of this tea.
     */
    QString name() const;

    /**
     * Set the name of this the.
     * @param name the new name for this tea.
     */
    void setName(const QString &name);

    /**
     * Returns the time for this tea in seconds.
     *
     * @return the time for this tea in seconds.
     */
    unsigned time() const;

    /**
     * Set the time of this tea.
     * @param time the new time for this tea in seconds.
     */
    void setTime(const unsigned time);

    /**
     * Returns the time for this tea as a @ref QString.
     * @param longdesc if true return long names like  "5 minutes 30 seconds", else returns a short form like "5 min 30 s".
     *
     * @return the time for this tea as a @ref QString.
     */
    QString timeToString(const bool longdesc = false) const;

    /**
     * Returns a formatted @ref QString for the given time.
     * @param time the time in seconds.
     * @param longdesc if true return long names like  "5 minutes 30 seconds", else returns a short form like "5 min 30 s".
     *
     * @return the formatted @ref QString.
     */
    static QString int2time(const int time, const bool longdesc = false);

private:
    QString m_name;
    unsigned m_time;
};

#endif
