/*
 *   Copyright (c) 2007 Stefan Böhmann <kde@hilefoks.org>
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
#include "tea.h"

#include <QString>


QString Tea::int2time(const int time, const bool longdesc)
{
    QString str;
    const unsigned min=60;
    const unsigned hour=60*min;
    const unsigned day=24*hour;
    const unsigned year=365*day;

    if(time/year) {
        if(longdesc) str.append(i18np("%1 year", "%1 years", time/year ));
        else str.append(i18np("%1 a", "%1 a", time/year ));
    }

    if((time%year)/day) {
        if(!str.isEmpty()) str.append(' ');
        if(longdesc) str.append(i18np("%1 day", "%1 days", (time%year)/day ));
        else str.append(i18np("%1 d", "%1 d", (time%year)/day ));
    }

    if((time%day)/hour) {
        if(!str.isEmpty()) str.append(' ');
        if(longdesc) str.append(i18np("%1 hour", "%1 hours", ((time%day)/hour) ));
        else str.append(i18np("%1 h", "%1 h", ((time%day)/hour)  ));
    }

    if((time%hour)/min) {
        if(!str.isEmpty()) str.append(' ');
        if(longdesc) str.append(i18np("%1 minute", "%1 minutes", ((time%hour)/min) ));
        else str.append(i18np("%1 min", "%1 min", ((time%hour)/min) ));
    }

    if(time%min) {
        if(!str.isEmpty()) str.append(' ');
        if(longdesc) str.append(i18np("%1 second", "%1 seconds", time%min));
        else str.append(i18np("%1 s", "%1 s", time%min));
    }

    return str;
}

