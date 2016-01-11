/*-------------------------------------------------------------------+
 |     Copyright (c) 2006-2010 Hartmut Ring, University Siegen       |
 |     http://www.mathgui.de                                         |
 |                                                                   |
 |     This file is part of the mathGUIde package.                   |
 |                                                                   |
 | mathGUIde is free software: you can redistribute it and/or modify |
 | it under the terms of the GNU General Public License as published |
 | by the Free Software Foundation, either version 3 of the License, |
 | or (at your option) any later version.                            |
 |                                                                   |
 | You should have received a copy of the GNU General Public License |
 | along with this program. If not, see http://www.gnu.org/licenses/.|
 +-------------------------------------------------------------------*/

#include "constants.h"
#include "py-highlighter.h"

PythonHighlighter::PythonHighlighter(QTextDocument* document)
:   QSyntaxHighlighter(document)
{
    _keywords                   << "False"    << "None"     <<  "True"
    << "and"      << "as"       << "assert"   << "break"    << "class"
    << "continue" << "def"      << "del"      << "elif"     << "else"
    << "except"   << "finally"  << "for"      << "from"     << "global"
    << "if"       << "import"   << "in"       << "is"       << "lambda"
    << "nonlocal" << "not"      << "or"       << "pass"     << "raise"
    << "return"   << "try"      << "while"    << "with"     << "yield";

    createRules();
}

void PythonHighlighter::highlightBlock(const QString &text) {
    if (text.indexOf(Constants::errorPrefixChar) >= 0) {
        setFormat(0, text.length(), _errorFormat);
        return;
    }

    foreach (QRegExp expression, _keywordPatterns) {
         int index = text.indexOf(expression);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, _keywordFormat);
             index = text.indexOf(expression, index + length);
         }
     }

    int index = text.indexOf(Constants::placeholderChar);
     while (index >= 0) {
         setFormat(index, 1, _placeholderFormat);
         index = text.indexOf(Constants::placeholderChar, index + 1);
     }


    enum {NormalState=-1, InsideMultiLineString1, InsideNormalString1,
                          InsideMultiLineString2, InsideNormalString2};

    int pStart = 0;
    int state = previousBlockState();

    while (pStart >= 0) {
        switch (state) {
            case NormalState: {
                int q1 = text.indexOf("'", pStart);      //  '
                int q3 = text.indexOf("'''", pStart);    //  '''
                int Q1 = text.indexOf("\"", pStart);     //  "
                int Q3 = text.indexOf("\"\"\"", pStart); //  """
                int sharp = text.indexOf("#", pStart);   //  #

                if (sharp >= 0 && (q1 < 0 || sharp < q1) && (Q1 < 0 || sharp < Q1)) {
                    // first special char is #  ===> line end comment
                    setFormat(sharp, text.length()-sharp, _commentFormat);
                    pStart = -1;
                }

                else if (Q1 >= 0 && (q1 < 0 || Q1 < q1)) {
                    // first special char is "
                    if (Q3 < 0 || Q1 < Q3) {
                        // "double quoted string literal"
                        state = InsideNormalString2;
                        setFormat(Q1, 1, _literalStringFormat);
                        pStart = Q1 + 1;
                    }
                    else if (Q3 >= 0) {
                        // """double quoted multi line string literal"""
                        state = InsideMultiLineString2;
                        setFormat(Q3, 3, _multiLineStringFormat);
                        pStart = Q3 + 3;
                    }
                }

                else if (q1 >= 0 && (Q1 < 0 || q1 < Q1)) {
                    // first special char is '
                    if (q3 < 0 || q1 < q3) {
                        // 'single quoted string literal'
                        state = InsideNormalString1;
                        setFormat(q1, 1, _literalStringFormat);
                        pStart = q1 + 1;
                    }
                    else if (q3 >= 0) {
                        // '''single quoted multi line string literal'''
                        state = InsideMultiLineString1;
                        setFormat(q3, 3, _multiLineStringFormat);
                        pStart = q3 + 3;
                    }
                }
                else
                    pStart = -1;
                break;
            }
            case InsideNormalString1:
            case InsideNormalString2: {
                QString delimiter = (state == InsideNormalString1) ? "'" : "\"";
                int pEnd = text.indexOf(delimiter, pStart);
                if (pEnd < 0) {
                    setFormat(pStart, text.length()-pStart, _literalStringFormat);
                    pStart = -1;
                }
                else {
                    setFormat(pStart, pEnd+1-pStart, _literalStringFormat);
                    pStart = pEnd + 1;
                }
                state = NormalState;
                break;
            }
            case InsideMultiLineString1:
            case InsideMultiLineString2: {
                QString delimiter = (state == InsideMultiLineString1) ? "'''" : "\"\"\"";
                int pEnd = text.indexOf(delimiter, pStart);
                if (pEnd < 0) {
                    setFormat(pStart, text.length()-pStart, _multiLineStringFormat);
                    pStart = -1;
                }
                else {
                    state = NormalState;
                    setFormat(pStart, pEnd+3-pStart, _multiLineStringFormat);
                    pStart = pEnd + 3;
                }
                break;
            }
        }
        setCurrentBlockState(state);
    }
}

void PythonHighlighter::createRules() {
    _keywordFormat.setForeground(Qt::darkBlue);
    _keywordFormat.setFontWeight(QFont::Bold);

    _commentFormat.setForeground(Qt::darkGreen);
    _errorFormat.setForeground(QColor(224, 0, 32));
  
    _literalStringFormat.setForeground(Qt::darkRed);
    _multiLineStringFormat.setForeground(Qt::darkGray);

    _placeholderFormat.setBackground(QColor(198,217,241));
    _placeholderFormat.setForeground(QColor(142,180,227));

    // Keywords
    _keywordPatterns.clear();
    QString pattern = "\\b%1\\b";
    foreach (QString keyword, _keywords) {
        _keywordPatterns.append(QRegExp(pattern.arg(keyword)));
    }
}
