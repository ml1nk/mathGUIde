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

#include "inputwindow.h"
#include "constants.h"

#include <QRegExp>
#include <QApplication>
#include <QStatusBar>
#include <QDateTime>
#include <QFile>
#include <QTextCursor>
#include <QTextBlock>
#include <QKeyEvent>

//--------------------------------------------------------------
// class InputWindow
//--------------------------------------------------------------

InputWindow::InputWindow(QWidget* parent/*=0*/)
:   QTextEdit(parent),
    _previousTextLength(0),
    _tabWidth(4)
{
    //connect(this, SIGNAL(returnPressed()), this, SLOT(newLine()));
    connect(this, SIGNAL(textChanged()),   this, SLOT(onTextChanged()));
    setLineWrapMode(QTextEdit::NoWrap);
}

void InputWindow::moveLeft(int n, bool fromEnd/*=false*/) {
    QTextCursor cursor = textCursor();
    if (fromEnd)
        cursor.movePosition(QTextCursor::EndOfLine);
    cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, n);
    setTextCursor(cursor);
}

void InputWindow::newLine() {
    QTextCursor cursor = textCursor();
    int line = cursor.blockNumber();
    if (line > 0) {
        // use indentation of the previous line
        cursor.movePosition(QTextCursor::PreviousBlock);
        QString s = cursor.block().text();
        cursor = textCursor();
        int i = 0;
        while (i < (int)s.length() && (s[i] == ' ' || s[i] == '\t')) {
            cursor.insertText(s.mid(i,1));
            i++;
        }
        if (s.indexOf(QRegExp(":\\s*$")) >= 0) // colon at the end?
            for (int i=0; i<_tabWidth; i++)
                cursor.insertText(" ");
    }
}

void InputWindow::backspacePressed() {
    QKeyEvent ev(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
    keyPressEvent(&ev);
}

void InputWindow::textTyped(QString s) {
    QTextCursor cursor = textCursor();
    cursor.insertText(s);
}

void InputWindow::keyPressEvent(QKeyEvent* e) {
    int pos= textCursor().position();
    int len = toPlainText().length();
    bool cursorLeftOfPlaceholder = (pos < len && toPlainText()[pos] == Constants::placeholderChar);

    // if cursor is left of a placeholder, replace placeholder by typed text.
    if (e->text().length() > 0 && cursorLeftOfPlaceholder) {
            textCursor().deleteChar();
    }

    // if there are placeholders, right/left arrow moves cursor to next/previous placeholder.
    else if (e->key() == Qt::Key_Right) {
        int placeholderPos = toPlainText().indexOf(Constants::placeholderChar, pos);
        if (placeholderPos == pos)
            placeholderPos = toPlainText().indexOf(Constants::placeholderChar, pos+1);
        if (placeholderPos >= 0) {
            while (textCursor().position() < placeholderPos)
                moveCursor(QTextCursor::Right);
        }
        else
            QTextEdit::keyPressEvent(e);
        return;
    }
    else if (e->key() == Qt::Key_Left) {
        int placeholderPos = toPlainText().lastIndexOf(Constants::placeholderChar, pos);
        if (placeholderPos == pos)
            placeholderPos = toPlainText().lastIndexOf(Constants::placeholderChar, pos-1);
        if (placeholderPos >= 0) {
            while (textCursor().position() > placeholderPos)
                moveCursor(QTextCursor::Left);
        }
        else
            QTextEdit::keyPressEvent(e);
        return;
    }

    if (e->key() == Qt::Key_Space && (e->modifiers() & (Qt::ControlModifier | Qt::MetaModifier))) {
        QString s = Constants::findGlobals;
        emit inputChanged(s, cursorRect());
        return;
    }
    if (e->key() == Qt::Key_Escape) {
        emit escPressed();
        return;
    }
    if (e->key() == Qt::Key_Tab) {
        QTextCursor cursor = textCursor();
        cursor.insertText("    ");
        return;
    }
    if (e->key() == Qt::Key_Backspace) {
        QTextCursor cursor = textCursor();
        int col = cursor.columnNumber();
        QString s = cursor.block().text();
        if (col >= _tabWidth) {
            int p = col;
            while (p > 0 && s[p-1] == ' ')
                p--;
            if (p == 0) {
                p = col % _tabWidth;
                if (p == 0) p = _tabWidth;
                for (int i=0; i<p; i++)
                    QTextEdit::keyPressEvent(e);
                return;
            }
        }
    }
    QTextEdit::keyPressEvent(e);
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        newLine();
    }
}

void InputWindow::insertFromMimeData(const QMimeData* source) {
    QTextCursor cursor = textCursor();
    cursor.insertText(source->text());
}

void InputWindow::onTextChanged() {
    int newTextLength = toPlainText().length();
    if (newTextLength > _previousTextLength) {
        QTextCursor cursor = textCursor();
        int col = cursor.columnNumber();
        QString s = cursor.block().text().left(col);
        emit inputChanged(s, cursorRect());
    }
    _previousTextLength = toPlainText().length();
}

void InputWindow::insertStatement(QString s, int advanceCursor/*=4*/) {
    // the indentChar in s is interpreted as "same indentation as in cursor line
    QTextCursor cursor = textCursor();
    cursor.insertText(""); // (for getting the cursor position after deleting the selection)
    int col = cursor.columnNumber();
    QString t(col, ' ');
    s.replace(QString(Constants::indentChar), t);
    cursor.insertText(s);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, advanceCursor);
}
