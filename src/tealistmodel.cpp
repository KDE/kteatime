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

#include "tealistmodel.h"
#include "tea.h"

#include <klocalizedstring.h>



TeaListModel::TeaListModel(const QList<Tea> &tealist, QObject *parent)
  : QAbstractTableModel(parent), m_tealist(tealist)
{
}


QModelIndex TeaListModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED( parent );

    return createIndex(row, column);
}


int TeaListModel::rowCount(const QModelIndex &parent) const
{
    if( parent.isValid() ) {
        return 0;
    }

    return m_tealist.size();
}


int TeaListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED( parent );

    return 2;
}


QVariant TeaListModel::data(const QModelIndex &index, int role) const
{
    if( !index.isValid() ) {
        return QVariant();
    }

    if( role == Qt::DisplayRole ) {
        if( index.column() == 0 ) {
            return m_tealist.at( index.row() ).name();
        }

        return m_tealist.at( index.row() ).timeToString();
    }

    if( role == Qt::EditRole ) {
        if( index.column() == 0 ) {
            return m_tealist.at( index.row() ).name();
        }
        return m_tealist.at( index.row() ).time();
    }

    if( role == Qt::ToolTipRole ) {
        QString s( m_tealist.at( index.row() ).name() );
        s.append( i18n( " (" ) );
        s.append( m_tealist.at( index.row() ).timeToString( true ) );
        s.append( i18n( ")" ) );

        return s;
    }

    return QVariant();
}



bool TeaListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if( index.isValid() && ( role == Qt::EditRole || role == Qt::DisplayRole ) ) {
        if( index.column() == 0 ) {
            m_tealist[ index.row() ].setName( value.toString() );
        } else if( index.column() == 1 ) {
            m_tealist[ index.row() ].setTime( value.toUInt() );
        }
        dataChanged( index, index );
        return true;
    }

    return false;
}


QVariant TeaListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( orientation == Qt::Horizontal ) {
        if( role == Qt::DisplayRole )
            return section == 0 ? i18n("Name") : i18n("Time");
    }

    return QAbstractTableModel::headerData( section, orientation, role );
}


bool TeaListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED( parent );
    beginInsertRows( QModelIndex(), row, row+count-1 );

    for(int i = 0; i < count; ++i) {
        m_tealist.insert( row, Tea( i18n( "Unnamed Tea" ), 180 ) );
    }

    endInsertRows();
    return true;
}


bool TeaListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if( row-count > m_tealist.size() ) {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row+count-1);
    for(int i=0; i < count; ++i) {
        m_tealist.removeAt( row );
    }

    endRemoveRows();
    return true;
}


const QList<Tea> TeaListModel::getTeaList() const
{
    return m_tealist;
}


// kate: word-wrap off; encoding utf-8; indent-width 4; tab-width 4; line-numbers on; mixed-indent off; remove-trailing-space-save on; replace-tabs-save on; replace-tabs on; space-indent on;
// vim:set spell et sw=4 ts=4 nowrap cino=l1,cs,U1:

