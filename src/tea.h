/*
 *   Copyright (c) 2007-2009  Stefan Böhmann <kde@hilefoks.org>
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
#ifndef TEA_H
#define TEA_H

#include <QString>


/**
 * @short this class represent a tea.
 *
 * @author Stefan Böhmann <kde@hilefoks.org>
 */
class Tea {
    public:
         /**
          * Constructs an tea.
          * @param name the name of this tea.
          * @param time the tea time in seconds.
          */
        Tea(const QString &name = QString(), const unsigned time = 180);

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
        static QString int2time(const int time, const bool longdesc=false);

    private:
        QString m_name;
        unsigned m_time;
};

#endif

// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

