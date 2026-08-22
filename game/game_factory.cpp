/*****************************************************************************
 * Eliot
 * Copyright (C) 2005-2012 Olivier Teulière & Antoine Fraboulet
 * Authors: Olivier Teulière <ipkiss @@ gmail.com>
 *          Antoine Fraboulet <antoine.fraboulet @@ free.fr>
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

#include "config.h"

#include <getopt.h>
#include <string>
#include <iostream>
#include <exception>

#if ENABLE_NLS
#   include <libintl.h>
#   define _(String) gettext(String)
#else
#   define _(String) String
#endif

#include "game_factory.h"
#include "game_params.h"
#include "game.h"
#include "training.h"
#include "freegame.h"
#include "duplicate.h"
#include "arbitration.h"
#include "topping.h"
#include "player.h"
#include "ai_percent.h"
#include "dic.h"
#include "encoding.h"
#include "xml_reader.h"


INIT_LOGGER(game, GameFactory);

GameFactory *GameFactory::m_factory = nullptr;


GameFactory::GameFactory() = default;


GameFactory::~GameFactory()
{
    delete m_dic;
}


GameFactory *GameFactory::Instance()
{
    if (m_factory == nullptr)
        m_factory = new GameFactory;
    return m_factory;
}


void GameFactory::Destroy()
{
    delete m_factory;
    m_factory = nullptr;
}


Game *GameFactory::createGame(const GameParams &iParams, const Game *iMasterGame)
{
    if (iParams.getMode() == GameParams::kTRAINING)
    {
        LOG_INFO("Creating a training game");
        auto *game = new Training(iParams, iMasterGame);
        return game;
    }
    if (iParams.getMode() == GameParams::kFREEGAME)
    {
        LOG_INFO("Creating a free game");
        auto *game = new FreeGame(iParams, iMasterGame);
        return game;
    }
    if (iParams.getMode() == GameParams::kDUPLICATE)
    {
        LOG_INFO("Creating a duplicate game");
        auto *game = new Duplicate(iParams, iMasterGame);
        return game;
    }
    if (iParams.getMode() == GameParams::kARBITRATION)
    {
        LOG_INFO("Creating an arbitration game");
        auto *game = new Arbitration(iParams, iMasterGame);
        return game;
    }
    if (iParams.getMode() == GameParams::kTOPPING)
    {
        LOG_INFO("Creating a topping game");
        auto *game = new Topping(iParams, iMasterGame);
        return game;
    }
    throw GameException("Unknown game type");
}


Game *GameFactory::createFromCmdLine(int argc, char **argv)
{
    // 1) Parse command-line and store everything in member variables
    static const std::array<struct option, 9> long_options = {{
        {.name="help", .has_arg=no_argument, .flag=nullptr, .val='h'},
        {.name="version", .has_arg=no_argument, .flag=nullptr, .val='v'},
        {.name="dictionary", .has_arg=required_argument, .flag=nullptr, .val='d'},
        {.name="dict", .has_arg=required_argument, .flag=nullptr, .val='d'},
        {.name="mode", .has_arg=required_argument, .flag=nullptr, .val='m'},
        {.name="human", .has_arg=required_argument, .flag=nullptr, .val='u'},
        {.name="ai", .has_arg=required_argument, .flag=nullptr, .val='a'},
        {.name="joker", .has_arg=no_argument, .flag=nullptr, .val=500},
        {.name=nullptr, .has_arg=0, .flag=nullptr, .val=0}
    }};
    static const auto short_options = std::to_array("hvd:m:u:a:");

    int option_index = 1;
    int res;
    bool found_d = false;
    bool found_m = false;
    while ((res = getopt_long(argc, argv, short_options.data(),
                              long_options.data(), &option_index)) != -1)
    {
        switch (res)
        {
        case 'h':
            // Help requested, display it and exit
            printUsage(argv[0]);
            return nullptr;
        case 'v':
            // Version requested, display it and exit
            printVersion();
            return nullptr;
        case 'd':
            m_dicStr = optarg;
            found_d = true;
            break;
        case 'm':
            m_modeStr = optarg;
            found_m = true;
            break;
        case 'u':
        case 'a':
            // Handle both types of players together
            {
                wstring name;
                if (optarg == nullptr)
                {
                    string s = _fmt(_("Player {0}"), m_players.size() + 1);
                    name = wfl(s);
                }
                else
                    name = wfl(optarg);
                m_players.emplace_back(res == 'u', name);
            }
            break;
        case 500:
            m_joker = true;
            break;
        }
    }

    // 2) Make sure the mandatory options are present
    if (!found_d || !found_m)
    {
        cerr << "Mandatory option missing: ";
        if (!found_d)
            cerr << "dict";
        else if (!found_m)
            cerr << "mode";
        cerr << endl;

        printUsage(argv[0]);
        return nullptr;
    }

    // 3) Try to load the dictionary
    try
    {
        m_dic = new Dictionary(m_dicStr);
    }
    catch (std::exception &e)
    {
        cerr << e.what() << endl;
        return nullptr;
    }

    // 4) Prepare game parameters
    GameParams params(*m_dic);
    if (m_joker)
        params.addVariant(GameParams::kJOKER);

    if (m_modeStr == "training" || m_modeStr == "t")
        params.setMode(GameParams::kTRAINING);
    else if (m_modeStr == "freegame" || m_modeStr == "f")
        params.setMode(GameParams::kFREEGAME);
    else if (m_modeStr == "duplicate" || m_modeStr == "d")
        params.setMode(GameParams::kDUPLICATE);
    else
    {
        cerr << "Invalid game mode '" << m_modeStr << "'" << endl;
        return nullptr;
    }

    // 5) Try to create a game object
    Game *game = createGame(params);

    // 6) Add the players
    for (auto & player_and_name : m_players)
    {
        // Human?
        Player *new_player;
        if (player_and_name.first)
            new_player = new HumanPlayer;
        else
            new_player = new AIPercent(1);
        new_player->setName(player_and_name.second);
        game->addPlayer(new_player);
    }

    return game;
}


Game* GameFactory::load(const string &iFileName, const Dictionary &iDic)
{
    return XmlReader::read(iFileName, iDic);
}


void GameFactory::printUsage(const string &iBinaryName) const
{
    cout << "Usage: " << iBinaryName << " [options]" << endl
         << "Options:" << endl
         << "  -h, --help               Print this help and exit" << endl
         << "  -v, --version            Print version information and exit" << endl
         << "  -m, --mode {duplicate,d,freegame,f,training,t}" << endl
         << "                           Choose game mode (mandatory)" << endl
         << "  -d, --dict <path>        Choose a dictionary (mandatory)" << endl
         << "  -u  --human <name>       Add a human player" << endl
         << "  -a  --ai <name>          Add a AI (Artificial Intelligence) player" << endl
         << "      --joker              Play with the \"Joker game\" variant" << endl;
}


void GameFactory::printVersion() const
{
    cout << PACKAGE_STRING << endl
         << "This program comes with NO WARRANTY, to the extent permitted by "
         << "law." << endl << "You may redistribute it under the terms of the "
         << "GNU General Public License;" << endl
         << "see the file named COPYING for details." << endl;
}

