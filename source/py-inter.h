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

#ifndef __PY_INTER_H__
#define __PY_INTER_H__

#include <QString> 
#include <QStringList> 

class PythonInterpreter {
public:
    PythonInterpreter(QString dataPath);
    ~PythonInterpreter();

    bool runString(QString input, QString& output, QString& error);
    QString getContextInfo(const QString& line);
    void getMethod(QString sourceLine, QString& cls, QString& method);
    QString source(QString method, QString cls="global");
    QString editError(QString error, QString errorPrefix);

    bool isOk;
protected:
    void _initFiles();
    QString _dataPath;
    QString _errFile;
    QString _outFile;
    QString _source;
};

#endif //__PY_INTER_H__
