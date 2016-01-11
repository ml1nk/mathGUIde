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
 

#include <QClipboard>
#include <QFontDialog>
#include "dialogs.h"

//------ OptionsDialog ------
OptionsDialog::OptionsDialog(QWidget* parent/*=0*/)
    : QDialog(parent)
{
    setupUi(this);
    comboBoxToolbar->addItem(tr("Display icons only"));
    comboBoxToolbar->addItem(tr("Display text only"));
    comboBoxToolbar->addItem(tr("Display text beside the icon"));
    comboBoxToolbar->addItem(tr("Display text under the icon"));

    reset = false;
    connect(buttonChangeFont, SIGNAL(pressed()), this, SLOT(selectCurrentFont()));
    connect(buttonReset, SIGNAL(pressed()), this, SLOT(resetAll()));
    connect(sliderIconSize, SIGNAL(valueChanged(int)), spinBoxIconSize, SLOT(setValue(int)));
    connect(spinBoxIconSize, SIGNAL(valueChanged(int)), sliderIconSize, SLOT(setValue(int)));
}

void OptionsDialog::setIconStyle(int style) {
    int iconStyle = abs(style) % 4;
    comboBoxToolbar->setCurrentIndex(iconStyle);
}

int OptionsDialog::iconStyle() {
    return comboBoxToolbar->currentIndex();
}

void OptionsDialog::setIconSize(int size) {
    if (size < 16) size = 16;
    else if (size > 48) size = 48;
    sliderIconSize->setValue(size);
}

int OptionsDialog::iconSize() {
    return sliderIconSize->value();
}

void OptionsDialog::setCurrentFont(const QFont& font) {
    currentFont = font;
    QString fontDescr = QString("%1 %2").arg(font.family()).arg(font.pointSize());
    if (font.bold())
        fontDescr += QString(", ") + tr("bold");
    if (font.italic())
        fontDescr += QString(", ") + tr("italic");
    labelCurrentFont->setText(fontDescr);
    editExample->setFont(font);
}

void OptionsDialog::selectCurrentFont() {
#ifdef Q_WS_MAC
    // QFontDialog::getFont does not work on Mac OS (always returns Lucida Grande).
    QFontDialog fontDialog(this);
    fontDialog.setCurrentFont(currentFont);
    fontDialog.setOption(QFontDialog::DontUseNativeDialog, true);
    if (fontDialog.exec() == QFontDialog::Accepted)
        setCurrentFont(fontDialog.currentFont());
#else
    bool ok;
    QFont f = QFontDialog::getFont(&ok, currentFont, this);
    if (ok)
        setCurrentFont(f);
#endif
}

void OptionsDialog::resetAll() {
    reset = true;
    accept();
}

//------ SumDialog ------
SumDialog::SumDialog(QWidget* parent/*=0*/)
    : QDialog(parent)
{
    setupUi(this);
}

//------ ProdDialog ------
ProdDialog::ProdDialog(QWidget* parent/*=0*/)
    : QDialog(parent)
{
    setupUi(this);
}

//------ ListDialog ------
ListDialog::ListDialog(QWidget* parent/*=0*/)
    : QDialog(parent)
{
    setupUi(this);
}

//------ MatrixDialog ------
MatrixDialog::MatrixDialog(QWidget* parent/*=0*/)
    : QDialog(parent)
{
    setupUi(this);
    connect(radioButton2, SIGNAL(toggled(bool)), textLabel4, SLOT(setEnabled(bool)));
    connect(radioButton2, SIGNAL(toggled(bool)), editI, SLOT(setEnabled(bool)));
    connect(radioButton2, SIGNAL(toggled(bool)), textLabel1, SLOT(setEnabled(bool)));
    connect(radioButton2, SIGNAL(toggled(bool)), editK, SLOT(setEnabled(bool)));
    connect(radioButton2, SIGNAL(toggled(bool)), textLabel6, SLOT(setEnabled(bool)));
    connect(radioButton2, SIGNAL(toggled(bool)), editAik, SLOT(setEnabled(bool)));
    connect(radioButton2, SIGNAL(toggled(bool)), checkIndex1, SLOT(setEnabled(bool)));
}

//------ TableDialog ------
static QString pyPPP(QString s) {
    s.replace("^^", "<bit-xor>");  // use ^^ instead of ^ for bitwise xor
    s.replace("^", "**");          // use ^ instead of ** for exponentiation
    s.replace("<bit-xor>", "^");
    return s;
}

TableDialog::TableDialog(QWidget* parent/*=0*/)
  : QDialog(parent),
    timer(this),
    changed(false)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(updatePreview()));
    timer.start(1000);

    alignments << tr("left") << tr("centred") << tr("right");
    htmlAlignments << "left" << "center" << "right";
    setupUi(this);
    comboAlignCell->addItems(alignments);
    comboAlignCell->setCurrentIndex(2);
    comboAlignHead->addItems(alignments);
    comboAlignHead->setCurrentIndex(0);

    connect(checkBoxRowHeader, SIGNAL(toggled(bool)),               this, SLOT(rowHeaderToggled(bool)));
    connect(checkBoxColHeader, SIGNAL(toggled(bool)),               this, SLOT(colHeaderToggled(bool)));
    connect(editCellFn,        SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));
    connect(spinBoxHeight,     SIGNAL(valueChanged(int)),           this, SLOT(previewChanged()));
    connect(spinBoxWidth,      SIGNAL(valueChanged(int)),           this, SLOT(previewChanged()));
    connect(editRowHeadFn,     SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));
    connect(editColHeadFn,     SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));
    connect(comboAlignCell,    SIGNAL(currentIndexChanged(int)),    this, SLOT(previewChanged()));
    connect(comboAlignHead,    SIGNAL(currentIndexChanged(int)),    this, SLOT(previewChanged()));
    connect(editTitle,         SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));
    connect(editCorner,        SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));
}

void TableDialog::rowHeaderToggled(bool checked) {
    editRowHeadFn->setEnabled(checked);
    previewChanged();
}

void TableDialog::colHeaderToggled(bool checked) {
    editColHeadFn->setEnabled(checked);
    previewChanged();
}

void TableDialog::pythonResult(QString output, QString /*error*/) {
    textEditPreview->setHtml(output.replace("{html}", "").replace("{/html}", ""));
}

QString TableDialog::getPythonCall() {
    int nRows = spinBoxHeight->value();
    int nCols = spinBoxWidth->value();
    QString cellFn = editCellFn->text();

    bool showRowHead = checkBoxRowHeader->isChecked();
    QString rowHeadFn = editRowHeadFn->text();
    QString rowHeadFnParam = showRowHead ? QString("rowHeadFn = lambda i: %1, ").arg(rowHeadFn) : "";

    bool showColHead = checkBoxColHeader->isChecked();
    QString colHeadFn = editColHeadFn->text();
    QString colHeadFnParam = showColHead ? QString("colHeadFn = lambda k: %1, ").arg(colHeadFn) : "";

    QString headAlign = htmlAlignments[comboAlignHead->currentIndex()];
    QString cellAlign = htmlAlignments[comboAlignCell->currentIndex()];
    QString title = editTitle->text();
    QString corner = editCorner->text();

    bool cornerVisible = showRowHead && showColHead;
    editCorner->setEnabled(cornerVisible);
    labelCorner->setEnabled(cornerVisible);

    QString pyCall = QString("printTable(%1,%2, lambda i,k: %3, %4 %5"
        "headAlign = \"%6\", cellAlign = \"%7\", title = \"%8\",  corner = \"%9\")")
        .arg(nRows)
        .arg(nCols)
        .arg(cellFn)    // "10*i+k"
        .arg(rowHeadFnParam) // "rowHeadFn = lambda i: 10*(i+1), "
        .arg(colHeadFnParam) // "colHeadFn = lambda k: k+1, "
        .arg(headAlign) // "left"
        .arg(cellAlign) // "right"
        .arg(title)     // "Addition"
        .arg(corner);    // "+"

    //pyCall.replace("^^", "<bit-xor>");  // use ^^ instead of ^ for bitwise xor
    //pyCall.replace("^", "**");          // use ^ instead of ** for exponentiation
    //pyCall.replace("<bit-xor>", "^");
    return pyPPP(pyCall);
}

void TableDialog::previewChanged() {
    changed = true;
}

void TableDialog::updatePreview() {
    if (changed) {
        QString pyCall = getPythonCall();
        emit pythonCalc(pyCall);
        changed = false;
    }
}

//------ ValueTableDialogX ------
ValueTableDialogX::ValueTableDialogX(QWidget* parent/*=0*/)
  : QDialog(parent),
    timer(this),
    changed(false)
{
    setupUi(this);
    connect(checkFn2, SIGNAL(toggled(bool)), this,         SLOT(secondFunctionToggled(bool)));
    connect(checkFn3, SIGNAL(toggled(bool)), editFn3Value, SLOT(setEnabled(bool)));
    connect(checkFn3, SIGNAL(toggled(bool)), editFn3Title, SLOT(setEnabled(bool)));

    connect(&timer, SIGNAL(timeout()), this, SLOT(updatePreview()));
    timer.start(1000);

    alignments << tr("left") << tr("centred") << tr("right");
    htmlAlignments << "left" << "center" << "right";

    connect(editVariable,     SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));
    connect(editFrom,         SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));
    connect(editTo,           SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));
    connect(editInc,          SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));

    connect(editFn1Value,     SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));
    connect(editFn2Value,     SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));
    connect(editFn3Value,     SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));
    connect(editFn1Title,     SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));
    connect(editFn2Title,     SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));
    connect(editFn3Title,     SIGNAL(textChanged(const QString&)), this, SLOT(previewChanged()));

    connect(checkFn2,         SIGNAL(toggled(bool)),               this, SLOT(previewChanged()));
    connect(checkFn3,         SIGNAL(toggled(bool)),               this, SLOT(previewChanged()));
    connect(checkBoxVertical, SIGNAL(toggled(bool)),               this, SLOT(previewChanged()));
}

void ValueTableDialogX::secondFunctionToggled(bool checked) {
    editFn2Value->setEnabled(checked);
    editFn2Title->setEnabled(checked);
    checkFn3->setEnabled(checked);
    if (checked) {
        editFn3Value->setEnabled(checkFn3->isChecked());
        editFn3Title->setEnabled(checkFn3->isChecked());
    }
    else {
        checkFn3->setChecked(false);
        editFn3Value->setEnabled(false);
        editFn3Title->setEnabled(false);
    }
}

void ValueTableDialogX::pythonResult(QString output, QString error) {
    textEditPreview->setHtml(
            QString("<span style=\"color:#E00020\">%1</span>%2")
            .arg(error)
            .arg(output.replace("{html}", "").replace("{/html}", ""))
    );
}

static QString lambdaExpression(QString var, QString value, QString title) {
    return QString("(lambda %1: %2, \"%3\")").arg(var).arg(value).arg(title);
}

QString ValueTableDialogX::getPythonCall() {
    QString var = editVariable->text();
    QString from = editFrom->text();
    QString to = editTo->text();
    QString inc = editInc->text();
    QString vert = this->checkBoxVertical->isChecked() ? "True" : "False";

    QStringList functionValues;
    QString t = editFn1Title->text();
    functionValues.push_back(lambdaExpression(var, pyPPP(editFn1Value->text()), t.isEmpty() ? editFn1Value->text() : t));
    if (checkFn2->isChecked()) {
        QString t = editFn2Title->text();
        functionValues.push_back(lambdaExpression(var, pyPPP(editFn2Value->text()), t.isEmpty() ? editFn2Value->text() : t));
    }
    if (checkFn3->isChecked()) {
        QString t = editFn3Title->text();
        functionValues.push_back(lambdaExpression(var, pyPPP(editFn3Value->text()), t.isEmpty() ? editFn3Value->text() : t));
    }

    // Example: printValueTable([("lambda n:n^2", "n²"), ("lambda n:n*(n+1)", "n*(n+1)")], "n", 0, 10, True)
    QString values = functionValues.join(",");
    QString lambdaExpressions = "";
    QString pyCall = QString("printValueTable([%1], \"%2\", %3, %4, %5, %6)")
        .arg(values).arg(var).arg(from).arg(to).arg(inc).arg(vert);

    return pyCall;
}

void ValueTableDialogX::previewChanged() {
    changed = true;
}

void ValueTableDialogX::updatePreview() {
    if (changed) {
        QString pyCall = getPythonCall();
        emit pythonCalc(pyCall);
        changed = false;
    }
}

#ifdef OLD_ValueTableDialog
//------ ValueTableDialog ------
ValueTableDialog::ValueTableDialog(QWidget* parent/*=0*/)
    : QDialog(parent)
{
    setupUi(this);
    connect(checkFn2, SIGNAL(toggled(bool)), this,    SLOT(secondFunctionToggled(bool)));
    connect(checkFn3, SIGNAL(toggled(bool)), editFn3, SLOT(setEnabled(bool)));
}

void ValueTableDialog::secondFunctionToggled(bool checked) {
    editFn2->setEnabled(checked);
    checkFn3->setEnabled(checked);
    if (checked) {
        editFn3->setEnabled(checkFn3->isChecked());
    }
    else {
        checkFn3->setChecked(false);
        editFn3->setEnabled(false);
    }
}
#endif

//------ PlotDialog ------
PlotDialog::PlotDialog(QWidget* parent/*=0*/)
    : QDialog(parent)
{
    setupUi(this);
    connect(checkFn3, SIGNAL(toggled(bool)), editFn3, SLOT(setEnabled(bool)));
    connect(checkFn2, SIGNAL(toggled(bool)), this, SLOT(secondFunctionToggled(bool)));
}

void PlotDialog::secondFunctionToggled(bool checked) {
    editFn2->setEnabled(checked);
    checkFn3->setEnabled(checked);
    if (checked) {
        editFn3->setEnabled(checkFn3->isChecked());
    }
    else {
        checkFn3->setChecked(false);
        editFn3->setEnabled(false);
    }
}

