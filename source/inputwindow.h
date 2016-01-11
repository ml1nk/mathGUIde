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

#ifndef __INPUTWINDOW_H__
#define __INPUTWINDOW_H__

#include <QTextEdit>
#include <QWidget>

class InputWindow: public QTextEdit {
    Q_OBJECT
public:
    InputWindow(QWidget* parent=0);
    void insertStatement(QString s, int advanceCursor=4);
    void moveLeft(int n, bool fromEnd=false);
public slots:
    void newLine();
    void onTextChanged();
    void backspacePressed();
    void textTyped(QString s);
protected:
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void insertFromMimeData(const QMimeData* source);
signals:
    void statusMessage(QString msg);
    void inputChanged(QString, QRect);
    void escPressed();
protected:
    int           _previousTextLength;
    int           _tabWidth;
    QStringList   _presentation;
    int           _presentationPos;
};

#endif //__INPUTWINDOW_H__
