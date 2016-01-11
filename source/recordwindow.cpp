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
#include "recordwindow.h"

#include <QtGui>
#include <QScrollBar>
#include <QMessageBox>

//--------------------------------------------------------------
// class RecordWindow
//--------------------------------------------------------------

RecordWindow::RecordWindow(QWidget* parent/*=0*/)
:   QTextEdit(parent),
    _firstRecord(true)
{
    setReadOnly(true);
}

#define MARK_PY_COMMENTS
void RecordWindow::toHTML(QString& s, const QString& color,
                          bool markComments/*=false*/) {
    s.replace(QRegExp("<"), "&lt;"); // avoid mistaking < for HTML tag
    s.replace("\n", "<br/>");
    s.replace(" ", "&nbsp;");
    if (markComments)
        s.replace("font&nbsp;color", "font color");
    s.replace(QRegExp("\t"), "&nbsp;&nbsp;&nbsp;&nbsp;");
    if (!color.isEmpty())
        s = QString("<font color=") + color + ">" + s + "&nbsp;</font>";
    s = QString("<p>%1</p>").arg(s);
}

QStringList RecordWindow::splitHTML(const QString& s) {
    // example:
    // "1{html}h2{/html}3{html}h4{/html}5"  ---> QStringList["1", "h2", "3", "h4", "5"]
    QStringList l1 = s.split("{html}");
    QStringList l;
    foreach (QString t, l1) {
        l.append(t.split("{/html}"));
    }
    return l;
}

void RecordWindow::appendTexts(QString input, QString output, QString error) {
    // add input to repeating list
    _inputs.append(input);
    _cursor = _inputs.size()-1;
    emit cursorMoved(_cursor==0, true);

    // format output
    QString html;
    QStringList parts = RecordWindow::splitHTML(output);
    for (int i=0; i<parts.size(); i++) {
        // alternating simple text and HTML-preformatted:
        if (i % 2 == 0) {
            QString t = parts[i];
            toHTML(t, Constants::outputColor);
            html += t;
        }
        else {
            html += parts[i];
        }
    }
    output = html;

    // format error
    if (! error.isEmpty())
        error = QString("<p>%1</p>").arg(error);

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);

    QString display = (! _firstRecord ? "<p/><table width=\"100%\" height=\"3\" bgcolor=\"#376092\"><tr><td></td></tr></table><p/>" : "")
                      //+ input
                    + QString("<table width=\"100%\" bgcolor=\"#f8f8f8\"><tr><td>") + input + QString("</td></tr></table>")
                    + QString("<hr/>") + output + error;
    insertHtml(display);
    _firstRecord = false;
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

QString RecordWindow::curInput() const {
    if (! _inputs.empty())
        return _inputs[_cursor];
    return "";
}

void RecordWindow::moveCursor(bool down) {
    int c = _cursor;
    if (down && _cursor < (int)_inputs.size()-1)
        _cursor++;
    if (! down && _cursor > 0)
        _cursor--;
    if (_cursor != c)
        emit cursorMoved(_cursor==0, _cursor==_inputs.size()-1);
}

void RecordWindow::saveHtml(QString& fileName) {
    if (! fileName.isEmpty()) {
        if (fileName.right(5) != ".html")
            fileName += ".html";
        if (QFile::exists(fileName)
                && QMessageBox::warning(this,
                        tr("mathGUIde"),
                        tr("The file %1 existists.\nOverwrite?")
                            .arg(QDir::toNativeSeparators(fileName)),
                        QMessageBox::Yes | QMessageBox::Default,
                        QMessageBox::No | QMessageBox::Escape)
                   == QMessageBox::No)
            return;
        QFile f(fileName);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&f);
        out << toHtml().replace("src=\":/", "src=\"./");
        f.close();
    }
}
