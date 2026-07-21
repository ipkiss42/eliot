/*****************************************************************************
 * Eliot
 * Copyright (C) 2010-2012 Olivier Teulière
 * Authors: Olivier Teulière <ipkiss @@ gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *****************************************************************************/

#ifndef TILE_LAYOUT_H_
#define TILE_LAYOUT_H_

#include <QLayout>

#include "logging.h"


class TileLayout : public QLayout
{
    Q_OBJECT;
    DEFINE_LOGGER();

public:
    TileLayout(int nbRows = 0, int nbCols = 0);
    ~TileLayout() override;

    void clear();

    QRect getBoardRect() const;

    int getSquareSize() const;

    void addItem(QLayoutItem *item) override { m_items.append(item); }
    int count() const override { return m_items.size(); }
    QLayoutItem *itemAt(int index) const override { return m_items.value(index); }
    QLayoutItem *takeAt(int index) override;
    QSize minimumSize() const override;
    QSize sizeHint() const override;
    void setGeometry(const QRect &rect) override;

private:
    QList<QLayoutItem *> m_items;
    bool m_dynamicRow;
    bool m_dynamicCol;
    int m_nbCols;
    int m_nbRows;
};

#endif

