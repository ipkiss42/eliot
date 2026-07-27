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

#include <fstream>
#include <algorithm>
#include <boost/format.hpp>
#include <pugixml.hpp>

#include "config.h"
#if ENABLE_NLS
#   include <libintl.h>
#   define _(String) gettext(String)
#else
#   define _(String) String
#endif

#include "xml_reader.h"
#include "dic.h"
#include "game_exception.h"
#include "game_params.h"
#include "game_factory.h"
#include "training.h"
#include "duplicate.h"
#include "freegame.h"
#include "player.h"
#include "ai_percent.h"
#include "encoding.h"
#include "cmd/game_rack_cmd.h"
#include "cmd/game_move_cmd.h"
#include "cmd/player_rack_cmd.h"
#include "cmd/player_move_cmd.h"
#include "cmd/player_event_cmd.h"
#include "cmd/master_move_cmd.h"
#include "navigation.h"
#include "header.h"

// Current version of our save game format. Bump it when it becomes
// incompatible (and keep it in sync with xml_writer.cpp)
#define CURRENT_XML_VERSION "2"

#define FMT1(s, a1) (boost::format(s) % (a1)).str()
#define FMT2(s, a1, a2) (boost::format(s) % (a1) % (a2)).str()


using namespace std;

INIT_LOGGER(game, XmlReader);


static wstring fromUtf8(const string &str)
{
    return readFromUTF8(str, "Loading game");
}


static int toInt(const string &str)
{
    if (str.empty())
        throw LoadGameException(_("Invalid string to int conversion: empty string received"));
    return atoi(str.c_str());
}


static void validateDictionary(const pugi::xml_node& dicNode, const Dictionary& iDic)
{
    // Validate the dictionary
    pugi::xml_node lettersNode = dicNode.child("Letters");
    if (lettersNode)
    {
        const wdstring & displayLetters = iDic.convertToDisplay(iDic.getHeader().getLetters());
        // Remove spaces
        string parsedLetters = lettersNode.text().get();
        string::iterator it = remove(parsedLetters.begin(), parsedLetters.end(), L' ');
        parsedLetters.erase(it, parsedLetters.end());
        // Compare
        if (displayLetters != fromUtf8(parsedLetters))
            throw LoadGameException(_("The current dictionary is different from the one used in the saved game"));
    }
    pugi::xml_node wordNbNode = dicNode.child("WordNb");
    if (wordNbNode)
    {
        if (iDic.getHeader().getNbWords() != (unsigned)toInt(wordNbNode.text().get()))
            throw LoadGameException(_("The current dictionary is different from the one used in the saved game"));
    }
}


static Game* createGame(const pugi::xml_node& gameNode, const Dictionary& iDic)
{
    GameParams params(iDic);

    // Parse the game mode
    string mode = gameNode.child("Mode").text().get();
    if (mode == "duplicate")
        params.setMode(GameParams::kDUPLICATE);
    else if (mode == "freegame")
        params.setMode(GameParams::kFREEGAME);
    else if (mode == "training")
        params.setMode(GameParams::kTRAINING);
    else if (mode == "arbitration")
        params.setMode(GameParams::kARBITRATION);
    else if (mode == "topping")
        params.setMode(GameParams::kTOPPING);
    else
        throw GameException("Invalid game mode: " + mode);

    // Parse the variants
    for (pugi::xml_node variantNode : gameNode.children("Variant")) {
        string variant = variantNode.text().get();
        if (variant == "bingo")
            params.addVariant(GameParams::kJOKER);
        else if (variant == "explosive")
            params.addVariant(GameParams::kEXPLOSIVE);
        else if (variant == "7among8")
            params.addVariant(GameParams::k7AMONG8);
        else if (variant != "")
            throw LoadGameException(FMT1(_("Invalid game variant: %1%"), variant));
    }

    // Create the game
    return GameFactory::Instance()->createGame(params);
}


static Player* createPlayer(const pugi::xml_node& playerNode)
{
    string playerId = playerNode.attribute("id").value();

    Player *p;
    string playerType = playerNode.child_value("Type");
    if (playerType == "human")
        p = new HumanPlayer();
    else if (playerType == "computer")
    {
        int level = toInt(playerNode.child_value("Level"));
        p = new AIPercent(0.01 * level);
    }
    else
        throw LoadGameException(FMT1(_("Invalid player type: %1%"), playerType));

    // Set the ID
    p->setId((unsigned int)toInt(playerId));

    // Set the name
    string name = playerNode.child_value("Name");
    p->setName(fromUtf8(name));

    // Set the table number
    string tableNb = playerNode.child_value("TableNb");
    if (tableNb != "")
        p->setTableNb(toInt(tableNb));

    return p;
}


static string readPlayerIdAttribute(const pugi::xml_node &node)
{
    // For backwards compatibility ("playerid" was used in saved games
    // before release 2.1, with XML version 2)
    if (node.attribute("playerid"))
        return node.attribute("playerid").value();

    return node.attribute("playerId").value();
}


static Player & getPlayer(map<unsigned int, Player*> &all_players,
                          const string &id, const string &iTag)
{
    int intId = toInt(id);
    if (all_players.find(intId) == all_players.end())
        throw LoadGameException(FMT2(_("Invalid player ID: %1% (processing tag '%2%')"), id, iTag));
    return *all_players[intId];
}


static Move buildMove(const Game &iGame, const pugi::xml_node &moveCmdNode, bool checkRack)
{
    string type = moveCmdNode.attribute("type").value();
    string word = moveCmdNode.attribute("word").value();
    string coord = moveCmdNode.attribute("coord").value();
    string letters = moveCmdNode.attribute("letters").value();
    // Build the Move object
    if (type == "valid")
    {
        wstring wword = iGame.getDic().convertFromInput(fromUtf8(word));
        Move move;
        int res = iGame.checkPlayedWord(fromUtf8(coord), wword, move, checkRack);
        if (res != 0)
        {
            throw LoadGameException(FMT2(_("Invalid move marked as valid: %1% (%2%)"),
                                         word, coord));
        }
        return move;
    }
    else if (type == "invalid")
    {
        return Move(fromUtf8(word), fromUtf8(coord));
    }
    else if (type == "change")
    {
        return Move(fromUtf8(letters));
    }
    else if (type == "pass")
    {
        return Move(L"");
    }
    else if (type == "none")
    {
        return Move();
    }
    else
        throw LoadGameException(FMT1(_("Invalid move type: %1%"), type));
}


Game * XmlReader::read(const string &iFileName, const Dictionary &iDic)
{
    LOG_INFO("Parsing savegame '" << iFileName << "'");

    // Load the XML file into memory
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(iFileName.c_str());
    if (!result)
        throw LoadGameException(FMT2(_("Cannot open file '%1%': %2%"), iFileName, result.description()));

    pugi::xml_node root = doc.child("EliotGame");
    if (!root || string(root.attribute("format").value()) != CURRENT_XML_VERSION)
    {
        LOG_ERROR("Incompatible save game format: current="
                  << CURRENT_XML_VERSION
                  << " savegame=" << root.attribute("format").value());
        throw LoadGameException(_("This saved game is not compatible with the current version of Eliot."));
    }

    validateDictionary(root.child("Dictionary"), iDic);

    pugi::xml_node gameNode = root.child("Game");
    Game *game = createGame(gameNode, iDic);

    map<unsigned int, Player*> all_players;
    for (pugi::xml_node playerNode : gameNode.children("Player")) {
        Player *player = createPlayer(playerNode);
        if (all_players.find(player->getId()) != all_players.end())
            throw LoadGameException(FMT1(_("A player ID must be unique: %1%"), player->getId()));
        all_players[player->getId()] = player;
        game->addPlayer(player);
    }

    bool firstTurn = true;
    for (pugi::xml_node turnNode : root.child("History").children("Turn"))
    {
        // End the previous turn
        if (firstTurn)
            firstTurn = false;
        else
            game->accessNavigation().newTurn();

        for (pugi::xml_node cmdNode : turnNode.children())
        {
            string tagName = cmdNode.name();
            string cmdText = cmdNode.text().get();
            if (tagName == "PlayerRack")
            {
                string letters = cmdNode.text().get();
                // Build a rack for the correct player
                const wstring &rackStr = iDic.convertFromInput(fromUtf8(cmdText));
                PlayedRack pldrack;
                if (!iDic.validateLetters(rackStr, L"-+"))
                {
                    throw LoadGameException(FMT1(_("Rack invalid for the current dictionary: %1%"), cmdText));
                }
                pldrack.setManual(rackStr);
                LOG_DEBUG("loaded rack: " << lfw(pldrack.toString()));

                Player &p = getPlayer(all_players, readPlayerIdAttribute(cmdNode), tagName);
                PlayerRackCmd *cmd = new PlayerRackCmd(p, pldrack);
                game->accessNavigation().addAndExecute(cmd);
                LOG_DEBUG("rack: " << lfw(pldrack.toString()));
            }
            else if (tagName == "GameRack")
            {
                // Build the game rack
                const wstring &rackStr = iDic.convertFromInput(fromUtf8(cmdText));
                PlayedRack pldrack;
                if (!iDic.validateLetters(rackStr, L"-+"))
                {
                    throw LoadGameException(FMT1(_("Rack invalid for the current dictionary: %1%"), cmdText));
                }
                pldrack.setManual(rackStr);
                LOG_DEBUG("loaded rack: " << lfw(pldrack.toString()));

                GameRackCmd *cmd = new GameRackCmd(*game, pldrack);
                game->accessNavigation().addAndExecute(cmd);
                LOG_DEBUG("rack: " << lfw(pldrack.toString()));
            }
            else if (tagName == "MasterMove")
            {
                const Move move = buildMove(*game, cmdNode, false);
                Duplicate *duplicateGame = dynamic_cast<Duplicate*>(game);
                if (duplicateGame == nullptr)
                {
                    throw LoadGameException(_("The 'MasterMove' tag should only be present for duplicate games"));
                }
                MasterMoveCmd *cmd = new MasterMoveCmd(*duplicateGame, move);
                game->accessNavigation().addAndExecute(cmd);
            }
            else if (tagName == "PlayerMove")
            {
                // FIXME: this is game-related logic. It should not be done here.
                bool isArbitrationGame = game->getParams().getMode() == GameParams::kARBITRATION;

                const Move move = buildMove(*game, cmdNode, /*XXX:true*/false);
                Player &p = getPlayer(all_players, readPlayerIdAttribute(cmdNode), tagName);
                PlayerMoveCmd *cmd = new PlayerMoveCmd(p, move, isArbitrationGame);
                game->accessNavigation().addAndExecute(cmd);
            }
            else if (tagName == "GameMove")
            {
                const Move move = buildMove(*game, cmdNode, false);
                GameMoveCmd *cmd = new GameMoveCmd(*game, move);
                game->accessNavigation().addAndExecute(cmd);
            }
            else if (tagName == "Warning")
            {
                Player &p = getPlayer(all_players, readPlayerIdAttribute(cmdNode), tagName);
                PlayerEventCmd *cmd = new PlayerEventCmd(p, PlayerEventCmd::WARNING);
                game->accessNavigation().addAndExecute(cmd);
            }
            else if (tagName == "Penalty")
            {
                Player &p = getPlayer(all_players, readPlayerIdAttribute(cmdNode), tagName);
                int points = cmdNode.attribute("points").as_int();
                PlayerEventCmd *cmd = new PlayerEventCmd(p, PlayerEventCmd::PENALTY, points);
                game->accessNavigation().addAndExecute(cmd);
            }
            else if (tagName == "Solo")
            {
                Player &p = getPlayer(all_players, readPlayerIdAttribute(cmdNode), tagName);
                int points = cmdNode.attribute("points").as_int();
                PlayerEventCmd *cmd = new PlayerEventCmd(p, PlayerEventCmd::SOLO, points);
                game->accessNavigation().addAndExecute(cmd);
            }
            else if (tagName == "EndGame")
            {
                Player &p = getPlayer(all_players, readPlayerIdAttribute(cmdNode), tagName);
                int points = cmdNode.attribute("points").as_int();
                PlayerEventCmd *cmd = new PlayerEventCmd(p, PlayerEventCmd::END_GAME, points);
                game->accessNavigation().addAndExecute(cmd);
            }
            else
            {
                LOG_ERROR("Unknown tag: " << tagName);
            }
        }
    }

    LOG_INFO("Savegame parsed successfully");
    return game;
}
