/*****************************************************************************
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

#ifndef RANDOM_H_
#define RANDOM_H_

#include <random>

class Random {
public:
    // Force the singleton pattern
    Random() = delete;

    static std::mt19937& getEngine() {
        return s_engine;
    }

    static void setSeed(unsigned int iSeed) {
        s_currentSeed = iSeed;
        s_engine.seed(iSeed);
    }

    static unsigned int getUsedSeed() {
        return s_currentSeed;
    }

private:
    // Seed used to initialize the engine
    inline static unsigned int s_currentSeed = 0;

    // Initialize the engine and capture the seed simultaneously
    static std::mt19937 initEngine() {
        s_currentSeed = std::random_device{}();
        return std::mt19937(s_currentSeed);
    }

    // The shared engine, initialized via the helper function above
    inline static std::mt19937 s_engine = initEngine();

};

#endif
