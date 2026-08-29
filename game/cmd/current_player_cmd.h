/*******************************************************************
 * Eliot
 * Copyright (C) 2026 Olivier Teulière
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

#ifndef CURRENT_PLAYER_CMD_H_
#define CURRENT_PLAYER_CMD_H_

#include "command.h"
#include "logging.h"

class Game;

/**
 * This class implements the Command design pattern.
 * It is used to keep track of the current player changes.
 */
class CurrentPlayerCmd: public Command
{
    DEFINE_LOGGER();

    public:
        CurrentPlayerCmd(Game &ioGame, unsigned int iPlayerId);
        unsigned int getNewPlayerId() const { return m_newPlayerId; }

        wstring toString() const override;

    protected:
        void doExecute() override;
        void doUndo() override;

    private:
        Game &m_game;
        unsigned int m_newPlayerId;
        unsigned int m_oldPlayerId{0};
};

#endif

