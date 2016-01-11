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

#include "popupwidgets.h"
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QClipboard>


//--------------------------------------------------------------
// class PopupBrowser
//--------------------------------------------------------------

PopupBrowser::PopupBrowser(QWidget* parent/*=0*/)
: QWebView(parent)
{
    frame = new QFrame(parent, Qt::Popup);
    frame->setFrameShape(QFrame::StyledPanel);
    //frame->setFrameShadow(QFrame::Raised);
    QVBoxLayout* layout = new QVBoxLayout(frame);
    layout->setMargin(0);
    layout->addWidget(this);
}

void PopupBrowser::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Escape)
        emit escPressed();
    else if (e->key() == Qt::Key_Backspace)
        emit backspacePressed();
    else if (e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_C) {
        // copy selected Text to Clipboard
        QApplication::clipboard()->setText(selectedText());
    }
    else
        emit textTyped(e->text());
}

void PopupBrowser::showAtCursor(QRect cursorRect, QWidget* coordinatesWidget) {
    frame->adjustSize();
    QPoint ptTop = coordinatesWidget->mapToGlobal(cursorRect.topLeft());
    QPoint ptBtm = coordinatesWidget->mapToGlobal(cursorRect.bottomLeft());
    int bottom = QApplication::desktop()->availableGeometry().bottom();
    int frameHeight = frame->frameSize().height();
    if (ptBtm.y() + frameHeight < bottom)
        frame->move(ptBtm.x(), ptBtm.y());
    else
        frame->move(ptTop.x(), ptTop.y()-frameHeight);
    frame->show();
}

void PopupBrowser::hideFrame() {
    frame->hide();
}


//--------------------------------------------------------------
// class PopupList
//--------------------------------------------------------------

#ifdef Q_OS_MAC
PopupListFrame::PopupListFrame(QWidget* parent/*=0*/, Qt::WindowFlags f/*=0*/)
    : QFrame(parent, f)
{}

void PopupListFrame::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Return) {
        emit returnPressed();
    }
}
#endif

PopupList::PopupList(QWidget* parent/*=0*/)
: QListWidget(parent)
{
#ifdef Q_OS_MAC
    frame = new PopupListFrame(parent, Qt::Popup);
#else
    frame = new QFrame(parent, Qt::Popup);
#endif
    QVBoxLayout* layout = new QVBoxLayout(frame);
    layout->setMargin(0);
    layout->addWidget(this);
#ifdef Q_OS_MAC
    frame->popupList = this;
    connect(frame, SIGNAL(returnPressed()), this, SLOT(returnSelectedItem()));
#endif
}

#ifdef Q_OS_MAC
void PopupList::returnSelectedItem() {
    emit itemActivated(currentItem());
}
#endif

int PopupList::nCharsEntered() {
    return prefixEntered.size();
}

void PopupList::hideEvent(QHideEvent* e) {
    prefixEntered.clear();
    QListWidget::hideEvent(e);
}

void PopupList::keyPressEvent(QKeyEvent* e) {
    if (e->key() >= Qt::Key_Escape && e->key() != Qt::Key_Backspace) {
        if (e->key() == Qt::Key_Escape) {
            emit escPressed();
            prefixEntered.clear();
        }
        else
            QListWidget::keyPressEvent(e);
    }
    else {
        if (e->key() == Qt::Key_Backspace) {
            if (prefixEntered.length() > 0) {
                prefixEntered = prefixEntered.left(prefixEntered.length()-1);
            }
            else
                emit escPressed();
            emit backspacePressed();
        }
        else if (!e->text()[0].isLetter()) {
            emit textTyped(e->text());
            emit escPressed();
        }
        else {
            prefixEntered += e->text().toLower();
            emit textTyped(e->text());
        }
        for (int i=0; i<count(); i++) {
            if (item(i)->text().toLower().startsWith(prefixEntered)) {
                setCurrentRow(i);
                break;
            }
        }
    }
}

void PopupList::showAtCursor(QRect cursorRect, QWidget* coordinatesWidget) {
    frame->adjustSize();
    QPoint ptTop = coordinatesWidget->mapToGlobal(cursorRect.topLeft());
    QPoint ptBtm = coordinatesWidget->mapToGlobal(cursorRect.bottomLeft());
    int bottom = QApplication::desktop()->availableGeometry().bottom();
    int frameHeight = frame->frameSize().height();
    if (ptBtm.y() + frameHeight < bottom)
        frame->move(ptBtm.x(), ptBtm.y());
    else
        frame->move(ptTop.x(), ptTop.y()-frameHeight);
    frame->show();
}

void PopupList::hideFrame() {
    frame->hide();
}
