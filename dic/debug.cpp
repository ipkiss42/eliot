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

#include <boost/stacktrace.hpp>
#include <format>
#include <iostream>

using std::cerr;
using std::endl;


#include "debug.h"


std::string StackTrace::GetStack() {
    // Skip the first frame, to avoid seeing this function
    auto toSkip = 1;
    auto trace = boost::stacktrace::stacktrace(toSkip, static_cast<std::size_t>(-1));
    return boost::stacktrace::to_string(trace);
}


void eliotAssert(std::string_view msg) {
    auto errorMsg = std::format(
        "ASSERTION FAILED: {} (at {}#{})", msg, __FILE__, __LINE__
    );

    cerr << errorMsg << endl;
    auto stack = StackTrace::GetStack();
    cerr << stack << endl;

    abort();
}
