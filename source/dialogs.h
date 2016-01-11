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
 
#ifndef DIALOGS_H
#define DIALOGS_H

#include <QDialog>
#include <QTimer>
#include "ui_optionsdlg.h"
#include "ui_sumdlg.h"
#include "ui_proddlg.h"
#include "ui_listdlg.h"
#include "ui_matrixdlg.h"
#include "ui_tabledlg.h"
#include "ui_valuetabledlgx.h"
#include "ui_plotdlg.h"


class OptionsDialog: public QDialog, public Ui::OptionsDialog {
    Q_OBJECT
public:
    OptionsDialog(QWidget* parent=0);
    void setCurrentFont(const QFont& font);
    QFont currentFont;
    void setIconStyle(int style);
    int iconStyle();
    void setIconSize(int size);
    int iconSize();
    bool reset;
protected slots:
    void selectCurrentFont();
    void resetAll();
};

class SumDialog: public QDialog, public Ui::SumDialog {
    Q_OBJECT
public:
    SumDialog(QWidget* parent=0);
};

class ProdDialog: public QDialog, public Ui::ProdDialog {
    Q_OBJECT
public:
    ProdDialog(QWidget* parent=0);
};

class ListDialog: public QDialog, public Ui::ListDialog {
    Q_OBJECT
public:
    ListDialog(QWidget* parent=0);
};

class MatrixDialog: public QDialog, public Ui::MatrixDialog {
    Q_OBJECT
public:
    MatrixDialog(QWidget* parent=0);
};

class TableDialog: public QDialog, public Ui::TableDialog {
    Q_OBJECT
public:
    TableDialog(QWidget* parent=0);
    QString getPythonCall();
public slots:
    void previewChanged();
    void updatePreview();
protected slots:
    void rowHeaderToggled(bool checked);
    void colHeaderToggled(bool checked);
    void pythonResult(QString output, QString error);
signals:
    void pythonCalc(QString s);
private:
    QStringList alignments;
    QStringList htmlAlignments;
    QTimer timer;
    bool changed;
};

class ValueTableDialogX: public QDialog, public Ui::ValueTableDialogX {
    Q_OBJECT
public:
    ValueTableDialogX(QWidget* parent=0);
    QString getPythonCall();
public slots:
    void previewChanged();
    void updatePreview();
protected slots:
    void pythonResult(QString output, QString error);
    void secondFunctionToggled(bool checked);
signals:
    void pythonCalc(QString s);
private:
    QStringList alignments;
    QStringList htmlAlignments;
    QTimer timer;
    bool changed;
};

#ifdef OLD_ValueTableDialog
class ValueTableDialog: public QDialog, public Ui::ValueTableDialog {
    Q_OBJECT
public:
    ValueTableDialog(QWidget* parent=0);
protected slots:
    void secondFunctionToggled(bool checked);
};
#endif

class PlotDialog: public QDialog, public Ui::PlotDialog {
    Q_OBJECT
public:
    PlotDialog(QWidget* parent=0);
protected slots:
    void secondFunctionToggled(bool checked);
};

#endif // DIALOGS_H
