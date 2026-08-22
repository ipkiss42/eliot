/*****************************************************************************
 * Eliot
 * Copyright (C) 2005-2012 Antoine Fraboulet & Olivier Teulière
 * Authors: Antoine Fraboulet <antoine.fraboulet @@ free.fr>
 *          Olivier Teulière <ipkiss @@ gmail.com>
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
#include <regex>
#include <string>

#include "coord.h"
#include "board.h" // for BOARD_MIN and BOARD_MAX (TODO: remove this include)
#include "debug.h"
#include "encoding.h"


INIT_LOGGER(game, Coord);


Coord::Coord(int iRow, int iCol, Direction iDir)
{
    m_row = iRow;
    m_col = iCol;
    m_dir = iDir;
}

Coord::Coord(const wstring &iStr)
{
    setFromString(iStr);
}

bool Coord::isValid() const
{
    return (m_row >= BOARD_MIN && m_row <= BOARD_MAX &&
            m_col >= BOARD_MIN && m_col <= BOARD_MAX);
}


bool Coord::operator==(const Coord &iOther) const
{
    if (!isValid() && !iOther.isValid())
        return true;
    return m_row == iOther.m_row
        && m_col == iOther.m_col
        && m_dir == iOther.m_dir;
}


void Coord::swap()
{
    int tmp = m_col;
    m_col = m_row;
    m_row = tmp;
}


void Coord::setFromString(const wstring &iWStr)
{
    static const std::wregex horizPattern(L"^([a-oA-O])([0-9]{1,2})$");
    static const std::wregex vertPattern(L"^([0-9]{1,2})([a-oA-O])$");

    wchar_t row = L'\0';
    int col = -1;

    std::wsmatch matches;
    if (std::regex_match(iWStr, matches, horizPattern))
    {
        setDir(HORIZONTAL);
        row = matches[1].str()[0];
        col = std::stoi(matches[2].str());
    }
    else if (std::regex_match(iWStr, matches, vertPattern))
    {
        setDir(VERTICAL);
        col = std::stoi(matches[1].str());
        row = matches[2].str()[0];
    }
    else
    {
        row = L'A' - 1;
    }

    // std::towupper handles the wide character directly (from <cwctype>)
    row = std::towupper(row) - L'A' + 1;
    setCol(col);
    setRow(row);
}


wstring Coord::toString() const
{
    ASSERT(isValid(), "Invalid coordinates");

    // Convert the numeric row to its corresponding wide character (e.g. 1 -> L'A')
    auto rowChar = static_cast<wchar_t>(m_row + L'A' - 1);

    // Format directly into a safe, dynamically allocated std::wstring
    if (getDir() == HORIZONTAL)
    {
        return std::format(L"{}{}", rowChar, m_col);
    }
    else
    {
        return std::format(L"{}{}", m_col, rowChar);
    }
}

