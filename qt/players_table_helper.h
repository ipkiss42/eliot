/*****************************************************************************
 * Eliot
 * Copyright (C) 2012 Olivier Teulière
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

#ifndef PLAYERS_TABLE_HELPER_H_
#define PLAYERS_TABLE_HELPER_H_

#include <QWidget>
#include <QItemDelegate>
#include <QString>
#include <QList>

#include "logging.h"


class QTableWidget;
class QPushButton;
class QMenu;
class QPoint;


struct PlayerDef
{
    PlayerDef(QString name, QString type, QString level, bool isDefault);
    QString name;
    QString type;
    QString level;
    bool isDefault;

    bool operator==(const PlayerDef &) const;
};


class PlayersTableHelper : public QObject
{
    Q_OBJECT;
    DEFINE_LOGGER();

public:
    /// Possible values for the player type
    static const char * kHUMAN;
    static const char * kAI;

    PlayersTableHelper(QObject *parent,
                       QTableWidget *tableWidget,
                       QPushButton *addButton = nullptr,
                       QPushButton *removeButton = nullptr,
                       bool showDefaultColumn = false);

    /// Associate up/down buttons
    void setUpDown(QPushButton *iButtonUp, QPushButton *iButtonDown);

    QList<PlayerDef> getPlayers(bool onlySelected) const;
    void addPlayers(const QList<PlayerDef> &iList);
    void addPlayer(const PlayerDef &iPlayer,
                   bool selectAndEdit = false,
                   bool renameIfDuplicate = false);

    static QList<PlayerDef> getFavPlayers();
    static void saveFavPlayers(const QList<PlayerDef> &iFavPlayers);

    int getRowCount() const;

    void addPopupAction(QAction *iAction);
    void addPopupRemoveAction();

signals:
    void rowCountChanged();

private slots:
    void populateMenu(QMenu &, const QPoint &);
    /// Enable selection-dependent buttons
    void enableSelDepButtons();
    void removeSelectedRows();
    void addRow();
    void addRow(const PlayerDef &iDef);
    void moveSelectionUp();
    void moveSelectionDown();

private:
    QTableWidget *m_tablePlayers;
    QPushButton *m_buttonAdd;
    QPushButton *m_buttonRemove;
    QList<QAction*> m_popupActions;
    bool m_showDefaultColumn;
    // Up/down buttons (optional)
    QPushButton *m_buttonUp;
    QPushButton *m_buttonDown;

    /// Return a "normalized" player def, i.e. with correct values
    PlayerDef normalize(const PlayerDef &iDef) const;

    /// Move row at index rowFrom to index rowTo
    void moveRow(int rowFrom, int rowTo);
};


/// Delegate used for the edition of the players type
class PlayersTypeDelegate: public QItemDelegate
{
    Q_OBJECT;

public:
    explicit PlayersTypeDelegate(QObject *parent = nullptr);
    ~PlayersTypeDelegate() override {}

    // Implement the needed methods
    QWidget *createEditor(QWidget *parent,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const override;
    void setEditorData(QWidget *editor,
                               const QModelIndex &index) const override;
    void setModelData(QWidget *editor,
                              QAbstractItemModel *model,
                              const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const override;
};


/// Delegate used for the edition of the players level
class PlayersLevelDelegate: public QItemDelegate
{
    Q_OBJECT;

public:
    explicit PlayersLevelDelegate(QObject *parent = nullptr);
    ~PlayersLevelDelegate() override {}

    // Implement the needed methods
    QWidget *createEditor(QWidget *parent,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const override;
    void setEditorData(QWidget *editor,
                               const QModelIndex &index) const override;
    void setModelData(QWidget *editor,
                              QAbstractItemModel *model,
                              const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const override;
};

#endif

