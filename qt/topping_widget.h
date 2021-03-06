/*****************************************************************************
 * Eliot
 * Copyright (C) 2012-2013 Olivier Teulière
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

#ifndef TOPPING_WIDGET_H_
#define TOPPING_WIDGET_H_

#include <QWidget>
#include "ui/topping_widget.ui.h"
#include "logging.h"


class QStandardItemModel;
class QString;
class PlayModel;
class TimerModel;
class PlayWordMediator;
class PublicGame;
class HintsDialog;
class AbstractHint;

class ToppingWidget: public QWidget, private Ui::ToppingWidget
{
    Q_OBJECT;
    DEFINE_LOGGER();

public:
    explicit ToppingWidget(QWidget *parent, PlayModel &iPlayModel,
                           TimerModel &iTimerModel, PublicGame *iGame);

public slots:
    void onNewTurn();
    void refresh();

signals:
    void gameUpdated();
    void notifyProblem(QString iMsg);
    void notifyInfo(QString iMsg);

protected:
    /// Define a default size
    virtual QSize sizeHint() const;

private slots:
    void lockSizesChanged(bool checked);
    void shuffle();
    void showHintsDialog();
    void playWord(const wstring &iWord, const wstring &iCoord);
    void hintUsed(const AbstractHint&);
    void timeoutPenalty();

private:
    /// Encapsulated training game, can be NULL
    PublicGame *m_game;

    /// Indicate whether the columns should be resized after a search
    bool m_autoResizeColumns;

    /// Model of the search results
    QStandardItemModel *m_model;

    /// Mediator for the "play word" controls
    PlayWordMediator *m_mediator;

    TimerModel *m_timerModel;

    /// Dialog used to display hints
    HintsDialog *m_hintsDialog;

    /// Palette to write text in black
    QPalette blackPalette;

    /// Palette to write text in red
    QPalette redPalette;

    /// Force synchronizing the model with the contents of the search results
    void updateModel();

};

#endif

