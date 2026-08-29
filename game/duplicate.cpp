/*****************************************************************************
 * Eliot
 * Copyright (C) 2005-2012 Olivier Teulière
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

#include "duplicate.h"
#include "game_exception.h"
#include <game_params.h>
#include "rack.h"
#include "move.h"
#include "pldrack.h"
#include "results.h"
#include "player.h"
#include "cmd/player_move_cmd.h"
#include "cmd/player_rack_cmd.h"
#include "cmd/player_event_cmd.h"
#include "cmd/game_move_cmd.h"
#include "cmd/master_move_cmd.h"
#include "ai_player.h"
#include "navigation.h"
#include "turn.h"
#include "turn_data.h"
#include "settings.h"
#include "encoding.h"
#include "debug.h"
#include <memory>
#include <utility>


INIT_LOGGER(game, Duplicate);


Duplicate::Duplicate(const GameParams &iParams, std::unique_ptr<const Game> iMasterGame)
    : Game(iParams, std::move(iMasterGame))
{
}


int Duplicate::play(const wstring &iCoord, const wstring &iWord)
{
    ASSERT(!isFinished(), "The game is already finished");

    Player &currPlayer = *m_players[m_currPlayer];

    ASSERT(currPlayer.isHuman(), "Human player expected");
    ASSERT(!hasPlayed(m_currPlayer), "Human player has already played");

    // Perform all the validity checks, and try to fill a round
    Move move;
    int res = checkPlayedWord(iCoord, iWord, move);
    if (res != 0 && Settings::Instance().getBool("duplicate.reject-invalid"))
    {
        return res;
    }

    // If we reach this point, either the move is valid and we can use the
    // "move" variable, or it is invalid but played nevertheless
    recordPlayerMove(currPlayer, move);

    // Little hack to handle duplicate games with only AI players.
    // This will have no effect when there is at least one human player
    tryEndTurn();

    return 0;
}


void Duplicate::playAI(unsigned int p)
{
    ASSERT(p < getNPlayers(), "Wrong player number");
    ASSERT(!hasPlayed(p), "AI player has already played");

    auto *player = dynamic_cast<AIPlayer*>(m_players[p].get());
    ASSERT(player != nullptr, "AI requested for a human player");

    player->compute(getDic(), getBoard(), getHistory().beforeFirstRound());
    const Move &move = player->getMove();
    if (move.isChangeLetters() || move.isPass())
    {
        // The AI player must be buggy...
        ASSERT(false, "AI tried to cheat!");
    }

    recordPlayerMove(*player, move);
}


void Duplicate::start()
{
    ASSERT(getNPlayers(), "Cannot start a game without any player");

    // Arbitrary player, since they should all have the same rack
    m_currPlayer = 0;

    // Complete the racks
    try
    {
        if (hasMasterGame())
        {
            // When a master game is defined, it forces the master move
            setMasterMove(getMoveFromMasterGame());
        }
        else
        {
            // Reset the master move
            setMasterMove(Move());
        }

        bool fillRacks = Settings::Instance().getBool("arbitration.fill-rack");
        if (isArbitrationGame() && !fillRacks && !hasMasterGame())
            setGameAndPlayersRack(getHistory().getCurrentRack(), true);
        else
        {
            const PlayedRack &newRack =
                helperSetRackRandom(getHistory().getCurrentRack(), true, RACK_NEW);
            setGameAndPlayersRack(newRack, true);
        }
    }
    catch (EndGameException &e)
    {
        endGame();
        return;
    }

    if (!isArbitrationGame())
    {
        // Little hack to handle duplicate games with only AI players.
        // This will have no effect when there is at least one human player
        tryEndTurn();
    }
}


bool Duplicate::isFinished() const
{
    return !canDrawRack(m_players[0]->getHistory().getCurrentRack(), true);
}


void Duplicate::tryEndTurn()
{
    if (!isArbitrationGame())
    {
        for (unsigned int i = 0; i < getNPlayers(); i++)
        {
            if (m_players[i]->isHuman() && !hasPlayed(i))
            {
                // A human player has not played...
                m_currPlayer = i;
                // So we don't finish the turn
                return;
            }
        }
    }

    // Now that all the human players have played,
    // make AI players play their turn
    // Some may have already played, in arbitration mode, if the future turns
    // were removed (because of the isHumanIndependent() behaviour)
    for (unsigned int i = 0; i < getNPlayers(); i++)
    {
        if (!m_players[i]->isHuman() && !hasPlayed(i))
        {
            playAI(i);
        }
    }

    // Next turn
    endTurn();
}


struct MatchingPlayer
{
    MatchingPlayer(unsigned iPlayerId) : m_playerId(iPlayerId) {}

    bool operator()(const PlayerMoveCmd &cmd)
    {
        return cmd.getPlayer().getId() == m_playerId;
    }

    const unsigned m_playerId;
};


void Duplicate::recordPlayerMove(Player &ioPlayer, const Move &iMove)
{
    LOG_INFO("Player {} plays: ", ioPlayer.getId(), lfw(iMove.toString()));

    // Search a PlayerMoveCmd for the given player
    MatchingPlayer predicate(ioPlayer.getId());
    const auto *cmd =
        getNavigation().getCurrentTurn().findMatchingCmd<PlayerMoveCmd>(predicate);
    if (cmd == nullptr)
    {
        accessNavigation().addAndExecute(
            std::make_unique<PlayerMoveCmd>(ioPlayer, iMove, isArbitrationGame())
        );
    }
    else
    {
        // Replace the player move
        LOG_DEBUG("Replacing move for player {}", ioPlayer.getId());
        if (!isArbitrationGame() && !getNavigation().isLastTurn())
            throw GameException("Cannot add a command to an old turn");
        accessNavigation().replaceCommand(*cmd,
            std::make_unique<PlayerMoveCmd>(ioPlayer, iMove, isArbitrationGame())
        );
    }
}


Player * Duplicate::findBestPlayer() const
{
    Player *bestPlayer = nullptr;
    int bestScore = -1;
    for (const auto &player : m_players)
    {
        const Move &move = player->getLastMove();
        if (move.isValid() && move.getScore() > bestScore)
        {
            bestScore = move.getScore();
            bestPlayer = player.get();
        }
    }
    return bestPlayer;
}


void Duplicate::endTurn()
{
    static const unsigned int REF_PLAYER_ID = 0;

    // If there is a master game, the master move should still have its
    // original value (set in the start() method)
    ASSERT(!hasMasterGame() || m_masterMove == getMoveFromMasterGame(),
           "The master move has been modified");

    // Define the master move if it is not already defined
    if (!m_masterMove.isValid())
    {
        // The chosen implementation is to find the best move among the players' moves.
        // It is more user-friendly than forcing the best move when nobody found it.
        // If you want the best move instead, simply add an AI player to the game!

        // Find the player with the best score
        const Player *bestPlayer = findBestPlayer();
        if (bestPlayer != nullptr)
        {
            setMasterMove(bestPlayer->getLastMove());
        }
        else
        {
            // If nobody played a valid round, we are forced to play a valid move.
            // So let's take the best one...
            MasterResults results;
            // Take the first player's rack
            const Rack &rack =
                m_players[REF_PLAYER_ID]->getLastRack().getRack();
            results.search(getDic(), getBoard(), rack, getHistory().beforeFirstRound());
            if (results.isEmpty())
            {
                // This would be very bad luck that no move is possible...
                // It's probably not even possible, but let's be safe.
                throw EndGameException(_("No possible move"));
            }

            setMasterMove(Move(results.get(0)));
        }
    }

    // Handle solo bonus
    if (!isArbitrationGame())
    {
        unsigned minNbPlayers = Settings::Instance().getInt("duplicate.solo-players");
        int soloValue = Settings::Instance().getInt("duplicate.solo-value");
        setSoloAuto(minNbPlayers, soloValue);
    }
    else
    {
        bool useSoloAuto = Settings::Instance().getBool("arbitration.solo-auto");
        if (useSoloAuto)
        {
            unsigned minNbPlayers = Settings::Instance().getInt("arbitration.solo-players");
            int soloValue = Settings::Instance().getInt("arbitration.solo-value");
            setSoloAuto(minNbPlayers, soloValue);
        }
    }

    // Play the master word on the board
    // We assign it to player 0 arbitrarily (this is only used
    // to retrieve the rack, which is the same for all players...)
    accessNavigation().addAndExecute(
        std::make_unique<GameMoveCmd>(*this, m_masterMove)
    );

    // Change the turn after doing all the game changes.
    // The navigation system expects auto-executable commands (like setting
    // the players racks) at the beginning of the turn, to work properly.
    accessNavigation().newTurn();

    // Leave the same reliquate to all players
    // This is required by the start() method which will be called to
    // start the next turn
    const PlayedRack& pld = getHistory().getCurrentRack();
    for (auto &player : m_players)
    {
        accessNavigation().addAndExecute(
            std::make_unique<PlayerRackCmd>(*player, pld)
        );
    }

    // Start next turn...
    start();
}


void Duplicate::endGame()
{
    LOG_INFO("End of the game");
    // No more master move
    setMasterMove(Move());
}


void Duplicate::setPlayer(unsigned int p)
{
    ASSERT(p < getNPlayers(), "Wrong player number");

    // Forbid switching to an AI player
    if (!m_players[p]->isHuman())
        throw GameException(_("Cannot switch to a non-human player"));

    // Forbid switching back to a player who has already played
    if (hasPlayed(p))
        throw GameException(_("Cannot switch to a player who has already played"));

    m_currPlayer = p;
}


bool Duplicate::hasPlayed(unsigned iPlayerId) const
{
    ASSERT(iPlayerId < getNPlayers(), "Wrong player number");

    // Search a PlayerMoveCmd for the given player
    MatchingPlayer predicate(iPlayerId);
    const auto *cmd =
        getNavigation().getCurrentTurn().findMatchingCmd<PlayerMoveCmd>(predicate);
    return cmd != nullptr && cmd->isExecuted() && !cmd->getMove().isNull();
}


void Duplicate::innerSetMasterMove(const Move &iMove)
{
    m_masterMove = iMove;
}


void Duplicate::setMasterMove(const Move &iMove)
{
    ASSERT(iMove.isValid() || iMove.isNull(), "Invalid move type");

    // If this method is called several times for the same turn, it will
    // result in many MasterMoveCmd commands in the command stack.
    // This shouldn't be a problem though.
    LOG_DEBUG("Setting master move: " + lfw(iMove.toString()));
    accessNavigation().addAndExecute(
        std::make_unique<MasterMoveCmd>(*this, iMove)
    );
}


bool Duplicate::isArbitrationGame() const
{
    return getParams().getMode() == GameParams::kARBITRATION;
}


void Duplicate::setSoloAuto(unsigned int minNbPlayers, int iSoloValue)
{
    // Remove all existing solos
    for (const auto &player : m_players)
    {
        const PlayerEventCmd *cmd = getPlayerEvent(player->getId(), PlayerEventCmd::SOLO);
        if (cmd != nullptr)
        {
            accessNavigation().dropCommand(*cmd);
        }
    }

    // Check whether there are enough players in the game for the
    // solo to apply. We count only the "active" players, i.e. the ones
    // which have played at least one word during the game, even if they
    // have left the game since then, or have arrived after the beginning.
    unsigned countActive = 0;
    for (const auto &player : m_players)
    {
        for (unsigned i = 0; i < player->getHistory().getSize(); ++i)
        {
            if (!player->getHistory().getTurn(i).getMove().isNull())
            {
                ++countActive;
                break;
            }
        }
    }
    if (countActive < minNbPlayers)
        return;

    // Find the player with the best score
    Player *bestPlayer = findBestPlayer();
    if (bestPlayer != nullptr)
    {
        int bestScore = bestPlayer->getLastMove().getScore();

        // Find whether other players than imax have the same score
        bool otherWithSameScore = false;
        for (const auto &player : m_players)
        {
            if (player.get() != bestPlayer &&
                player->getLastMove().getScore() >= bestScore &&
                player->getLastMove().isValid())
            {
                otherWithSameScore = true;
                break;
            }
        }

        if (!otherWithSameScore)
        {
            // Give the bonus to the player of the best move
            LOG_INFO("Giving a solo of {} to player {}", iSoloValue, bestPlayer->getId());
            accessNavigation().insertCommand(
                std::make_unique<PlayerEventCmd>(*bestPlayer, PlayerEventCmd::SOLO, iSoloValue)
            );
        }
    }
}


/// Predicate to help retrieving commands
struct MatchingPlayerAndEventType
{
    MatchingPlayerAndEventType(unsigned iPlayerId, int iEventType)
        : m_playerId(iPlayerId), m_eventType(iEventType) {}

    bool operator()(const PlayerEventCmd &cmd)
    {
        return cmd.getPlayer().getId() == m_playerId
            && cmd.getEventType() == m_eventType;
    }

    const unsigned m_playerId;
    const int m_eventType;
};


const PlayerEventCmd * Duplicate::getPlayerEvent(unsigned iPlayerId,
                                                 int iEventType) const
{
    ASSERT(iPlayerId < getNPlayers(), "Wrong player number");
    MatchingPlayerAndEventType predicate(iPlayerId, iEventType);
    const Turn &currTurn = getNavigation().getCurrentTurn();
    return currTurn.findMatchingCmd<PlayerEventCmd>(predicate);
}


