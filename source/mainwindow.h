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
 
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextBrowser>
#include <QSettings>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDockWidget>
#include <QProgressBar>
#include <QtWebKit>

#include "py-inter.h"
#include "popupwidgets.h"


class RecordWindow;
class InputWindow;
class PlotWindow;
class GraphWidget;
class PythonHighlighter;

class GuideView: public QWebView {
    Q_OBJECT
public:
    GuideView(QWidget* parent, QSettings* settings);
protected slots:
    void copy();
    void zoomIn();
    void zoomOut();
    void zoomReset();
private:
    QSettings* _settings;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QString dataPath);
    ~MainWindow();
    QString userLanguageId;
protected:
    virtual void closeEvent(QCloseEvent*);
protected slots:
    void about();
    void quit();
    void toggleGuideVisible();
    void guideForward();
    void guideBack();
    void home();
    void activateReference();
    void helpGraph();
    void search();
    void searchNext();
    void toggleGuideDocking();

    void saveHtml();
    void openGuide();
    void insertPlot();
    void showStatusMessage(QString);
    void contextHelp(QString, QRect);
    void hideTipWindows();
    void showMethodToolTip();
    void up();
    void down();
    void insertSum();
    void insertProd();
    void insertRational();
    void insertIf();
    void insertFor();
    void insertMatrix();
    void insertList();
    void insertTable();
    void insertValueTable();

    void calc();
    void calcString(QString s);

    void selectOptions();
    void selectFont();
    void clearSettings();

    void insertListBoxItem(QListWidgetItem* item);

    void recordCursorMoved(bool, bool);
    void setLanguage(QString lang);
    void setIconStyle(int style, int size);
    void handleGuideLink(const QUrl&);
    void handleGuideHover(const QString& link, const QString& title, const QString& textContent);
    void webLoadStarted();
    void webLoadProgress(int percent);
    void updateNavigation(bool);
    void updateGuideVisibility(bool);

signals:
    void calcStringResult(QString output, QString error);

private:
    void createWidgets();
    void createActions();
    void createToolbar();
    void createMenu();
    void createGuide();
    void createConnections();
    void showGuide();
    void updateGuideDocked();

    QAction* _homeAction;
    QAction* _clearRecordsAction;
    QAction* _saveHtmlAction;
    QAction* _quitAction;
    QAction* _calcAction;
    QAction* _upAction;
    QAction* _downAction;
    QAction* _sumAction;
    QAction* _prodAction;
    QAction* _rationalAction;
    QAction* _ifAction;
    QAction* _forAction;
    QAction* _matrixAction;
    QAction* _listAction;
    QAction* _valueTableAction;
    QAction* _tableAction;
    QAction* _plotAction;

    QAction* _optionsAction;
    QAction* _aboutAction;
    QAction* _referenceAction;

    QAction* _toggleGuideVisibleAction;
    QAction* _toggleGuideDockingAction;
    QAction* _guideForwardAction;
    QAction* _guideBackAction;
    QAction* _searchAction;
    QAction* _searchNextAction;

    QAction* _guideOpenAction;
    QAction* _guideCopyAction;
    QAction* _guideZoomInAction;
    QAction* _guideZoomOutAction;
    QAction* _guideZoomResetAction;

    QAction* _escapeAction;

    PythonInterpreter  _python;
    QString            _dataPath;

    QToolBar*          _toolbar;
    QToolBar*          _browserToolbar;
    QProgressBar*      _browserProgressBar;
    QSplitter*         _workWnd;
    QTabWidget*        _tabView;
    QTabWidget*        _graphTabView;
    InputWindow*       _inputWnd;
    RecordWindow*      _recordWnd;
    PlotWindow*        _plotWnd;
    QList<GraphWidget*> _graphWidgets;
    static const int   _maxGraphTabs;

    GuideView*         _guideView;
    QDockWidget*       _guideDock;

    PythonHighlighter* _pythonHighlighter;

    PopupList*         _methodsPopup;
    PopupBrowser*      _parametersPopup;

    QString            _placeholderInfo;
    QFont              _font;
    QSettings          _settings;
    bool               _saveSettings;

    QString            _style;

    QIcon              _classIcon;
    QIcon              _functionIcon;
    QIcon              _elementIcon;
};

#endif
