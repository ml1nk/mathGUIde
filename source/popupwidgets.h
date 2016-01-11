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
 
#ifndef POPUPTEXT_H
#define POPUPTEXT_H

#include <QTextEdit>
#include <QWebView>
#include <QListWidget>
#include <QFrame>


class PopupBrowser: public QWebView {
    Q_OBJECT
public:
    PopupBrowser(QWidget* parent=0);
    virtual void keyPressEvent(QKeyEvent* e);
    void showAtCursor(QRect cursorRect, QWidget* coordinatesWidget);
    void hideFrame();
signals:
    void escPressed();
    void backspacePressed();
    void textTyped(QString s);
private:
    QFrame* frame;
};

#ifdef Q_OS_MAC
// In Mac OS X QListWidget keyPressEvent() is not called when the return key is pressed.
// So this is passed from the frame to the list

class PopupList;

class PopupListFrame: public QFrame {
    Q_OBJECT
public:
    PopupListFrame(QWidget* parent=0, Qt::WindowFlags f=0);
    virtual void keyPressEvent(QKeyEvent* e);
    PopupList* popupList;
signals:
    void returnPressed();
};
#endif

class PopupList: public QListWidget {
    Q_OBJECT
public:
    PopupList(QWidget* parent=0);
    virtual void keyPressEvent(QKeyEvent* e);
    int nCharsEntered();
    void showAtCursor(QRect cursorRect, QWidget* coordinatesWidget);
    void hideFrame();
signals:
    void escPressed();
    void backspacePressed();
    void textTyped(QString s);
protected:
    virtual void hideEvent(QHideEvent* e);
private:
    QString prefixEntered;
#ifdef Q_OS_MAC
    PopupListFrame* frame;
protected slots:
    void returnSelectedItem();
#else
    QFrame* frame;
#endif
};

#endif // POPUPTEXT_H
