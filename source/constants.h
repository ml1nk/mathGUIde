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
 
#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <QChar>
#include <QString>

class Constants {
public:
    static const QChar indentChar;
    static const QChar placeholderChar;
    static const QChar errorPrefixChar;

    static const QString outputColor;
    static const QString errorColor;
    static const QString commentColor;

    static const QString findGlobals;
};

#endif //__CONSTANTS_H__
