/*****************************************************************************
 * Eliot
 * Copyright (C) 2008 Olivier Teulière
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

#include <iterator>
#include <string_view>
#include <stack>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_chset.hpp>
#include <boost/spirit/include/classic_ast.hpp>
#ifdef DEBUG_RE
#include <boost/spirit/include/classic_tree_to_xml.hpp>
#include <map>
#include <iostream>
#endif

#include "dic.h"
#include "header.h"
#include "encoding.h"
#include "regexp.h"

using namespace boost::spirit::classic;
using namespace std;

// A few typedefs to simplify things
using iterator_t = const wchar_t *;
using parse_tree_match_t = tree_match<iterator_t>;
using iter_t = parse_tree_match_t::const_tree_iterator;
using tree_node_t = parse_tree_match_t::node_t;


struct RegexpGrammar : grammar<RegexpGrammar>
{
    static const int wrapperId = 0;
    static const int exprId = 1;
    static const int repeatId = 2;
    static const int groupId = 3;
    static const int varId = 4;
    static const int choiceId = 5;
    static const int alphavarId = 6;

    RegexpGrammar(const wstring &letters)
    {
        m_allLetters = letters + toLower(letters);
    }

    template <typename ScannerT>
    struct definition
    {
        // Constructor
        definition(const RegexpGrammar &self)
        {
            wrapper
                = expr >> L"#"
                ;

            expr
                = repeat >> *expr;
                ;

            repeat
                = group >> root_node_d[ch_p(L'?')]
                | group >> root_node_d[ch_p(L'*')]
                | group >> root_node_d[ch_p(L'+')]
                | group
                ;

            group
                = var
                | root_node_d[str_p(L"[^")] >> choice >> no_node_d[ch_p(L']')]
                | root_node_d[ch_p(L'[')] >> choice >> no_node_d[ch_p(L']')]
                | root_node_d[ch_p(L'(')] >> +repeat >> no_node_d[ch_p(L')')] // XXX: 'expr' instead of '+repeat' doesn't work. Why?
                ;

            var
                = alphavar
                | ch_p(L'.')
                | str_p(L":v:")
                | str_p(L":c:")
                | str_p(L":1:")
                | str_p(L":2:")
                ;

            choice
                = leaf_node_d[+alphavar]
                ;

            alphavar
                = chset<wchar_t>(self.m_allLetters.c_str())
                ;
        }

        rule<ScannerT, parser_context<>, parser_tag<wrapperId> > wrapper;
        rule<ScannerT, parser_context<>, parser_tag<exprId> > expr;
        rule<ScannerT, parser_context<>, parser_tag<repeatId> > repeat;
        rule<ScannerT, parser_context<>, parser_tag<groupId> > group;
        rule<ScannerT, parser_context<>, parser_tag<varId> > var;
        rule<ScannerT, parser_context<>, parser_tag<choiceId> > choice;
        rule<ScannerT, parser_context<>, parser_tag<alphavarId> > alphavar;

        const rule<ScannerT, parser_context<>, parser_tag<wrapperId> > & start() const { return wrapper; }
    };

    wstring m_allLetters;
};


void evaluate(const Header &iHeader, const tree_node_t &i, stack<Node*> &evalStack,
              searchRegExpLists &iList, bool negate = false)
{
    std::wstring_view valueView(&*i.value.begin(), std::distance(i.value.begin(), i.value.end()));

    if (i.value.id() == RegexpGrammar::alphavarId)
    {
        assert(i.children.size() == 0);

        // Extract the character and convert it to its internal code
        uint8_t code = iHeader.getCodeFromChar(valueView.front());
        evalStack.push(new Node(NODE_VAR, code, nullptr, nullptr));
    }
    else if (i.value.id() == RegexpGrammar::choiceId)
    {
        assert(i.children.size() == 0);

        wstring choiceLetters(valueView);
        // Make sure the letters are in upper case
        choiceLetters = toUpper(choiceLetters);
        // The dictionary letters are already in upper case
        const wstring &letters = iHeader.getLetters();
        // j is the index of the new list we create
        size_t j = iList.symbl.size();
        iList.symbl.push_back(RE_ALL_MATCH + j);
        iList.letters.emplace_back(DIC_LETTERS + 1, false);
        for (wchar_t letter : letters)
        {
            bool contains = choiceLetters.contains(letter);
            iList.letters[j][iHeader.getCodeFromChar(letter)] =
                (contains ? !negate : negate);
        }
        evalStack.push(new Node(NODE_VAR, iList.symbl[j], nullptr, nullptr));
    }
    else if (i.value.id() == RegexpGrammar::varId)
    {
        assert(i.children.size() == 0);

        uint8_t matchType = RE_ALL_MATCH;
        if (valueView == L":v:")
            matchType = RE_VOWL_MATCH;
        else if (valueView == L":c:")
            matchType = RE_CONS_MATCH;
        else if (valueView == L":1:")
            matchType = RE_USR1_MATCH;
        else if (valueView == L":2:")
            matchType = RE_USR2_MATCH;
        else if (valueView == L".")
            matchType = RE_ALL_MATCH;
        else
            assert(0);

        evalStack.push(new Node(NODE_VAR, matchType, nullptr, nullptr));
    }
    else if (i.value.id() == RegexpGrammar::groupId)
    {
        if (valueView.starts_with(L'('))
        {
            assert(i.children.size() != 0);
            // Create a node for each child
            for (const auto &child : i.children)
                evaluate(iHeader, child, evalStack, iList);
            // "Concatenate" the created child nodes with AND nodes
            for (unsigned int j = 0; j < i.children.size() - 1; ++j)
            {
                Node *old2 = evalStack.top();
                evalStack.pop();
                Node *old1 = evalStack.top();
                evalStack.pop();
                evalStack.push(new Node(NODE_AND, '\0', old1, old2));
            }
        }
        else if (valueView.starts_with(L'['))
        {
            assert(i.children.size() == 1);
            bool hasCaret = valueView.size() > 1;
            evaluate(iHeader, i.children.front(), evalStack, iList, hasCaret);
        }
        else
            assert(0);
    }
    else if (i.value.id() == RegexpGrammar::repeatId)
    {
        assert(i.children.size() == 1);
        evaluate(iHeader, i.children.front(), evalStack, iList);

        Node *old = evalStack.top();
        evalStack.pop();

        if (*i.value.begin() == L'*')
        {
            evalStack.push(new Node(NODE_STAR, '\0', old, nullptr));
        }
        else if (*i.value.begin() == L'+')
        {
            evalStack.push(new Node(NODE_PLUS, '\0', old, nullptr));
        }
        else if (*i.value.begin() == L'?')
        {
            Node *epsilon = new Node(NODE_VAR, RE_EPSILON, nullptr, nullptr);
            evalStack.push(new Node(NODE_OR, '\0', old, epsilon));
        }
        else
            assert(0);
    }
    else if (i.value.id() == RegexpGrammar::exprId)
    {
        assert(i.children.size() == 2);
        evaluate(iHeader, i.children.front(), evalStack, iList);
        evaluate(iHeader, *(i.children.begin() + 1), evalStack, iList);

        Node *old2 = evalStack.top();
        evalStack.pop();
        Node *old1 = evalStack.top();
        evalStack.pop();
        evalStack.push(new Node(NODE_AND, '\0', old1, old2));
    }
    else if (i.value.id() == RegexpGrammar::wrapperId)
    {
        assert(i.children.size() == 2);
        evaluate(iHeader, i.children.front(), evalStack, iList);
        Node *old = evalStack.top();
        evalStack.pop();
        Node* sharp = new Node(NODE_VAR, RE_FINAL_TOK, nullptr, nullptr);
        evalStack.push(new Node(NODE_AND, '\0', old, sharp));
    }
    else
    {
        assert(0);
    }
}


bool parseRegexp(const Dictionary &iDic, const wchar_t *input, Node **root,
                 searchRegExpLists &iList)
{
    // Create a grammar object
    RegexpGrammar g(iDic.getHeader().getLetters());
    // Parse the input and generate an Abstract Syntax Tree (AST)
    tree_parse_info<const wchar_t*> info = ast_parse(input, g);

    if (info.full)
    {
#ifdef DEBUG_RE
        // Dump parse tree as XML
        static const std::map<parser_id, std::string_view> rule_names {
            { RegexpGrammar::wrapperId,  "wrapper"  },
            { RegexpGrammar::exprId,     "expr"     },
            { RegexpGrammar::repeatId,   "repeat"   },
            { RegexpGrammar::groupId,    "group"    },
            { RegexpGrammar::varId,      "var"      },
            { RegexpGrammar::choiceId,   "choice"   },
            { RegexpGrammar::alphavarId, "alphavar" }
        };
        tree_to_xml(cout, info.trees);
#endif

        stack<Node*> evalStack;
        evaluate(iDic.getHeader(), *info.trees.begin(), evalStack, iList);
        assert(evalStack.size() == 1);
        *root = evalStack.top();
        return true;
    }
    else
    {
        return false;
    }
}

