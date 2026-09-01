/*****************************************************************************
 * Eliot
 * Copyright (C) 1999-2013 Antoine Fraboulet & Olivier Teulière
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

#include <algorithm>
#include <bit>
#include <cstdint>
#include <ctime>
#include <cwctype>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>

#include "compdic.h"
#include "encoding.h"
#include "dic_exception.h"

#define MAX_STRING_LENGTH 200

INIT_LOGGER(dic, CompDic);


std::size_t DicEdgeVectorHash::operator()(const vector<DicEdge>& edges) const {
    std::size_t seed = edges.size();
    for (const auto& edge : edges) {
        auto rawBits = std::bit_cast<uint32_t>(edge);
        std::size_t edgeHash = std::hash<uint32_t>()(rawBits);

        // Magic hashing constants
        seed ^= edgeHash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}


CompDic::CompDic()

{
    m_headerInfo.root       = 0;
    m_headerInfo.nwords     = 0;
    m_headerInfo.nodesused  = 1;
    m_headerInfo.edgesused  = 1;
    m_headerInfo.nodessaved = 0;
    m_headerInfo.edgessaved = 0;

    m_stringBuf = new wchar_t[MAX_STRING_LENGTH];
    m_endString = m_stringBuf;
}


CompDic::~CompDic()
{
    delete[] m_stringBuf;
}


void CompDic::addLetter(wchar_t chr, int points, int frequency,
                        bool isVowel, bool isConsonant,
                        const vector<wstring> &iInputs)
{
    // We don't support non-alphabetical characters in the dictionary
    // apart from the joker '?'. For more explanations on the issue, see
    // on the eliot-dev mailing-list the thread with the following title:
    //   re: Unable to show menus in Catalan, and some weird char "problem"
    // (started on 2009/12/31)
    if (!iswalpha(chr) && chr != L'?')
    {
        throw DicException(std::format(
            "{}\n{}",
            _fmt(_("'{0}' is not a valid letter."), lfw(chr)),
            _fmt(_("For technical reasons, Eliot currently only supports "
                   "alphabetical characters as internal character "
                   "representation, even if the tile has a display string "
                   "defined. Please use another character and change your "
                   "word list accordingly."))
        ));
    }

    const wchar_t upChar = towupper(chr);
    m_headerInfo.letters += upChar;
    m_headerInfo.points.push_back(points);
    m_headerInfo.frequency.push_back(frequency);
    m_headerInfo.vowels.push_back(isVowel);
    m_headerInfo.consonants.push_back(isConsonant);

    // Ensure the input strings are in upper case
    if (!iInputs.empty())
    {
        vector<wstring> upperInputs = iInputs;
        for (wstring &str : upperInputs)
        {
            std::ranges::transform(str, str.begin(), towupper);
        }

        // If the display string is identical to the internal char and if
        // there is no other input, no need to save this information, as
        // it is already the default.
        if (upperInputs.size() != 1 || upperInputs[0] != wstring(1, upChar))
        {
            m_headerInfo.displayInputData[upChar] = upperInputs;
        }
    }
}


void CompDic::loadWordList(const std::filesystem::path &iFileName, vector<wstring> &oWordList)
{
    ifstream file(iFileName, ios::in | ios::binary);
    if (!file.is_open())
        throw DicException(_fmt(_("Could not open file '{0}'"), iFileName.string()));

    // Get the file size
    std::error_code ec;
    auto size = std::filesystem::file_size(iFileName, ec);
    if (ec)
        throw DicException(_fmt(_("Could not open file '{0}'"), iFileName.string()));
    int dicSize = static_cast<int>(size);

    // Reserve some space (heuristic: the average length of words is 11)
    oWordList.reserve(dicSize / 11);

    string line;
    while (getline(file, line))
    {
        // If there is a BOM in the file, remove it from the first word
        if (oWordList.empty() && line.size() >= 3 &&
            (uint8_t)line[0] == 0xEF &&
            (uint8_t)line[1] == 0xBB &&
            (uint8_t)line[2] == 0xBF)
        {
            line = line.substr(3);
        }
        // Remove potential \r
        if (line[line.size() - 1] == '\r')
            line = line.substr(0, line.size() - 1);
        // Ignore empty lines
        if (line == "")
            continue;
        // Ensure the word is in upper case
        wstring wstr = readFromUTF8(line, "loadWordList");
        std::ranges::transform(wstr, wstr.begin(), towupper);

        oWordList.push_back(wstr);
    }

    // Sort the word list, to perform a better compression
    std::ranges::sort(oWordList);
}


Header CompDic::writeHeader(ostream &outFile) const
{
    // Go back to the beginning of the stream before writing the header
    outFile.seekp(0, ios::beg);
    Header aHeader(m_headerInfo);
    aHeader.write(outFile);
    return aHeader;
}


void CompDic::writeNode(DicEdge *ioEdges, unsigned int num, ostream &outFile)
{
    auto *edgesAsUint = reinterpret_cast<uint32_t*>(ioEdges);
    // Handle endianness
    for (unsigned int i = 0; i < num; ++i)
    {
        edgesAsUint[i] = hton(edgesAsUint[i]);
    }

    LOG_TRACE("writing {} edges", num);
    outFile.write((char*)edgesAsUint, num * sizeof(DicEdge));
}

#define MAX_EDGES 2000
/* ods3: ??   */
/* ods4: 1746 */


#ifdef CHECK_RECURSION
class IncDec
{
    public:
        IncDec(int &ioCounter) : m_counter(ioCounter) { ++m_counter; }
        ~IncDec() { --m_counter; }
    private:
        int &m_counter;
};
#endif


unsigned int CompDic::makeNode(ostream &outFile, const Header &iHeader,
                               vector<wstring>::const_iterator &itCurrWord,
                               const vector<wstring>::const_iterator &itLastWord,
                               wstring::const_iterator &itPosInWord,
                               const wchar_t *iPrefix)
{
#ifdef CHECK_RECURSION
    IncDec inc(m_currentRec);
    if (m_currentRec > m_maxRec)
        m_maxRec = m_currentRec;

    // Instead of creating a vector, try to reuse an existing one
    vector<DicEdge> &edges = m_mapForDepth[m_currentRec];
    edges.reserve(MAX_EDGES);
    edges.clear();
#else
    vector<DicEdge> edges;
    // Optimize allocation
    edges.reserve(MAX_EDGES);
#endif
    DicEdge newEdge;

    while (iPrefix == m_endString)
    {
        // More edges out of node
        newEdge.ptr  = 0;
        newEdge.term = 0;
        newEdge.last = 0;
        try
        {
            newEdge.chr = iHeader.getCodeFromChar(*m_endString = *itPosInWord);
            ++m_endString;
            ++itPosInWord;
        }
        catch (DicException &e)
        {
            // If an invalid character is found, be specific about the problem
            throw DicException(_fmt(
                _("Error in the word list on line {0}, col {1}: {2}"),
                1 + m_headerInfo.nwords,
                1 + m_endString - m_stringBuf,
                e.what()
            ));
        }
        edges.push_back(newEdge);

        // End of a word?
        if (itPosInWord == itCurrWord->end())
        {
            m_headerInfo.nwords++;
            *m_endString = L'\0';
            // Mark edge as word
            edges.back().term = 1;

            // Next word
            ++itCurrWord;
            // At the end of input?
            if (itCurrWord == itLastWord)
                break;
            itPosInWord = itCurrWord->begin();

            m_endString = m_stringBuf;
            // This assumes that a word cannot be a prefix of the previous one
            while (*m_endString == *itPosInWord)
            {
                ++m_endString;
                ++itPosInWord;
            }
        }
        // Make dawg pointed to by this edge
        edges.back().ptr = makeNode(outFile, iHeader, itCurrWord, itLastWord,
                                    itPosInWord, iPrefix + 1);
    }

    int numedges = edges.size();
    if (numedges == 0)
    {
        // Special node zero - no edges
        return 0;
    }

    // Mark the last edge
    edges.back().last = 1;

    auto itMap = m_hashMap.find(edges);
    if (itMap != m_hashMap.end())
    {
        m_headerInfo.edgessaved += numedges;
        m_headerInfo.nodessaved++;

        return itMap->second;
    }
    else
    {
        unsigned int node_pos = m_headerInfo.edgesused;
        m_hashMap[edges] = m_headerInfo.edgesused;
        m_headerInfo.edgesused += numedges;
        m_headerInfo.nodesused++;
        writeNode(&edges.front(), numedges, outFile);

        return node_pos;
    }
}


Header CompDic::generateDawg(const std::filesystem::path &iWordListFile,
                             const std::filesystem::path &iDawgFile,
                             const string &iDicName)
{
    m_headerInfo.dicName = wfl(iDicName);
    // We are not (yet) able to build the GADDAG format
    m_headerInfo.dawg = true;

    // Open the output file
    ofstream outFile(iDawgFile, ios::out | ios::binary | ios::trunc);
    if (!outFile.is_open())
    {
        throw DicException(_fmt(_("Cannot open output file '{0}'"), iDawgFile.string()));
    }

    const clock_t startLoadTime = clock();
    vector<wstring> wordList;
    loadWordList(iWordListFile, wordList);
    const clock_t endLoadTime = clock();
    m_loadTime = 1.0 * (endLoadTime - startLoadTime) / CLOCKS_PER_SEC;

    if (wordList.empty())
    {
        throw DicException(_("The word list is empty!"));
    }

    // Write the header a first time, to reserve the space in the file
    Header tempHeader = writeHeader(outFile);

    DicEdge specialNode = {.ptr=0, .term=0, .last=0, .chr=0};
    specialNode.last = 1;
    // Temporary variable to avoid a warning when compiling with -O2
    // (there is no warning with -O0... g++ bug?)
    writeNode(&specialNode, 1, outFile);

    auto firstWord = wordList.cbegin();
    wstring::const_iterator initialPos = firstWord->begin();

    // Call makeNode with null (relative to stringbuf) prefix;
    // Initialize string to null; Put index of start node on output
    DicEdge rootNode = {.ptr=0, .term=0, .last=0, .chr=0};
    m_endString = m_stringBuf;
    const clock_t startBuildTime = clock();
    rootNode.ptr = makeNode(outFile, tempHeader,
                            firstWord, wordList.end(),
                            initialPos, m_endString);
    // Reuse the temporary variable
    writeNode(&rootNode, 1, outFile);
    const clock_t endBuildTime = clock();
    m_buildTime = 1.0 * (endBuildTime - startBuildTime) / CLOCKS_PER_SEC;

    // Write the header again, now that it is complete
    m_headerInfo.root = m_headerInfo.edgesused;
    const Header finalHeader = writeHeader(outFile);

    // Clean up
    outFile.close();

    return finalHeader;
}

