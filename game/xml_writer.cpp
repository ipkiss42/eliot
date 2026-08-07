/*******************************************************************
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

#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <boost/format.hpp>
#include <pugixml.hpp>

#include "config.h"
#if ENABLE_NLS
#   include <libintl.h>
#   define _(String) gettext(String)
#else
#   define _(String) String
#endif

#include "xml_writer.h"
#include "encoding.h"
#include "turn.h"
#include "turn_data.h"
#include "game_params.h"
#include "game.h"
#include "player.h"
#include "ai_percent.h"
#include "game_exception.h"
#include "turn.h"
#include "cmd/game_rack_cmd.h"
#include "cmd/game_move_cmd.h"
#include "cmd/player_rack_cmd.h"
#include "cmd/player_move_cmd.h"
#include "cmd/player_event_cmd.h"
#include "cmd/master_move_cmd.h"
#include "cmd/topping_move_cmd.h"
#include "dic.h"
#include "header.h"

// Current version of our save game format. Bump it when it becomes
// incompatible (and keep it in sync with xml_reader.cpp)
#define CURRENT_XML_VERSION "2"

#define FMT1(s, a1) (boost::format(s) % (a1)).str()
#define FMT2(s, a1, a2) (boost::format(s) % (a1) % (a2)).str()


using namespace std;

INIT_LOGGER(game, XmlWriter);


static string toUtf8(const wstring &s)
{
    return writeInUTF8(s, "Saving game");
}

static void writeMove(pugi::xml_node & parentNode, const Move &iMove,
                      const string &iTag, int iPlayerId)
{
    pugi::xml_node moveNode = parentNode.append_child(iTag);
    if (iPlayerId != -1)
        moveNode.append_attribute("playerId").set_value(iPlayerId);
    moveNode.append_attribute("points").set_value(iMove.getScore());
    if (iMove.isValid())
    {
        const Round &round = iMove.getRound();
        moveNode.append_attribute("type").set_value("valid");
        moveNode.append_attribute("word").set_value(toUtf8(round.getWord()));
        moveNode.append_attribute("coord").set_value(toUtf8(round.getCoord().toString()));
    }
    else if (iMove.isInvalid())
    {
        moveNode.append_attribute("type").set_value("invalid");
        moveNode.append_attribute("word").set_value(toUtf8(iMove.getBadWord()));
        moveNode.append_attribute("coord").set_value(toUtf8(iMove.getBadCoord()));
    }
    else if (iMove.isChangeLetters()) {
        moveNode.append_attribute("type").set_value("change");
        moveNode.append_attribute("letters").set_value(toUtf8(iMove.getChangedLetters()));
    }
    else if (iMove.isPass())
        moveNode.append_attribute("type").set_value("pass");
    else if (iMove.isNull())
        moveNode.append_attribute("type").set_value("none");
    else
        throw SaveGameException(FMT1(_("Unsupported move: %1%"), lfw(iMove.toString())));
}


void XmlWriter::write(const Game& iGame, const std::string& iFileName)
{
    pugi::xml_document doc;

    // XML Declaration Header Block
    pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "UTF-8";

    // Root element
    pugi::xml_node root = doc.append_child("EliotGame");
    root.append_attribute("format") = CURRENT_XML_VERSION;

    // Write the dictionary information
    pugi::xml_node dictNode = root.append_child("Dictionary");
    const Dictionary& dict = iGame.getDic();
    const Header &header = iGame.getDic().getHeader();
    dictNode.append_child("Name").text().set(toUtf8(header.getName()));
    string dictType;
    if (header.getType() == Header::kDAWG)
        dictType = "dawg";
    else if (header.getType() == Header::kGADDAG)
        dictType = "gaddag";
    else
        throw SaveGameException(_("Invalid dictionary type"));
    dictNode.append_child("Type").text().set(dictType);

    // Retrieve the dictionary letters, ans separate them with spaces
    wstring lettersWithSpaces = header.getLetters();
    for (size_t i = lettersWithSpaces.size() - 1; i > 0; --i)
        lettersWithSpaces.insert(i, 1, L' ');
    // Convert to a display string
    const wstring &displayLetters = dict.convertToDisplay(lettersWithSpaces);
    dictNode.append_child("Letters").text().set(toUtf8(displayLetters));
    dictNode.append_child("WordNb").text().set(std::to_string(header.getNbWords()));

    // ------------------------
    // Write the game header
    pugi::xml_node gameNode = root.append_child("Game");
    // Game type
    string mode;
    if (iGame.getMode() == GameParams::kDUPLICATE)
        mode = "duplicate";
    else if (iGame.getMode() == GameParams::kFREEGAME)
        mode = "freegame";
    else if (iGame.getMode() == GameParams::kARBITRATION)
        mode = "arbitration";
    else if (iGame.getMode() == GameParams::kTOPPING)
        mode = "topping";
    else
        mode = "training";
    gameNode.append_child("Mode").text().set(mode);

    // Game variant
    if (iGame.getParams().hasVariant(GameParams::kJOKER))
        gameNode.append_child("Variant").text().set("bingo");
    if (iGame.getParams().hasVariant(GameParams::kEXPLOSIVE))
        gameNode.append_child("Variant").text().set("explosive");
    if (iGame.getParams().hasVariant(GameParams::k7AMONG8))
        gameNode.append_child("Variant").text().set("7among8");

    // Players
    for (unsigned int i = 0; i < iGame.getNPlayers(); ++i)
    {
        const Player& player = iGame.getPlayer(i);
        pugi::xml_node pNode = gameNode.append_child("Player");
        pNode.append_attribute("id").set_value(player.getId());

        pNode.append_child("Name").text().set(toUtf8(player.getName()));
        pNode.append_child("Type").text().set(player.isHuman() ? "human" : "computer");
        if (!player.isHuman())
        {
            const AIPercent *ai = dynamic_cast<const AIPercent *>(&player);
            if (ai == nullptr)
                throw SaveGameException(FMT1(_("Invalid player type for player %1%"), i));
            pNode.append_child("Level").text().set(std::to_string(lrint(ai->getPercent() * 100)));
        }
        pNode.append_child("TableNb").text().set(std::to_string(player.getTableNb()));
    }

    // Number of turns
    gameNode.append_child("Turns").text().set(
        std::to_string(iGame.getNavigation().getNbTurns())
    );
    // End of the header
    // ------------------------

    // ------------------------
    // Write the game history
    pugi::xml_node historyNode = root.append_child("History");

#if 0
    iGame.getNavigation().print();
#endif
    const vector<Turn *> &turnVect = iGame.getNavigation().getTurns();
    for (const Turn *turn : turnVect)
    {
        if (turn->getCommands().empty() && turn == turnVect.back())
            continue;

        pugi::xml_node turnNode = historyNode.append_child("Turn");
        for (const Command *cmd : turn->getCommands())
        {
            if (dynamic_cast<const GameRackCmd*>(cmd))
            {
                const GameRackCmd *rackCmd = static_cast<const GameRackCmd*>(cmd);
                turnNode.append_child("GameRack").text().set(toUtf8(rackCmd->getRack().toString()));
            }
            else if (dynamic_cast<const PlayerRackCmd*>(cmd))
            {
                const PlayerRackCmd *rackCmd = static_cast<const PlayerRackCmd*>(cmd);
                unsigned int id = rackCmd->getPlayer().getId();
                pugi::xml_node pRack = turnNode.append_child("PlayerRack");
                pRack.append_attribute("playerId").set_value(id);
                pRack.text().set(toUtf8(rackCmd->getRack().toString()));
            }
            else if (dynamic_cast<const PlayerMoveCmd*>(cmd))
            {
                const PlayerMoveCmd *moveCmd = static_cast<const PlayerMoveCmd*>(cmd);
                unsigned int id = moveCmd->getPlayer().getId();
                writeMove(turnNode, moveCmd->getMove(), "PlayerMove", id);
            }
            else if (dynamic_cast<const GameMoveCmd*>(cmd))
            {
                const GameMoveCmd *moveCmd = static_cast<const GameMoveCmd*>(cmd);
                writeMove(turnNode, moveCmd->getMove(), "GameMove", -1);
            }
            else if (dynamic_cast<const MasterMoveCmd*>(cmd))
            {
                const MasterMoveCmd *moveCmd = static_cast<const MasterMoveCmd*>(cmd);
                writeMove(turnNode, moveCmd->getMove(), "MasterMove", -1);
            }
            else if (dynamic_cast<const ToppingMoveCmd*>(cmd))
            {
                const ToppingMoveCmd *moveCmd = static_cast<const ToppingMoveCmd*>(cmd);
                unsigned int id = moveCmd->getPlayerId();
                writeMove(turnNode, moveCmd->getMove(), "ToppingMove", id);
            }
            else if (dynamic_cast<const PlayerEventCmd*>(cmd))
            {
                const PlayerEventCmd *eventCmd = static_cast<const PlayerEventCmd*>(cmd);
                unsigned int id = eventCmd->getPlayer().getId();
                int value = eventCmd->getPoints();
                // Warnings
                if (eventCmd->getEventType() == PlayerEventCmd::WARNING)
                {
                    turnNode.append_child("Warning").append_attribute("playerId").set_value(id);
                }
                // Penalties
                else if (eventCmd->getEventType() == PlayerEventCmd::PENALTY)
                {
                    pugi::xml_node penaltyNode = turnNode.append_child("Penalty");
                    penaltyNode.append_attribute("playerId").set_value(id);
                    penaltyNode.append_attribute("points").set_value(value);
                }
                // Solos
                else if (eventCmd->getEventType() == PlayerEventCmd::SOLO)
                {
                    pugi::xml_node soloNode = turnNode.append_child("Solo");
                    soloNode.append_attribute("playerId").set_value(id);
                    soloNode.append_attribute("points").set_value(value);
                }
                // End game bonuses (freegame mode)
                else if (eventCmd->getEventType() == PlayerEventCmd::END_GAME)
                {
                    pugi::xml_node endGameNode = turnNode.append_child("EndGame");
                    endGameNode.append_attribute("playerId").set_value(id);
                    endGameNode.append_attribute("points").set_value(value);
                }
                else
                {
                    LOG_ERROR("Unknown event type: {}", static_cast<int>(eventCmd->getEventType()));
                }
            }
            else
            {
                LOG_ERROR("Unsupported command: {}", lfw(cmd->toString()));
                turnNode.append_child(pugi::node_comment).text().set(
                    FMT1("FIXME: Unsupported command: %1%", lfw(cmd->toString())));
                // XXX
                //throw SaveGameException(FMT1(_("Unsupported command: %1%"), lfw(cmd->toString())));
            }
        }
    }
    // End of the game history
    // ------------------------

    // Statistics
    root.append_child(pugi::node_comment).set_value(
        " These statistics are simply informative, they are not used when loading a game. "
    );
    pugi::xml_node statsNode = root.append_child("Statistics");

    // Compute the total number of points in the game
    int gameTotal = 0;
    for (unsigned i = 0; i < iGame.getHistory().getSize(); ++i)
    {
        gameTotal += iGame.getHistory().getTurn(i).getMove().getScore();
    }

    pugi::xml_node gStats = statsNode.append_child("GameStats");
    gStats.append_attribute("totalScore").set_value(gameTotal);

    for (unsigned int i = 0; i < iGame.getNPlayers(); ++i)
    {
        const Player &player = iGame.getPlayer(i);
        const int playerTotal = player.getTotalScore();

        // Compute the percentage, compared to the top
        long int percentage = 0;
        if (gameTotal != 0)
            percentage = lround((double)100 * playerTotal / gameTotal);

        // Compute the rank of the player (naive algorithm)
        int rank = 1;
        for (unsigned j = 0; j < iGame.getNPlayers(); ++j)
        {
            if (i == j)
                continue;
            if (playerTotal < iGame.getPlayer(j).getTotalScore())
                ++rank;
        }

        pugi::xml_node psNode = statsNode.append_child("PlayerStats");
        psNode.append_attribute("playerId").set_value(player.getId());
        psNode.append_attribute("rawScore").set_value(player.getMovePoints());
        psNode.append_attribute("warningsNb").set_value(player.getWarningsNb());
        psNode.append_attribute("penaltiesPoints").set_value(player.getPenaltyPoints());
        psNode.append_attribute("solosPoints").set_value(player.getSoloPoints());
        psNode.append_attribute("totalScore").set_value(playerTotal);
        psNode.append_attribute("diffWithTop").set_value(playerTotal - gameTotal);
        psNode.append_attribute("percentTop").set_value(percentage);
        psNode.append_attribute("rank").set_value(rank);
    }
    // End of the statistics

    //. Save file with precise 4-space layout indentation
    bool success = doc.save_file(iFileName.c_str(), "    ", pugi::format_default, pugi::encoding_utf8);
    if (!success)
    {
        throw SaveGameException(FMT1(_("Cannot open file for writing: '%1%'"), iFileName));
    }
}

