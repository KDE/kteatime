/*
    SPDX-FileCopyrightText: 2007-2009 Stefan Böhmann <kde@hilefoks.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef TEALISTMODEL_H
#define TEALISTMODEL_H

#include <QAbstractTableModel>

class Tea;

/**
 * @short provides an model used by SettingsDialog
 *
 * @author Stefan Böhmann <kde@hilefoks.org>
 */
class TeaListModel : public QAbstractTableModel
{
public:
    /**
     * Constructs an TeaListModel with the given list of Teas and for the given parent.
     * @param teas the initial list of teas to manage.
     * @param parent the parent object.
     */
    explicit TeaListModel(const QList<Tea> &teas, QObject *parent = nullptr);

    /**
     * Returns the index of the item in the model specified by the given row, column and parent index.
     * @param row the row.
     * @param column the column.
     * @param parent will be ignored by this model.
     *
     * @return @ref QModelIndex with the index of the item.
     */
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    /**
     * Returns the number of rows.
     * @param parent will be ignored by this model.
     *
     * @return the number of rows.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * Returns the number of columns.
     * @param parent will be ignored by this model.
     *
     * @return the number of columns.
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * Returns the data stored under the given role for the item referred to by the index.
     * @param index the index of the item.
     * @param role the role
     *
     * @return the specified data.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * Sets the role data for the item at index to value.
     * @param index the index of the item.
     * @param value the new value for the item.
     * @param role the role.
     *
     * @return if successful true, otherwise false.
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    /**
     * Returns the data for the given role and section in the header with the specified orientation.
     * @param section the section.
     * @param orientation the orientation.
     * @param role the role.
     *
     * @return the specified data.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * inserts rows into the model before the given one.
     *
     * @param row the row - if 0 the new rows will be insert before any exists rows.
     * @param count number of rows to add.
     * @param parent will be ignored by this model.
     *
     * @return true if the rows were successfully inserted, otherwise false.
     */
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    /**
     * removes rows from the model, starting with the given row.
     * @param row the first row to remove.
     * @param count number of rows to remove.
     * @param parent will be ignored by this model.
     *
     * @return true if the rows were successfully removed, otherwise false.
     */
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    /**
     * Returns the whole list of teas.
     *
     * @return list of teas.
     */
    const QList<Tea> getTeaList() const;

private:
    QList<Tea> m_tealist;
};

#endif
