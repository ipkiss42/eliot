/*****************************************************************************
 * Eliot
 * Copyright (C) 2009-2012 Olivier Teulière
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

#include <QLineEdit>
#include <QPushButton>

#include "play_word_mediator.h"
#include "validator_factory.h"
#include "play_model.h"
#include "qtcommon.h"

#include "public_game.h"
#include "coord.h"
#include "dic.h"
#include "debug.h"


INIT_LOGGER(qt, PlayWordMediator);


PlayWordMediator::PlayWordMediator(QObject *parent, QLineEdit &iEditPlay,
                                   QLineEdit &iEditCoord, QLineEdit *iEditPoints,
                                   QPushButton &iButtonPlay,
                                   PlayModel &iPlayModel, PublicGame *iGame)
    : QObject(parent), m_game(iGame), m_lineEditPlay(iEditPlay),
    m_lineEditCoord(iEditCoord), m_lineEditPoints(iEditPoints),
    m_pushButtonPlay(iButtonPlay), m_playModel(iPlayModel)
{
    SetTooltips(m_lineEditPlay, m_lineEditCoord);

    blackPalette = m_lineEditPlay.palette();
    redPalette = m_lineEditPlay.palette();
    redPalette.setColor(QPalette::Text, Qt::red);

    /// Set validators;
    if (m_game)
    {
        m_lineEditPlay.setValidator(ValidatorFactory::newPlayWordValidator(this, m_game->getDic()));
        m_lineEditCoord.setValidator(ValidatorFactory::newCoordsValidator(this));
    }

    // Set all the connections
    QObject::connect(&m_lineEditPlay, SIGNAL(textChanged(const QString&)),
                     this, SLOT(onWordChanged()));
    QObject::connect(&m_lineEditPlay, SIGNAL(returnPressed()),
                     this, SLOT(playWord()));
    QObject::connect(&m_lineEditCoord, SIGNAL(textChanged(const QString&)),
                     this, SLOT(onCoordChanged()));
    QObject::connect(&m_lineEditCoord, SIGNAL(returnPressed()),
                     this, SLOT(playWord()));
    QObject::connect(&m_pushButtonPlay, SIGNAL(clicked()),
                     this, SLOT(playWord()));
    QObject::connect(&m_playModel, SIGNAL(coordChanged(const Coord&, const Coord&)),
                     this, SLOT(updateCoord(const Coord&)));

    // Initial state
    updatePointsAndState();
}


void PlayWordMediator::setCleverFocus()
{
    if (!m_lineEditPlay.hasFocus() && !m_lineEditCoord.hasFocus())
        m_lineEditPlay.setFocus();
}


void PlayWordMediator::SetTooltips(QLineEdit &iEditWord, QLineEdit &iEditCoord)
{
    // These strings cannot be in the .ui file, because of the newlines
    iEditWord.setToolTip(_q("Enter the word to play (case-insensitive).\n"
            "A joker from the rack must be written in parentheses.\n"
            "E.g.: w(o)rd or W(O)RD"));
    iEditCoord.setToolTip(_q("Enter the coordinates of the word.\n"
            "Specify the row before the column for horizontal words,\n"
            "and the column before the row for vertical words.\n"
            "E.g.: H4 or 4H"));
}


bool PlayWordMediator::GetPlayedWord(QLineEdit &iEditWord,
                                     const Dictionary &iDic,
                                     wstring *oPlayedWord,
                                     QString *oProblemCause)
{
    // Convert the jokers to lowercase
    const wistring &inputWord = wfq(iEditWord.text().toUpper());
    // Convert to internal representation, then back to QString
    QString word = qfw(iDic.convertFromInput(inputWord));

    int pos;
    while ((pos = word.indexOf('(')) != -1)
    {
        if (word.size() < pos + 3 || word[pos + 2] != ')' ||
            !iDic.validateLetters(wfq(QString(word[pos + 1]))))
        {
            // Cannot parse the string...
            *oPlayedWord = wfq(word);
            if (oProblemCause)
                *oProblemCause = _q("Cannot play word: misplaced parentheses");
            return false;
        }

        QChar chr = word[pos + 1].toLower();
        word.remove(pos, 3);
        word.insert(pos, chr);
    }

    // Convert the input string into an internal one
    *oPlayedWord = wfq(word);
    return true;
}


void PlayWordMediator::updatePointsAndState()
{
    bool acceptableInput =
        m_lineEditPlay.hasAcceptableInput() &&
        m_lineEditCoord.hasAcceptableInput();
    m_pushButtonPlay.setEnabled(acceptableInput);

    if (m_lineEditPoints)
    {
        if (!acceptableInput)
            m_lineEditPoints->clear();
        else
        {
            // Compute the points of the word
            const wstring &word = getWord();
            const wstring &coords = wfq(m_lineEditCoord.text());
            int points = m_game->computePoints(word, coords);
            if (points >= 0)
                m_lineEditPoints->setText(QString("%1").arg(points));
            else
                m_lineEditPoints->setText("#");
        }
    }
}


void PlayWordMediator::playWord()
{
    if (!m_lineEditPlay.hasAcceptableInput() ||
        !m_lineEditCoord.hasAcceptableInput())
        return;

    const wstring &word = getWord();
    QString coords = m_lineEditCoord.text();
    m_playModel.playWord(word, wfq(coords));
}


void PlayWordMediator::onCoordChanged()
{
    Coord coord(wfq(m_lineEditCoord.text()));
    m_playModel.setCoord(coord);
    onWordChanged();
}


void PlayWordMediator::onWordChanged()
{
    bool acceptableInput = m_lineEditPlay.hasAcceptableInput() || m_lineEditPlay.text() == "";
    m_lineEditPlay.setPalette(acceptableInput ? blackPalette : redPalette);

    if (acceptableInput)
    {
        wstring playedWord;
        GetPlayedWord(m_lineEditPlay, m_game->getDic(), &playedWord, NULL);

        Move move;
        m_game->checkPlayedWord(playedWord, wfq(m_lineEditCoord.text()), move);
        m_playModel.setMove(move);
        updatePointsAndState();
    }
}


void PlayWordMediator::updateCoord(const Coord &iNewCoord)
{
    // Ignore updates to non-visible controls (which happens when there
    // are several human players).
    // It fixes a focus problem.
    if (!m_lineEditPlay.isVisible())
        return;

    if (iNewCoord.isValid() && m_lineEditCoord.text() != qfw(iNewCoord.toString()))
        m_lineEditCoord.setText(qfw(iNewCoord.toString()));
    else if (!iNewCoord.isValid() && m_lineEditCoord.hasAcceptableInput())
        m_lineEditCoord.setText("");
    // Set the focus to the "Play word" zone if the focus is not
    // already on a QLineEdit (because of a click on the board)
    setCleverFocus();

    updatePointsAndState();
}


wstring PlayWordMediator::getWord()
{
    wstring playedWord;
    QString msg;
    // Avoid a warning in non debug mode (unused variable)
#ifdef DEBUG
    bool ok =
#endif
        GetPlayedWord(m_lineEditPlay, m_game->getDic(), &playedWord, &msg);
    // The method should only be called for a correct input
    ASSERT(ok, "Invalid word (should not be possible)");
    return playedWord;
}

