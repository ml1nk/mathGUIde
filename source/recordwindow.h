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

#ifndef __RECORDWINDOW_H__
#define __RECORDWINDOW_H__

#include <QWidget>
#include <QTextEdit>
#include <QStringList>

class RecordWindow: public QTextEdit {
    Q_OBJECT
public:
    RecordWindow(QWidget* parent=0);
    void appendTexts(QString input, QString output, QString error);
    QString curInput() const;
public slots:
    void moveCursor(bool down);
    //void saveDemo(QString& fileName);
    void saveHtml(QString& fileName);
private:
    static QStringList splitHTML(const QString& s);
    static void toHTML(QString& s, const QString& color, bool markComments=false);
    bool        _firstRecord;
    QStringList _inputs;
    int         _cursor;
signals:
    void cursorMoved(bool top, bool btm);
};

#endif //__RECORDWINDOW_H__
