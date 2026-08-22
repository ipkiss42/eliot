/*****************************************************************************
 * Eliot
 * Copyright (C) 1999-2012 Antoine Fraboulet & Olivier Teulière
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

#include <iostream>
#include <string>

#include "encoding.h"
#include "header.h"
#include "dic_internals.h"
#include "dic.h"
#include "listdic.h"

using namespace std;


INIT_LOGGER(dic, ListDic);


static void printDicRec(ostream &out, const Dictionary &iDic, wstring &current_word, DicEdge edge)
{
    if (edge.term)
    {
        // Edge points at a complete word
        out << lfw(current_word) << endl;
    }

    if (edge.ptr)
    {
        // Compute index: is it non-zero?
        const DicEdge *p = iDic.getEdgeAt(edge.ptr);
        do
        {
            // Append the character for this branch
            current_word.push_back(iDic.getHeader().getCharFromCode(p->chr));

            // Recurse deeper into the tree
            printDicRec(out, iDic, current_word, *p);

            // Backtrack
            current_word.pop_back();
        }
        while (!(*p++).last);
    }
}


void ListDic::printWords(ostream &out, const Dictionary &iDic)
{
    wstring word;
    printDicRec(out, iDic, word, *iDic.getEdgeAt(iDic.getRoot()));
}

