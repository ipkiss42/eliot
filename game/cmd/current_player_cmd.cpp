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

#include <format>

#include "cmd/current_player_cmd.h"
#include "game.h"


INIT_LOGGER(game, CurrentPlayerCmd);


CurrentPlayerCmd::CurrentPlayerCmd(Game &ioGame, unsigned int iPlayerId)
    : m_game(ioGame), m_newPlayerId(iPlayerId)
{
}


void CurrentPlayerCmd::doExecute()
{
    m_oldPlayerId = m_game.currPlayer();
    m_game.setCurrentPlayer(m_newPlayerId);
}


void CurrentPlayerCmd::doUndo()
{
    m_game.setCurrentPlayer(m_oldPlayerId);
}


wstring CurrentPlayerCmd::toString() const
{
    std::wstring old_player_info = isExecuted()
        ? std::format(L"  old player: {}", m_oldPlayerId)
        : L"";

    return std::format(L"CurrentPlayerCmd (new player: {}{})", m_newPlayerId, old_player_info);
}

