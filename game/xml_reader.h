/*****************************************************************************
 * Eliot
 * Copyright (C) 2009-2012 Olivier Teulière
 * Authors: Olivier Teulière  <ipkiss@via.ecp.fr>
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

#ifndef XML_READER_H_
#define XML_READER_H_

#include <map>

#include "config.h"

#include "logging.h"
#include "game_params.h"

class Dictionary;
class Game;
class Player;

using std::string;
using std::map;


class XmlReader
{
    DEFINE_LOGGER();
public:
    ~XmlReader() = default;

    /**
     * Only entry point of the class.
     * Create a Game object, from a XML file created using the XmlWriter class.
     * The method throws an exception in case of problem.
     */
    static Game * read(const string &iFileName, const Dictionary &iDic);

private:
    bool m_firstTurn{true};

    XmlReader(const XmlReader&);
    XmlReader& operator=(const XmlReader&);
    bool operator==(const XmlReader&);
};

#endif

