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
 
#ifndef PY_HIGHLIGHTER_H
#define PY_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QList>
#include <QStringList>
#include <QRegExp>

class PythonHighlighter: QSyntaxHighlighter {
    Q_OBJECT
public:
    PythonHighlighter(QTextDocument* document);

protected:
    virtual void highlightBlock(const QString& text);

    void createRules();

    QStringList     _keywords;
    QList<QRegExp>  _keywordPatterns;

    QTextCharFormat _keywordFormat;
    QTextCharFormat _commentFormat;
    QTextCharFormat _errorFormat;
    QTextCharFormat _literalStringFormat;
    QTextCharFormat _multiLineStringFormat;
    QTextCharFormat _placeholderFormat;
};

#endif // PY_HIGHLIGHTER_H
