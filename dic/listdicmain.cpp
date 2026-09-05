/*****************************************************************************
 * Eliot
 * Copyright (C) 1999-2007 Antoine Fraboulet & Olivier Teulière
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

#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <print>

#include <getopt.h>

#include "config.h"

#ifdef WIN32
#   include <windows.h>
#endif

#include "header.h"
#include "encoding.h"
#include "dic_internals.h"
#include "listdic.h"
#include "dic.h"

using namespace std;


static void printHeader(const Dictionary &iDic)
{
    iDic.getHeader().print(cout);
}


static void printLetters(const Dictionary &iDic)
{
    const Header &header = iDic.getHeader();
    const wstring &letters = header.getLetters();
    for (unsigned i = 1; i <= letters.size(); ++i)
    {
        // Main data
        wstring wlett(1, letters[i - 1]);
        cout << ufw(wlett) << " "
             << (int) header.getPoints(i) << " "
             << (int) header.getFrequency(i) << " "
             << (header.isVowel(i) ? "1" : "0") << " "
             << (header.isConsonant(i) ? "1" : "0");

        // Display and input strings
        auto it = header.getDisplayInputData().find(letters[i - 1]);
        if (it != header.getDisplayInputData().end())
        {
            for (const wstring &input : it->second)
            {
                cout << " " << ufw(input);
            }
        }

        cout << endl;
    }
}


static void printHexa(const Dictionary &iDic)
{
    union edge_t
    {
        DicEdge e;
        uint32_t s;
    } ee;

    printf(_("offset binary   | structure\n"));
    std::println("------ -------- | --------------------");
    for (unsigned int i = 0; i < (iDic.getHeader().getNbEdgesUsed() + 1); i++)
    {
        ee.e = *reinterpret_cast<const DicEdge*>(iDic.getEdgeAt(i));

        std::println("0x{:04x} {:08x} |{:4} ptr={:8} t={} l={} chr={:2} ({:c})",
               i*sizeof(ee), ee.s,
               i, +ee.e.ptr, +ee.e.term, +ee.e.last, +ee.e.chr,
               (wint_t)(ee.e.chr == 0 ? L'-' : iDic.getHeader().getCharFromCode(ee.e.chr)));
    }
}


static void printUsage(const string &iBinaryName)
{
    cout << "Usage: " << iBinaryName << " [-e|-l|-w|-x]] -d <dawg_file>" << endl
         << _("Mandatory options:") << endl
         << _("  -d, --dictionary <string>  Dictionary file (.dawg) to use") << endl
         << _("Output options:") << endl
         << _("  -e, --header            Print the dictionary header") << endl
         << _("  -l, --letters           Print letters information, in a format") << endl
         << _("                          suitable for the 'compdic' program") << endl
         << _("  -w, --words             Print all the words stored in the dictionary") << endl
         << _("  -x, --hexa              Print data as hexadecimal (for debugging)") << endl
         << _("Other options:") << endl
         << _("  -h, --help              Print this help and exit") << endl
         << endl
         << _("If no output option is specified, --header is used implicitly.") << endl
         << _("Example: ") << iBinaryName << " -w -d ods.dawg" << endl;
}


int main(int argc, char *argv[])
{
    // Set locale via LC_ALL
    setlocale(LC_ALL, "");

#if ENABLE_NLS
    // Set the message domain
#ifdef WIN32
    const std::filesystem::path localeDir =
        std::filesystem::absolute(argv[0]).parent_path() / "locale";
#else
    static const std::filesystem::path localeDir = LOCALEDIR;
#endif
    bindtextdomain(PACKAGE, localeDir.string().c_str());
    textdomain(PACKAGE);
#endif

    static const std::array<struct option, 8> long_options = {{
        {.name="help", .has_arg=no_argument, .flag=nullptr, .val='h'},
        {.name="dictionary", .has_arg=required_argument, .flag=nullptr, .val='d'},
        {.name="header", .has_arg=no_argument, .flag=nullptr, .val='e'},
        {.name="letters", .has_arg=no_argument, .flag=nullptr, .val='l'},
        {.name="raw-words", .has_arg=no_argument, .flag=nullptr, .val='r'},
        {.name="words", .has_arg=no_argument, .flag=nullptr, .val='w'},
        {.name="hexa", .has_arg=no_argument, .flag=nullptr, .val='x'},
        {.name=nullptr, .has_arg=0, .flag=nullptr, .val=0}
    }};
    static const auto short_options = std::to_array("hd:elrwx");

    bool dicSpecified = false;
    bool shouldPrintHeader = false;
    bool shouldPrintLetters = false;
    bool shouldPrintInternalWords = false;
    bool shouldPrintDisplayWords = false;
    bool shouldPrintHexa = false;
    string dicPath;

    int res;
    int option_index = 1;
    while ((res = getopt_long(argc, argv, short_options.data(),
                              long_options.data(), &option_index)) != -1)
    {
        switch (res)
        {
            case 'h':
                printUsage(argv[0]);
                exit(0);
            case 'd':
                dicSpecified = true;
                dicPath = optarg;
                break;
            case 'e':
                shouldPrintHeader = true;
                break;
            case 'l':
                shouldPrintLetters = true;
                break;
            case 'r':
                shouldPrintInternalWords = true;
                break;
            case 'w':
                shouldPrintDisplayWords = true;
                break;
            case 'x':
                shouldPrintHexa = true;
                break;
        }
    }

    // Check mandatory options
    if (!dicSpecified)
    {
        cerr << _("A mandatory option is missing") << endl;
        printUsage(argv[0]);
        exit(1);
    }

    // The default is to print the header
    if (!shouldPrintHeader && !shouldPrintLetters &&
        !shouldPrintInternalWords && !shouldPrintDisplayWords && !shouldPrintHexa)
    {
        shouldPrintHeader = true;
    }

    try
    {
        // Load the dictionary
        Dictionary dic(dicPath);

        if (shouldPrintHeader)
            printHeader(dic);
        if (shouldPrintLetters)
            printLetters(dic);
        if (shouldPrintInternalWords)
            ListDic::printWords(cout, dic, false);
        if (shouldPrintDisplayWords)
            ListDic::printWords(cout, dic, true);
        if (shouldPrintHexa)
            printHexa(dic);

        return 0;
    }
    catch (std::exception &e)
    {
        cerr << e.what() << endl;
        return 1;
    }
}

