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
#include "mainwindow.h"
#include "recordwindow.h"
#include "inputwindow.h"
#include "plotwindow.h"
#include "graphwidget.h"
#include "dialogs.h"
#include "py-highlighter.h"
#include "popupwidgets.h"

#include <QApplication>
#include <QAction>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QMouseEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QTime>
#include <QMessageBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QScrollBar>
#include <QProcess>
#include <QInputDialog>
#include <QClipboard>
#include <QKeySequence>

//--------------------------------------------------------------
// class GuideView
//--------------------------------------------------------------

#include <QInputDialog>

GuideView::GuideView(QWidget* parent, QSettings* settings)
: QWebView(parent)
{
    _settings = settings;
    //page()->setContentEditable(true); // with this set to true scrolling with PgUp etc.
}                                       // does not work after klicking in the view!!!

void GuideView::copy() {
    QApplication::clipboard()->setText(selectedText());
}

void GuideView::zoomIn()    { setZoomFactor(zoomFactor() * 1.1); }
void GuideView::zoomOut()   { setZoomFactor(zoomFactor() / 1.1); }
void GuideView::zoomReset() { setZoomFactor(1.0); }


//--------------------------------------------------------------
// class MainWindow
//--------------------------------------------------------------

MainWindow::MainWindow(QString dataPath)
:   QMainWindow(0, 0),
    _python(dataPath),
    _dataPath(dataPath),
    _guideDock(0),
    _font("Courier", 14),
    _settings("HartmutRing", "mathGUIde"),
    _saveSettings(true),
    _classIcon(":/img/members/class.png"),
    _functionIcon(":/img/members/method.png"),
    _elementIcon(":/img/members/element.png")
{
    _font.setStyleHint(QFont::TypeWriter);
    userLanguageId = _settings.value("/lang", "en").toString();
    _placeholderInfo = tr("Navigate between the placeholders using the arrow keys (left/right)");

    QString s = _settings.value("/Font").toString();
    if (s.isEmpty()) {
#ifdef Q_WS_MAC
      _font.setFamily("Monaco");
      _font.setPointSize(14);
#elif defined(Q_WS_X11)
      _font.setFamily("Monospace");
      _font.setPointSize(11);
#else
      _font.setFamily("Courier");
      _font.setPointSize(12);
#endif
    }
    else
        _font.fromString(s);
    setWindowTitle("mathGUIde 2.1");

    createActions();
    createWidgets();
    createGuide();
    createToolbar();
    createMenu();

    setIconStyle(_settings.value("/Toolbar/IconStyle", 0).toInt(),
                 _settings.value("/Toolbar/IconSize", 24).toInt());
    createConnections();

    if (! _python.isOk) {
        QString mathguidePyPath = QCoreApplication::applicationDirPath(); // default: application path
        QFile f(QString("%1/mathguide.py").arg(dataPath));
        if (!f.exists())
            QMessageBox::warning(0, "mathGUIde",
                    tr("<h3>The file \"mathguide.py\" was not found.</h3>"
                    "<p>Please install mathGUide again or adapt the file \"settings.xml\" (in the application directory).</p>"));
        else
            QMessageBox::warning(0, "mathGUIde",
                    tr("<h3>Python 3.1 was not found.</h3>"
                    "<p>Please install <a href=\"http://www.python.org/download/releases/3.1/\">Python 3.1</a>.</p>"
                    "<p><b>Important</b>: You need a Python <b>3.1</b>.x version in order to run mathGUide.</p>"
                    "<hr/><p>If you have an other Python version (e.g. Python 2.6) installed, you may install Python 3.1 in addition.</p>"));
    }

    statusBar()->showMessage(tr("Press Ctrl+Space for a list of all global classes and functions"));
}

void MainWindow::createConnections() {
    connect(_methodsPopup, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(insertListBoxItem(QListWidgetItem*)));
    connect(_methodsPopup, SIGNAL(escPressed()),             this,        SLOT(hideTipWindows()));
    connect(_methodsPopup, SIGNAL(backspacePressed()),       _inputWnd,   SLOT(backspacePressed()));
    connect(_methodsPopup, SIGNAL(textTyped(QString)),       _inputWnd,   SLOT(textTyped(QString)));
    connect(_methodsPopup, SIGNAL(itemSelectionChanged()),   this,        SLOT(showMethodToolTip()));

    connect(_parametersPopup, SIGNAL(escPressed()),          this,        SLOT(hideTipWindows()));
    connect(_parametersPopup, SIGNAL(backspacePressed()),    _inputWnd,   SLOT(backspacePressed()));
    connect(_parametersPopup, SIGNAL(textTyped(QString)),    _inputWnd,   SLOT(textTyped(QString)));

    connect(_inputWnd, SIGNAL(statusMessage(QString)),       this,        SLOT(showStatusMessage(QString)));
    connect(_inputWnd, SIGNAL(inputChanged(QString, QRect)), this,        SLOT(contextHelp(QString, QRect)));
    connect(_inputWnd, SIGNAL(escPressed()),                 this,        SLOT(hideTipWindows()));

    connect(_recordWnd, SIGNAL(cursorMoved(bool, bool)),     this,        SLOT(recordCursorMoved(bool, bool)));

    foreach (GraphWidget* w, _graphWidgets) {
        connect(w, SIGNAL(graphDefined(QString)),            _inputWnd, SLOT(textTyped(QString)));
        connect(w, SIGNAL(helpRequested()),                  this, SLOT(helpGraph()));
    }

    connect(_guideView->page(), SIGNAL(linkClicked(const QUrl&)), this, SLOT(handleGuideLink(const QUrl&)));
    connect(_guideView->page(), SIGNAL(linkHovered(const QString, const QString, const QString)),
            this, SLOT(handleGuideHover(const QString, const QString, const QString)));
    connect(_guideView->page(), SIGNAL(loadStarted()),           this, SLOT(webLoadStarted()));
    connect(_guideView->page(), SIGNAL(loadFinished(bool)),      this, SLOT(updateNavigation(bool)));
    connect(_guideView->page(), SIGNAL(loadProgress(int)),       this, SLOT(webLoadProgress(int)));
    connect(_guideDock,         SIGNAL(visibilityChanged(bool)), this, SLOT(updateGuideVisibility(bool)));

    connect(_guideOpenAction,    SIGNAL(triggered()),            this, SLOT(openGuide()));
    connect(_guideCopyAction,    SIGNAL(triggered()), _guideView, SLOT(copy()));
    connect(_guideZoomInAction,  SIGNAL(triggered()), _guideView, SLOT(zoomIn()));
    connect(_guideZoomOutAction, SIGNAL(triggered()), _guideView, SLOT(zoomOut()));
    connect(_guideZoomResetAction, SIGNAL(triggered()), _guideView, SLOT(zoomReset()));

    connect(_saveHtmlAction,     SIGNAL(triggered()), this, SLOT(saveHtml()));
    connect(_quitAction,         SIGNAL(triggered()), this, SLOT(quit()));
    connect(_calcAction,         SIGNAL(triggered()), this, SLOT(calc()));
    connect(_upAction,           SIGNAL(triggered()), this, SLOT(up()));
    connect(_downAction,         SIGNAL(triggered()), this, SLOT(down()));
    connect(_toggleGuideVisibleAction, SIGNAL(triggered()), this, SLOT(toggleGuideVisible()));
    connect(_toggleGuideDockingAction, SIGNAL(triggered()), this, SLOT(toggleGuideDocking()));
    connect(_homeAction,         SIGNAL(triggered()), this, SLOT(home()));
    connect(_clearRecordsAction, SIGNAL(triggered()), _recordWnd, SLOT(clear()));
    connect(_guideForwardAction, SIGNAL(triggered()), this, SLOT(guideForward()));
    connect(_guideBackAction,    SIGNAL(triggered()), this, SLOT(guideBack()));
    connect(_searchAction,       SIGNAL(triggered()), this, SLOT(search()));
    connect(_searchNextAction,   SIGNAL(triggered()), this, SLOT(searchNext()));
    connect(_sumAction,          SIGNAL(triggered()), this, SLOT(insertSum()));
    connect(_prodAction,         SIGNAL(triggered()), this, SLOT(insertProd()));
    connect(_matrixAction,       SIGNAL(triggered()), this, SLOT(insertMatrix()));
    connect(_listAction,         SIGNAL(triggered()), this, SLOT(insertList()));
    connect(_tableAction,        SIGNAL(triggered()), this, SLOT(insertTable()));
    connect(_valueTableAction,   SIGNAL(triggered()), this, SLOT(insertValueTable()));
    connect(_plotAction,         SIGNAL(triggered()), this, SLOT(insertPlot()));
    connect(_rationalAction,     SIGNAL(triggered()), this, SLOT(insertRational()));
    connect(_ifAction,           SIGNAL(triggered()), this, SLOT(insertIf()));
    connect(_forAction,          SIGNAL(triggered()), this, SLOT(insertFor()));
    connect(_optionsAction,      SIGNAL(triggered()), this, SLOT(selectOptions()));
    connect(_referenceAction,    SIGNAL(triggered()), this, SLOT(activateReference()));
    connect(_aboutAction,        SIGNAL(triggered()), this, SLOT(about()));
    connect(_escapeAction,       SIGNAL(triggered()), this, SLOT(hideTipWindows()));
}

MainWindow::~MainWindow() {
    _settings.setValue("/Font", _font.toString());
    _settings.setValue("/lang", userLanguageId);
}

/*static*/ const int MainWindow::_maxGraphTabs = 8;

void MainWindow::insertListBoxItem(QListWidgetItem* item) {
    QString s = item->text().simplified();
    s = s.replace(QRegExp("\\s*\\(.*\\)"), "");
    for (int i=0; i<_methodsPopup->nCharsEntered(); i++)
        _inputWnd->textCursor().deletePreviousChar();
    _inputWnd->textCursor().insertText(s);
    _methodsPopup->hideFrame();
    _inputWnd->setFocus();
}

void MainWindow::createWidgets() {
    //--- Splitter Window for QTabWidget and InputWindow ---
    _workWnd = new QSplitter(this); // working window
    _workWnd->setOrientation(Qt::Vertical);
    _workWnd->restoreGeometry(_settings.value("geometry/workWnd").toByteArray());
    setCentralWidget(_workWnd);

    _tabView = new QTabWidget(_workWnd);
    _tabView->setMinimumSize(100, 30);
    _tabView->restoreGeometry(_settings.value("geometry/tabView").toByteArray());

    _inputWnd = new InputWindow(_workWnd);
    _inputWnd->setMinimumSize(100, 30);
    _inputWnd->restoreGeometry(_settings.value("geometry/inputWnd").toByteArray());
    _inputWnd->setFont(_font);

    _workWnd->restoreState(_settings.value("geometry/splitter").toByteArray());

    _pythonHighlighter = new PythonHighlighter(_inputWnd->document());

    // 1st tab: RecordWindow
    _recordWnd = new RecordWindow;
    _recordWnd->setFont(_font);
    new PythonHighlighter(_recordWnd->document());

    // 2nd tab: PlotWindow
    _plotWnd = new PlotWindow;

    // 3rd tab: GraphWidget
    _graphTabView = new QTabWidget(_workWnd);
    _graphTabView->setTabPosition(QTabWidget::East);
    for (int i=0; i<_maxGraphTabs; i++) {
        GraphWidget* w = new GraphWidget;
        _graphWidgets.append(w);
        _graphTabView->addTab(w, QString("%1").arg(i+1));
    }

    // Add Windows to QTabWidget
    _tabView->addTab(_recordWnd, QIcon(":/img/log.png"), tr("Log"));
    _tabView->addTab(_plotWnd, QIcon(":/img/plot.png"), tr("Plot"));
    _tabView->addTab(_graphTabView, QIcon(":/img/graph/edge.png"), tr("Graph"));
    _tabView->setCurrentWidget(_recordWnd);

    _inputWnd->setFocus();

    _methodsPopup = new PopupList;
    _methodsPopup->setFixedSize(200,200);
    _parametersPopup = new PopupBrowser(this);
    _parametersPopup->setFixedSize(480,300);
}

void MainWindow::createGuide() {
    _guideDock = new QDockWidget(tr("Guide"), this);
    _guideView = new GuideView(_guideDock, &_settings);
    QString fileName = QString("%1/%2/guide/index.html").arg(_dataPath).arg(userLanguageId);

    _guideView->load(QUrl::fromLocalFile(fileName));
    _guideView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    _browserToolbar = new QToolBar("Browser Tool Bar", this);
    _browserToolbar->addAction(_guideOpenAction);
    _browserToolbar->addAction(_guideBackAction);
    _browserToolbar->addAction(_guideForwardAction);
    _browserToolbar->addAction(_homeAction);
    _browserToolbar->addSeparator();
    //_browserToolbar->addAction(_guideOpenAction);
    _browserToolbar->addAction(_guideCopyAction);
    _browserToolbar->addAction(_searchAction);
    _browserToolbar->addAction(_searchNextAction);
    _browserToolbar->addSeparator();
    _browserToolbar->addAction(_guideZoomOutAction);
    _browserToolbar->addAction(_guideZoomInAction);
    _browserToolbar->addAction(_guideZoomResetAction);
    _browserToolbar->addSeparator();
    _browserToolbar->addAction(_toggleGuideDockingAction);

    _browserProgressBar = new QProgressBar(this);
    _browserProgressBar->setMinimum(0);
    _browserProgressBar->setMaximum(100);
    _browserProgressBar->setFixedWidth(100);
    _browserProgressBar->setMaximumHeight(20);
    _browserProgressBar->setVisible(false);

    QWidget* guideFrame = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* hLayout = new QHBoxLayout;

    layout->setMargin(0);
    hLayout->setMargin(0);
    hLayout->addWidget(_browserToolbar);
    hLayout->addStretch();
    hLayout->addWidget(_browserProgressBar);
    layout->addLayout(hLayout);
    layout->addWidget(_guideView);
    guideFrame->setLayout(layout);
    _guideDock->setWidget(guideFrame);

    addDockWidget(Qt::RightDockWidgetArea, _guideDock);

    _guideDock->setFloating(_settings.value("/guide/float", false).toBool());
    _guideDock->restoreGeometry(_settings.value("geometry/guide").toByteArray());

    bool showGuide = _settings.value("/guide/visible", true).toBool();
    if (! showGuide) {
        _guideDock->setVisible(false);
    }
    updateGuideVisibility(showGuide);
    updateGuideDocked();
}

void MainWindow::showGuide() {
    if (_guideDock == 0) {
        QMessageBox::information(this, "Guide", "is destroyed!");
    }
    else if (! _guideDock->isVisible()) {
        _guideDock->setVisible(true);
    }
}

static void addShortcutToTooltip(QAction* action) {
    action->setToolTip(QString("%1 (%2)").arg(action->text().replace("&", ""))
                                         .arg(action->shortcut()
                                         .toString(QKeySequence::NativeText)));
}

void MainWindow::createActions() {
    _homeAction = new QAction(tr("&Home"), this);
    _homeAction->setShortcut(tr("Alt+Home"));
    addShortcutToTooltip(_homeAction);
    _homeAction->setIcon(QIcon(":/img/home.png"));
    _homeAction->setStatusTip(tr("Go to start page in the Guide window"));

    _clearRecordsAction = new QAction(tr("&Clear Log"), this);
    _clearRecordsAction->setStatusTip(tr("Clear all content of the log view"));

    _saveHtmlAction = new QAction(tr("Save..."), this);
    _saveHtmlAction->setShortcut(QKeySequence::Save);  //_saveHtmlAction->setShortcut(tr("Ctrl+S"));
    addShortcutToTooltip(_saveHtmlAction);
    _saveHtmlAction->setIcon(QIcon(":/img/save-html.png"));
    _saveHtmlAction->setStatusTip(tr("Save the complete session log as HTML"));

    _calcAction = new QAction(tr("&Calculate"), this);
    _calcAction->setIcon(QIcon(":/img/calc.png"));
    _calcAction->setShortcut(tr("Ctrl+Return"));
    addShortcutToTooltip(_calcAction);
    _calcAction->setStatusTip(tr("Interpret contents of entry window"));

    _guideForwardAction = new QAction(tr("&Next page"), this);
    _guideForwardAction->setShortcut(QKeySequence::Forward);  //_guideForwardAction->setShortcut(tr("Alt+Right"));
    addShortcutToTooltip(_guideForwardAction);
    _guideForwardAction->setIcon(QIcon(":/img/arrowright.png"));
    _guideForwardAction->setStatusTip(tr("Go one page forward in the Guide window"));
    _guideForwardAction->setEnabled(false);

    _guideBackAction = new QAction(tr("&Previous page"), this);
    _guideBackAction->setShortcut(QKeySequence::Back);  //_guideBackAction->setShortcut(tr("Alt+Left"));
    addShortcutToTooltip(_guideBackAction);
    _guideBackAction->setIcon(QIcon(":/img/arrowleft.png"));
    _guideBackAction->setStatusTip(tr("Go one page back in the Guide window"));
    _guideBackAction->setEnabled(false);

    _guideOpenAction = new QAction(tr("&Open"), this);
    _guideOpenAction->setShortcut(QKeySequence::Open);  //_guideOpenAction->setShortcut(tr("Ctrl+O"));
    addShortcutToTooltip(_guideOpenAction);
    _guideOpenAction->setIcon(QIcon(":/img/open.png"));

    _guideCopyAction = new QAction(tr("&Copy"), this);
    _guideCopyAction->setShortcut(QKeySequence::Copy);  //_guideCopyAction->setShortcut(tr("Ctrl+C"));
    //_guideCopyAction->setShortcut(tr("PgDown"));
    addShortcutToTooltip(_guideCopyAction);
    _guideCopyAction->setIcon(QIcon(":/img/copy.png"));

    _guideZoomInAction = new QAction(tr("Zoom &in"), this);
    _guideZoomInAction->setShortcut(QKeySequence::ZoomIn);  //_guideZoomInAction->setShortcut(tr("Ctrl++"));
    addShortcutToTooltip(_guideZoomInAction);
    _guideZoomInAction->setIcon(QIcon(":/img/zoom-in.png"));

    _guideZoomOutAction = new QAction(tr("Zoom &out"), this);
    _guideZoomOutAction->setShortcut(QKeySequence::ZoomOut);  //_guideZoomOutAction->setShortcut(tr("Ctrl+-"));
    addShortcutToTooltip(_guideZoomOutAction);
    _guideZoomOutAction->setIcon(QIcon(":/img/zoom-out.png"));

    _guideZoomResetAction = new QAction(tr("&Reset Zoom"), this);
    _guideZoomResetAction->setShortcut(tr("Ctrl+0"));
    addShortcutToTooltip(_guideZoomResetAction);
    _guideZoomResetAction->setIcon(QIcon(":/img/zoom-reset.png"));

    _searchAction = new QAction(tr("&Find"), this);
    _searchAction->setShortcut(QKeySequence::Find);  //_searchAction->setShortcut(tr("Ctrl+F"));
    addShortcutToTooltip(_searchAction);
    _searchAction->setIcon(QIcon(":/img/search.png"));
    _searchAction->setStatusTip(tr("Find text"));

    _searchNextAction = new QAction(tr("&Find next"), this);
    _searchNextAction->setShortcut(QKeySequence::FindNext);  //_searchNextAction->setShortcut(tr("F3"));
    addShortcutToTooltip(_searchNextAction);
    _searchNextAction->setIcon(QIcon(":/img/search-next.png"));
    _searchNextAction->setStatusTip(tr("Find next occurence"));

    _toggleGuideDockingAction = new QAction(this);
    _toggleGuideDockingAction->setIcon(QIcon(":/img/undock.png"));
    _toggleGuideDockingAction->setCheckable(true);

    _quitAction = new QAction(tr("E&xit"), this);
    _quitAction->setMenuRole(QAction::QuitRole);
#ifdef Q_WS_X11
    _quitAction->setShortcut(tr("Ctrl+Q"));
#endif
    addShortcutToTooltip(_quitAction);
    _quitAction->setStatusTip(tr("Quit mathGUIde"));

    _upAction = new QAction(tr("&Previous entry"), this);
    _upAction->setShortcut(tr("Alt+Up"));
    addShortcutToTooltip(_upAction);
    _upAction->setIcon(QIcon(":/img/up.png"));
    _upAction->setStatusTip(tr("Paste previous entry from log"));
    _upAction->setEnabled(false);

    _downAction = new QAction(tr("&Next entry"), this);
    _downAction->setShortcut(tr("Alt+Down"));
    addShortcutToTooltip(_downAction);
    _downAction->setIcon(QIcon(":/img/down.png"));
    _downAction->setStatusTip(tr("Paste next entry from log"));
    _downAction->setEnabled(false);

    _sumAction = new QAction(tr("&Sum..."), this);
    _sumAction->setShortcut(tr("Shift+Ctrl+S"));
    addShortcutToTooltip(_sumAction);
    _sumAction->setIcon(QIcon(":/img/sum.png"));
    _sumAction->setStatusTip(tr("Define sum"));

    _prodAction = new QAction(tr("&Product..."), this);
    _prodAction->setShortcut(tr("Shift+Ctrl+P"));
    addShortcutToTooltip(_prodAction);
    _prodAction->setIcon(QIcon(":/img/prod.png"));
    _prodAction->setStatusTip(tr("Define product"));

    _matrixAction = new QAction(tr("&Matrix..."), this);
    _matrixAction->setShortcut(tr("Shift+Ctrl+M"));
    addShortcutToTooltip(_matrixAction);
    _matrixAction->setIcon(QIcon(":/img/matrix.png"));
    _matrixAction->setStatusTip(tr("Define a matrix"));

    _listAction = new QAction(tr("&List..."), this);
    _listAction->setShortcut(tr("Shift+Ctrl+L"));
    addShortcutToTooltip(_listAction);
    _listAction->setIcon(QIcon(":/img/list.png"));
    _listAction->setStatusTip(tr("Define a list"));

    _tableAction = new QAction(tr("&Table..."), this);
    _tableAction->setShortcut(tr("Shift+Ctrl+T"));
    addShortcutToTooltip(_tableAction);
    _tableAction->setIcon(QIcon(":/img/table.png"));
    _tableAction->setStatusTip(tr("Define a calculated table"));

    _valueTableAction = new QAction(tr("Value &table..."), this);
    _valueTableAction->setShortcut(tr("Shift+Ctrl+V"));
    addShortcutToTooltip(_valueTableAction);
    _valueTableAction->setIcon(QIcon(":/img/valuetable.png"));
    _valueTableAction->setStatusTip(tr("Define a table of function values"));

    _plotAction = new QAction(tr("Function &Graph..."), this);
    _plotAction->setShortcut(tr("Shift+Ctrl+G"));
    addShortcutToTooltip(_plotAction);
    _plotAction->setIcon(QIcon(":/img/plot.png"));
    _plotAction->setStatusTip(tr("Define a graph of a function to be plotted"));

    _rationalAction = new QAction(tr("&Rational"), this);
    _rationalAction->setShortcut(tr("Shift+Ctrl+R"));
    addShortcutToTooltip(_rationalAction);
    _rationalAction ->setStatusTip(tr("Insert empty constructor for a Rational"));

    _ifAction = new QAction(tr("&if statement"), this);
    _ifAction->setShortcut(tr("Shift+Ctrl+I"));
    addShortcutToTooltip(_ifAction);
    _ifAction->setStatusTip(tr("Insert if/else statement with placeholders"));

    _forAction = new QAction(tr("&for statement"), this);
    _forAction->setShortcut(tr("Shift+Ctrl+F"));
    addShortcutToTooltip(_forAction);
    _forAction->setStatusTip(tr("Insert for statement with placeholders"));

    _optionsAction = new QAction(tr("&Settings..."), this);
    _optionsAction->setIcon(QIcon(":/img/settings.png"));
    _optionsAction->setStatusTip(tr("Choose mathGUIde Settings"));
    _optionsAction->setMenuRole(QAction::PreferencesRole);

    _aboutAction = new QAction(tr("&About mathGUIde..."), this);
    _aboutAction->setStatusTip(tr("Show copyright information about mathGUIde"));
    _aboutAction->setMenuRole(QAction::AboutRole);

    _toggleGuideVisibleAction = new QAction(this);
    _toggleGuideVisibleAction->setShortcut(tr("Ctrl+G"));
    addShortcutToTooltip(_toggleGuideVisibleAction);
    _toggleGuideVisibleAction->setIcon(QIcon(":/img/guide-small.png"));
    _toggleGuideVisibleAction->setStatusTip(tr("Show/Hide the Guide"));
    _toggleGuideVisibleAction->setCheckable(true);

    _referenceAction = new QAction(tr("&Reference"), this);
    _referenceAction->setShortcut(tr("Ctrl+R"));
    addShortcutToTooltip(_referenceAction);
    _referenceAction->setStatusTip(tr("Show reference"));

    _escapeAction = new QAction("", this);
    _escapeAction->setShortcut(tr("Escape"));
}


void MainWindow::createMenu() {
    // File menu
    QMenu* menu = menuBar()->addMenu(tr("&File"));
    menu->addAction(_saveHtmlAction);
    menu->addAction(_clearRecordsAction);
    menu->addSeparator();
    menu->addAction(_quitAction);  //menu->addAction(tr("&Quit"), this, SLOT(close()));

    // Edit menu
    menu = menuBar()->addMenu(tr("&Edit"));
    menu->addAction(_calcAction);
    menu->addAction(_upAction);
    menu->addAction(_downAction);
    menu->addSeparator();
    menu->addAction(_optionsAction);

    // Insert menu
    menu = menuBar()->addMenu(tr("&Insert"));
    menu->addAction(_sumAction);
    menu->addAction(_prodAction);
    menu->addAction(_matrixAction);
    menu->addAction(_listAction);
    menu->addAction(_tableAction);
    menu->addAction(_valueTableAction);
    menu->addAction(_plotAction);
    menu->addSeparator();
    menu->addAction(_rationalAction);
    menu->addAction(_ifAction);
    menu->addAction(_forAction);

    // Help menu
    menu = menuBar()->addMenu(tr("&Help"));
    menu->addAction(_toggleGuideVisibleAction);
    menu->addAction(_homeAction);
    menu->addAction(_referenceAction);
    menu->addSeparator();
    menu->addAction(_aboutAction);
}

void MainWindow::createToolbar() {
    _toolbar = new QToolBar("Tool Bar", this);
#ifdef Q_WS_MAC
    _toolbar->setIconSize(QSize(24,24));
#endif
    addToolBar(_toolbar);
    _toolbar->addAction(_upAction);
    _toolbar->addAction(_calcAction);
    _toolbar->addAction(_downAction);
    _toolbar->addAction(_optionsAction);
    _toolbar->addSeparator();
    _toolbar->addAction(_sumAction);
    _toolbar->addAction(_prodAction);
    _toolbar->addAction(_matrixAction);
    _toolbar->addAction(_listAction);
    _toolbar->addAction(_tableAction);
    _toolbar->addAction(_valueTableAction);
    _toolbar->addAction(_plotAction);
    _toolbar->addSeparator();

    _toolbar->addAction(_toggleGuideVisibleAction);
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About mathGUIde"), tr(
        "<h1><b><font color=\"#376092\">math</font><font color=\"#558ED5\">GUIde</font></b></h1>"
        "<p><a href=\"http://www.mathGUI.de\">www.mathGUI.de</a></p>"
        "<h3>Version 2.1</h3>"
        "<p>Author and Copyright &copy; 2006-2011 Hartmut Ring (University Siegen)<br/>"
        "<a href=\"http://www.mathematik.uni-siegen.de/ring/\">http://www.mathematik.uni-siegen.de/ring/</a></p>"
        "<p>This program uses"
        "<ul><li><a href=\"http://qt.nokia.com/\">Qt 4.7</a> "
        "(Copyright &copy; 2010 Nokia Corporation)</li>"
        "<li><a href=\"http://www.python.org/download/releases/3.1.2/\">Python 3.1</a> "
        "(Copyright &copy; 2001-2010 Python Software Foundation)</li>"
        "<li><a href=\"http://www.math.union.edu/~dpvc/jsMath/\">jsMath</a> "
        "(Copyright &copy; 2004-2010 by Davide P. Cervone)</li>"
        "</ul></p>"
        "<p>mathGUIde is free software; you can redistribute it and/or modify it under the terms "
        "of the GNU General Public License as published by the Free Software Foundation; either "
        "version 3 of the License, or (at your option) any later version.</p>"
        "<p>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; "
        "without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "
        "See the GNU General Public License for more details.</p>"
        "<p>You should have received a copy of the GNU General Public License along with this program; "
        "if not, see <a href=\"http://www.gnu.org/licenses/\">www.gnu.org/licenses/</a>.</p>")
    );
}

void MainWindow::quit() {
    qApp->quit();
}

void MainWindow::closeEvent(QCloseEvent* ev) {
    if (_saveSettings) {
        _settings.setValue("/geometry/mainWnd", saveGeometry());
        _settings.setValue("/geometry/workWnd", _workWnd->saveGeometry());
        _settings.setValue("geometry/splitter", _workWnd->saveState());
        _settings.setValue("/geometry/tabView", _tabView->saveGeometry());
        _settings.setValue("/geometry/inputWnd", _inputWnd->saveGeometry());
        _settings.setValue("/geometry/guide", _guideDock->saveGeometry());
        _settings.setValue("/guide/float", _guideDock->isFloating());
        _settings.setValue("/guide/visible", _guideDock->isVisible());
    }
    ev->accept(); // TODO ask for saving
}

void MainWindow::openGuide() {
    QString dir = _settings.value("/dirs/guides", QDir::homePath()).toString();
    QString fileName = QFileDialog::getOpenFileName(
            this, tr("Open presentation"), dir, "*.html");
    if (!fileName.isNull()) {
        _guideView->load(QUrl::fromLocalFile(fileName));
        _settings.setValue("/dirs/guides", QFileInfo(fileName).absoluteDir().absolutePath());
    }
}

void MainWindow::saveHtml() {
    QString dir = _settings.value("/dirs/html", QDir::homePath()).toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save the complete session log as HTML"), dir, "*.html");
    if (!fileName.isEmpty()) {
        _recordWnd->saveHtml(fileName);
        _settings.setValue("/dirs/html", QFileInfo(fileName).absoluteDir().absolutePath());
    }
}

void MainWindow::showMethodToolTip() {
    //_methodsPopup->setToolTip(QString("<b>Baustelle:</b><br/>Hier soll die Detailinfo zu <code>%1</code> erscheinen!")
    //                          .arg(_methodsPopup->currentItem()->text()));
}

void MainWindow::showStatusMessage(QString msg) {
    statusBar()->showMessage(msg);
}

void MainWindow::hideTipWindows() {
    _parametersPopup->hideFrame();
    _methodsPopup->hideFrame();
}

void MainWindow::contextHelp(QString s, QRect cursorRect) {
    if (_methodsPopup->isVisible())
        // method is called due to typing from the listbox
        return;
    _methodsPopup->clear();
    int p = (int)s.length()-1;
    if (s.length() > 0) {
        if (s == Constants::findGlobals || s[p] == '.' || s[p] == '(') {
            QString src = s;
            s = _python.getContextInfo(s).trimmed();
            if (s[0] == 'L') {
                // methods list
                QStringList list = s.mid(1).split(QRegExp("\\s*,\\s*"));
                foreach (QString s, list) {
                    QChar c = s[s.length()-1];
                    s = s.left(s.length()-1);
                    if (c == 'C') _methodsPopup->addItem(new QListWidgetItem(_classIcon, s));
                    else if (c == 'F') _methodsPopup->addItem(new QListWidgetItem(_functionIcon, s));
                    else if (c == 'E') _methodsPopup->addItem(new QListWidgetItem(_elementIcon, s));
                }
                int n = _methodsPopup->count();
                if (n > 0) {
                    if (n > 12) n = 12;
                    int lineHeight = _methodsPopup->visualItemRect(_methodsPopup->item(0)).height();
                    _methodsPopup->setBaseSize(200, n * lineHeight);
                    _parametersPopup->hideFrame();
                    _methodsPopup->showAtCursor(cursorRect, _inputWnd);
                    _methodsPopup->setCurrentRow(0);
                    _methodsPopup->setFocus();
                }
            }
            else if (s[0] == 'S') {
                // function documentation
                int pos=0;
                pos = s.indexOf("----de----", 0);
                if (pos >=0) {
                    if (userLanguageId == "de")
                        s.remove(0, pos+10);
                    else
                        s.remove(pos, s.length()-pos);
                }

                QString method, cls;
                _python.getMethod(src, cls, method);
                QString methodSource = _python.source(method, cls);

                QString baseUrl = QString("%1/%2/guide/index.html").arg(_dataPath).arg(userLanguageId);
                _parametersPopup->settings()->setUserStyleSheetUrl(
                        QString("%1/%2/styles/popup.css").arg(_dataPath).arg(userLanguageId));
                if (cls != "global")
                    methodSource = QString("class %1 ...<br>%2").arg(cls).arg(methodSource);
                _parametersPopup->setHtml(QString("<html><body>%1"
                                                  "<hr/><pre class=\"src\"># <b>%2</b><br/>%3</pre>"
                                                  "</body></html>"
                                                  ).arg(s.mid(1)).arg(tr("Implementation:")).arg(methodSource));
                _parametersPopup->showAtCursor(cursorRect, _inputWnd);
                _parametersPopup->setFocus();
            }
        }
        else if (s[p] == ')') {
            _parametersPopup->hideFrame();
            _inputWnd->setFocus();
        }
    }
}

void MainWindow::helpGraph() {
    showGuide();
    QString fileName = QString("%1/%2/guide/graphs.html").arg(_dataPath).arg(userLanguageId);
    _guideView->load(QUrl::fromLocalFile(fileName));
}

void MainWindow::toggleGuideVisible() {
    _guideDock->setVisible(! _guideDock->isVisible());
}

void MainWindow::home() {
    showGuide();
    QString fileName = QString("%1/%2/guide/index.html").arg(_dataPath).arg(userLanguageId);
    _guideView->load(QUrl::fromLocalFile(fileName));
}

void MainWindow::activateReference() {
    showGuide();
    QString fileName = QString("%1/%2/doc/mathguide.html").arg(_dataPath).arg(userLanguageId);
    _guideView->load(QUrl::fromLocalFile(fileName));
}

void MainWindow::guideForward() {
    showGuide();
    QWebHistory* history = _guideView->page()->history();
    if (history->canGoForward()) {
        bool local = history->forwardItem().originalUrl().toString().contains("#");
        history->forward();
        if (local) {
            _guideBackAction->setEnabled(history->canGoBack());
            _guideForwardAction->setEnabled(history->canGoForward());
        }
    }
}

void MainWindow::guideBack() {
    showGuide();
    QWebHistory* history = _guideView->page()->history();
    if (history->canGoBack()) {
        bool local = history->currentItem().originalUrl().toString().contains("#");
        history->back();
        if (local) {
            _guideBackAction->setEnabled(history->canGoBack());
            _guideForwardAction->setEnabled(history->canGoForward());
        }
    }
}

void MainWindow::search() {
    bool ok;
    QString text = QInputDialog::getText(_guideView, tr("Search"),
                                         tr("Search for:"), QLineEdit::Normal,
                                         //"class",
                                         _settings.value("/find/text", "").toString(),
                                         &ok);
    if (ok && !text.isEmpty()) {
        _guideView->findText(text, QWebPage::FindCaseSensitively);
        _settings.setValue("/find/text", text);
    }
}

void MainWindow::searchNext() {
    _guideView->findText(_settings.value("/find/text", "").toString(),
                         QWebPage::FindCaseSensitively | QWebPage::FindWrapsAroundDocument);
}

void MainWindow::toggleGuideDocking() {
    _guideDock->setFloating(! _guideDock->isFloating());
    bool docked = !_guideDock->isFloating();
    _toggleGuideDockingAction->setChecked(!docked);
    _toggleGuideDockingAction->setText(docked ? tr("Float Guide") : tr("Dock Guide"));
    _toggleGuideDockingAction->setToolTip(docked ? tr("Show Guide as separate floating window") : tr("Dock Guide inside the main window"));
    //_toggleGuideDockingAction->setStatusTip(tr("Show Guide as floating window"));

}

void MainWindow::up() {
    _recordWnd->moveCursor(false);
    _inputWnd->setText(_recordWnd->curInput());
}

void MainWindow::down() {
    _recordWnd->moveCursor(true);
    _inputWnd->setText(_recordWnd->curInput());
}

void MainWindow::insertSum() {
    SumDialog dlg(this);
    dlg.editVar->setText(_settings.value("/sum/var", "i").toString());
    dlg.editFrom->setText(_settings.value("/sum/from", "1").toString());
    dlg.editTo->setText(_settings.value("/sum/to", "100").toString());
    dlg.editTerm->setText(_settings.value("/sum/term", "i^2").toString());
    dlg.checkBoxOverwrite->setChecked(_settings.value("/sum/overwrite", true).toBool());

    if (dlg.exec()) {
        _settings.setValue("/sum/var", dlg.editVar->text());
        _settings.setValue("/sum/from", dlg.editFrom->text());
        _settings.setValue("/sum/to", dlg.editTo->text());
        _settings.setValue("/sum/term", dlg.editTerm->text());
        _settings.setValue("/sum/overwrite", dlg.checkBoxOverwrite->isChecked());

        QString s = QString("sum([%1 for %2 in fromTo(%3, %4, 1)])")
            .arg(dlg.editTerm->text())
            .arg(dlg.editVar->text())
            .arg(dlg.editFrom->text())
            .arg(dlg.editTo->text());
        if (dlg.checkBoxOverwrite->isChecked())
            _inputWnd->clear();
        _inputWnd->textCursor().insertText(s);
    }
}

void MainWindow::insertProd() {
    ProdDialog dlg(this);
    dlg.editVar->setText(_settings.value("/prod/var", "i").toString());
    dlg.editFrom->setText(_settings.value("/prod/from", "1").toString());
    dlg.editTo->setText(_settings.value("/prod/to", "100").toString());
    dlg.editTerm->setText(_settings.value("/prod/term", "i^2").toString());
    dlg.checkBoxOverwrite->setChecked(_settings.value("/prod/overwrite", true).toBool());

    if (dlg.exec()) {
        _settings.setValue("/prod/var", dlg.editVar->text());
        _settings.setValue("/prod/from", dlg.editFrom->text());
        _settings.setValue("/prod/to", dlg.editTo->text());
        _settings.setValue("/prod/term", dlg.editTerm->text());
        _settings.setValue("/prod/overwrite", dlg.checkBoxOverwrite->isChecked());

        QString s = QString("product([%1 for %2 in fromTo(%3, %4, 1)])")
            .arg(dlg.editTerm->text())
            .arg(dlg.editVar->text())
            .arg(dlg.editFrom->text())
            .arg(dlg.editTo->text());
        if (dlg.checkBoxOverwrite->isChecked())
            _inputWnd->clear();
        _inputWnd->textCursor().insertText(s);
    }
}

void MainWindow::insertList() {
    ListDialog dlg(this);
    dlg.lineEdit1->setText(_settings.value("/list/base", "fromTo(1,10)").toString());
    dlg.lineEdit2->setText(_settings.value("/list/var", "i").toString());
    dlg.lineEdit3->setText(_settings.value("/list/term", "10*i").toString());
    dlg.lineEdit4->setText(_settings.value("/list/cond", "i%3 != 0").toString());
    dlg.checkBoxOverwrite->setChecked(_settings.value("/list/overwrite", true).toBool());

    if (dlg.exec()) {
        _settings.setValue("/list/base", dlg.lineEdit1->text());
        _settings.setValue("/list/var",  dlg.lineEdit2->text());
        _settings.setValue("/list/term", dlg.lineEdit3->text());
        _settings.setValue("/list/cond", dlg.lineEdit4->text());
        _settings.setValue("/list/overwrite", dlg.checkBoxOverwrite->isChecked());

        QString s;
        QString cond = dlg.lineEdit4->text();
        if (! cond.isEmpty())
            cond = " if " + cond;
        s = QString("[%1 for %2 in %3%4]") .arg(dlg.lineEdit3->text())
            .arg(dlg.lineEdit2->text())
            .arg(dlg.lineEdit1->text())
            .arg(cond);
        if (dlg.checkBoxOverwrite->isChecked())
            _inputWnd->clear();
        _inputWnd->textCursor().insertText(s);
    }
}

void MainWindow::insertMatrix() {
    MatrixDialog dlg(this);
    dlg.editN->setText(_settings.value("/matrix/n", "3").toString());
    dlg.editM->setText(_settings.value("/matrix/m", "3").toString());
    dlg.editI->setText(_settings.value("/matrix/i", "i").toString());
    dlg.editK->setText(_settings.value("/matrix/k", "k").toString());
    dlg.editAik->setText(_settings.value("/matrix/term", "10*i+k").toString());
    dlg.checkBoxOverwrite->setChecked(_settings.value("/matrix/overwrite", true).toBool());
    bool formula = _settings.value("/matrix/formula", false).toBool();
    bool from1 = _settings.value("/matrix/from1", false).toBool();
    dlg.checkIndex1->setChecked(from1);
    if (formula)
        dlg.radioButton2->setChecked(true);
    if (dlg.exec()) {
        from1 = dlg.checkIndex1->isChecked();
        formula = dlg.radioButton2->isChecked();
        _settings.setValue("/matrix/n", dlg.editN->text());
        _settings.setValue("/matrix/m", dlg.editM->text());
        _settings.setValue("/matrix/formula", formula);
        _settings.setValue("/matrix/i", dlg.editI->text());
        _settings.setValue("/matrix/k", dlg.editK->text());
        _settings.setValue("/matrix/term", dlg.editAik->text());
        _settings.setValue("/matrix/from1", from1);
        _settings.setValue("/matrix/overwrite", dlg.checkBoxOverwrite->isChecked());

        QString s;
        if (! formula) {
            int m = dlg.editM->text().toInt();
            if (m < 1) m = 1; if (m > 100) m = 100;
            int n = dlg.editN->text().toInt();
            if (n < 1) n = 1; if (n > 100) n = 100;
            QString sRow = QString("[");
            for (int i=1; i<m; i++)
                sRow += QString("%1,").arg(Constants::placeholderChar);
            sRow += QString("%1]").arg(Constants::placeholderChar);
            s = QString("Matrix([");
            for (int i=0; i<n; i++) {
                s += sRow;
                if (i < n-1)
                    s += QString(",");
            }
            s += QString("])");
            statusBar()->showMessage(_placeholderInfo);
        }
        else {
            s = QString(from1 ? "Matrix([[%1 for %2 in fromTo(1, %3)] for %4 in fromTo(1, %5)])"
                              : "Matrix([[%1 for %2 in range(%3)] for %4 in range(%5)])")
                .arg(dlg.editAik->text())
                .arg(dlg.editK->text())
                .arg(dlg.editM->text())
                .arg(dlg.editI->text())
                .arg(dlg.editN->text());
        }
        if (dlg.checkBoxOverwrite->isChecked())
            _inputWnd->clear();

        _inputWnd->textCursor().insertText(s);
        _inputWnd->moveLeft(s.length()-9); // behind "Matrix([["
    }
}

void MainWindow::calcString(QString s) {
    QString output, error;
    _python.runString(s, output, error);
    emit calcStringResult(output, error);
}

void MainWindow::insertTable() {
    int nRows   = _settings.value("/table/nRows", "5").toInt();
    int nCols   = _settings.value("/table/nCols", "5").toInt();
    QString cellFn =  _settings.value("/table/cellFn", "i*k % 5").toString();
    bool showRowHead = _settings.value("/table/showRowHead", "true").toBool();
    bool showColHead = _settings.value("/table/showColHead", "true").toBool();
    QString rowHeadFn =  _settings.value("/table/rowHeadFn", "i").toString();
    QString colHeadFn =  _settings.value("/table/colHeadFn", "k").toString();
    int headAlign = _settings.value("/table/headAlign", "0").toInt();
    int cellAlign = _settings.value("/table/cellAlign", "2").toInt();
    QString title = _settings.value("/table/title", tr("Multiplication mod 5")).toString();
    QString corner = _settings.value("/table/corner", "*").toString();

    TableDialog dlg(this);
    dlg.spinBoxHeight->setValue(nRows);
    dlg.spinBoxWidth->setValue(nCols);
    dlg.editCellFn->setText(cellFn);
    dlg.checkBoxRowHeader->setChecked(showRowHead);
    dlg.checkBoxColHeader->setChecked(showColHead);
    dlg.editRowHeadFn->setText(rowHeadFn);
    dlg.editColHeadFn->setText(colHeadFn);
    dlg.comboAlignHead->setCurrentIndex(headAlign);
    dlg.comboAlignCell->setCurrentIndex(cellAlign);
    dlg.editTitle->setText(title);
    dlg.editCorner->setText(corner);
    dlg.checkBoxOverwrite->setChecked(_settings.value("/table/overwrite", true).toBool());

    connect(&dlg, SIGNAL(pythonCalc(QString)), this, SLOT(calcString(QString)));
    connect(this, SIGNAL(calcStringResult(QString, QString)), &dlg, SLOT(pythonResult(QString, QString)));
    dlg.previewChanged();
    dlg.updatePreview();

    if (dlg.exec()) {
        if (dlg.checkBoxOverwrite->isChecked())
            _inputWnd->clear();
        _inputWnd->textCursor().insertText(dlg.getPythonCall());
        _settings.setValue("/table/nRows", dlg.spinBoxHeight->value());
        _settings.setValue("/table/nCols", dlg.spinBoxWidth->value());
        _settings.setValue("/table/cellFn", dlg.editCellFn->text());
        _settings.setValue("/table/showRowHead", dlg.checkBoxRowHeader->isChecked());
        _settings.setValue("/table/showColHead", dlg.checkBoxColHeader->isChecked());
        _settings.setValue("/table/rowHeadFn", dlg.editRowHeadFn->text());
        _settings.setValue("/table/colHeadFn", dlg.editColHeadFn->text());
        _settings.setValue("/table/headAlign", dlg.comboAlignHead->currentIndex());
        _settings.setValue("/table/cellAlign", dlg.comboAlignCell->currentIndex());
        _settings.setValue("/table/title", dlg.editTitle->text());
        _settings.setValue("/table/corner", dlg.editCorner->text());
        _settings.setValue("/table/overwrite", dlg.checkBoxOverwrite->isChecked());
    }
}

void MainWindow::insertValueTable() {
    QString var      = _settings.value("/table/var", "x").toString();
    QString sFrom    = _settings.value("/table/from", "1").toString();
    QString sTo      = _settings.value("/table/to", "10").toString();
    QString sIncr    = _settings.value("/table/incr", "1").toString();
    QString sFn1     = _settings.value("/table/fn1", "x^2").toString();
    QString sTitle1  = _settings.value("/table/title1", "x<sup>2</sup>").toString();
    QString sFn2     = _settings.value("/table/fn2", "2^x").toString();
    QString sTitle2  = _settings.value("/table/title2", "2<sup>x</sup>").toString();
    QString sFn3     = _settings.value("/table/fn3", "x^x").toString();
    QString sTitle3  = _settings.value("/table/title3", "x<sup>x</sup>").toString();
    static int nFunctions = _settings.value("/table/nFn", "1").toInt();
    bool vert     = _settings.value("/table/vert", "true").toBool();

    ValueTableDialogX dlg(this);
    dlg.editVariable->setText(var);
    dlg.editFrom->setText(sFrom);
    dlg.editTo->setText(sTo);
    dlg.editInc->setText(sIncr);
    dlg.editFn1Value->setText(sFn1);    dlg.editFn1Title->setText(sTitle1);
    dlg.editFn2Value->setText(sFn2);    dlg.editFn2Title->setText(sTitle2);
    dlg.editFn3Value->setText(sFn3);    dlg.editFn3Title->setText(sTitle3);

    dlg.checkFn2->setChecked(nFunctions > 1);
    dlg.checkFn3->setChecked(nFunctions > 2);

    dlg.checkBoxVertical->setChecked(vert);
    dlg.checkBoxOverwrite->setChecked(_settings.value("/table/overwrite", true).toBool());

    connect(&dlg, SIGNAL(pythonCalc(QString)), this, SLOT(calcString(QString)));
    connect(this, SIGNAL(calcStringResult(QString, QString)), &dlg, SLOT(pythonResult(QString, QString)));

    dlg.previewChanged();
    dlg.updatePreview();

    if (dlg.exec()) {
        if (dlg.checkBoxOverwrite->isChecked())
            _inputWnd->clear();
        _inputWnd->textCursor().insertText(dlg.getPythonCall());

        nFunctions = 1;
        if (dlg.checkFn2->isChecked() && ! dlg.editFn2Value->text().isEmpty()) {
            nFunctions = 2;
            if (dlg.checkFn3->isChecked() && ! dlg.editFn3Value->text().isEmpty())
                nFunctions = 3;
        }
        _settings.setValue("/table/nFn", nFunctions);

        _settings.setValue("/table/var", dlg.editVariable->text());
        _settings.setValue("/table/from", dlg.editFrom->text());
        _settings.setValue("/table/to", dlg.editTo->text());
        _settings.setValue("/table/incr", dlg.editInc->text());
        _settings.setValue("/table/fn1", dlg.editFn1Value->text());   _settings.setValue("/table/title1", dlg.editFn1Title->text());
        _settings.setValue("/table/fn2", dlg.editFn2Value->text());   _settings.setValue("/table/title2", dlg.editFn2Title->text());
        _settings.setValue("/table/fn3", dlg.editFn3Value->text());   _settings.setValue("/table/title3", dlg.editFn3Title->text());
        _settings.setValue("/table/nFn", nFunctions);

        _settings.setValue("/table/vert", dlg.checkBoxVertical->isChecked());
        _settings.setValue("/table/overwrite", dlg.checkBoxOverwrite->isChecked());

    }

    /***
    QString sX0   = _settings.value("/table/start", "1").toString();
    QString sN    = _settings.value("/table/nSteps", "10").toString();
    QString sStep = _settings.value("/table/incr", "1").toString();
    QString sFn1  = _settings.value("/table/fn1", "x**2").toString();
    QString sFn2  = _settings.value("/table/fn2", "2**x").toString();
    QString sFn3  = _settings.value("/table/fn3", "x**x").toString();
    static int nFunctions = _settings.value("/table/nFn", "1").toInt();
    bool vert     = _settings.value("/table/vert", "true").toBool();

    ValueTableDialog dlg(this);
    dlg.editX0->setText(sX0);
    dlg.editN->setText(sN);
    dlg.editStep->setText(sStep);

    dlg.editFn1->setText(sFn1);
    dlg.editFn2->setText(sFn2);
    dlg.editFn3->setText(sFn3);
    dlg.checkFn2->setChecked(nFunctions > 1);
    dlg.checkFn3->setChecked(nFunctions > 2);

    dlg.checkVertical->setChecked(vert);
    dlg.checkBoxOverwrite->setChecked(_settings.value("/table/overwrite", true).toBool());

    if (dlg.exec()) {
        sX0  = dlg.editX0->text();
        sN   = dlg.editN->text();
        sStep= dlg.editStep->text();
        sFn1 = dlg.editFn1->text();
        sFn2 = dlg.editFn2->text();
        sFn3 = dlg.editFn3->text();
        vert = dlg.checkVertical->isChecked();

        nFunctions = 1;
        if (dlg.checkFn2->isChecked() && ! sFn2.isEmpty()) {
            nFunctions = 2;
            if (dlg.checkFn3->isChecked() && ! sFn3.isEmpty())
                nFunctions = 3;
        }

        QString pyVert = dlg.checkVertical->isChecked() ? "True" : "False";

        QString funcList = QString("[\"x\", \"%1\"@]").arg(sFn1);
        if (nFunctions > 1) {
            funcList.replace("@", QString(", \"%1\"@").arg(sFn2));
            if (nFunctions > 2)
                funcList.replace("@", QString(", \"%1\"").arg(sFn3));
        }
        funcList.replace("@", "");
        QString s = QString("printValueTable(%1, start=%2, nValues=%3, increment=%4, vertical=%5)")
                .arg(funcList).arg(sX0).arg(sN).arg(sStep).arg(pyVert);
        if (dlg.checkBoxOverwrite->isChecked())
            _inputWnd->clear();
        _inputWnd->textCursor().insertText(s);

        _settings.setValue("/table/start", sX0);
        _settings.setValue("/table/nSteps", sN);
        _settings.setValue("/table/incr", sStep);
        _settings.setValue("/table/fn1", sFn1);
        _settings.setValue("/table/fn2", sFn2);
        _settings.setValue("/table/fn3", sFn3);
        _settings.setValue("/table/nFn", nFunctions);
        _settings.setValue("/table/vert", vert);
        _settings.setValue("/table/overwrite", dlg.checkBoxOverwrite->isChecked());
    }
    ***/
}

void MainWindow::insertPlot() {
    static QString sX0 = _settings.value("/plot/x0", "0").toString();
    static QString sX1 = _settings.value("/plot/x1", "2*pi").toString();
    static QString sFn = _settings.value("/plot/fn1", "sin(x)").toString();
    static QString sFn2 = _settings.value("/plot/fn2", "cos(x)").toString();
    static QString sFn3 = _settings.value("/plot/fn3", "sin(2*x)").toString();
    static int nFunctions = _settings.value("/plot/nFn", "1").toInt();
    static int nStrokes = _settings.value("/plot/nSteps", "200").toInt();
    static QString options = _settings.value("/plot/options", "").toString();

    PlotDialog dlg(this);
    dlg.editX0->setText(sX0);
    dlg.editX1->setText(sX1);

    dlg.editFn1->setText(sFn);
    dlg.editFn2->setText(sFn2);
    dlg.editFn3->setText(sFn3);
    dlg.checkFn2->setChecked(nFunctions > 1);
    dlg.checkFn3->setChecked(nFunctions > 2);

    dlg.checkLines->setChecked(options.indexOf('n') < 0);
    dlg.checkDots->setChecked(options.indexOf('d') >= 0);
    dlg.checkBars->setChecked(options.indexOf('b') >= 0);
    dlg.spinBoxSteps->setValue(nStrokes);
    dlg.checkBoxOverwrite->setChecked(_settings.value("/plot/overwrite", true).toBool());

    if (dlg.exec()) {
        sX0 = dlg.editX0->text();
        sX1 = dlg.editX1->text();
        sFn = dlg.editFn1->text();
        options = "";
        if (! dlg.checkLines->isChecked()) options += "n";
        if (dlg.checkDots->isChecked())    options += "d";
        if (dlg.checkBars->isChecked())    options += "b";
        nStrokes = dlg.spinBoxSteps->value();

        nFunctions = 1;
        if (dlg.checkFn2->isChecked() && !  sFn2.isEmpty()) {
            nFunctions = 2;
            if (dlg.checkFn3->isChecked() && !  sFn3.isEmpty())
                nFunctions = 3;
        }

        QString funcList = QString("[lambda x:%1@]").arg(sFn);
        if (nFunctions > 1) {
            funcList.replace("@", QString(", lambda x:%1@").arg(dlg.editFn2->text()));
            if (nFunctions > 2)
                funcList.replace("@", QString(", lambda x:%1").arg(dlg.editFn3->text()));
        }
        funcList.replace("@", "");

        QString s = QString("plotFunctions(%1, %2, %3, nSteps=%4").arg(funcList).arg(sX0).arg(sX1).arg(nStrokes);

        if (options.indexOf('n') >= 0) s += ", lines=False";
        if (options.indexOf('b') >= 0) s += ", bars=True";
        if (options.indexOf('d') >= 0) s += ", dots=True";
        s += ")";
        if (dlg.checkBoxOverwrite->isChecked())
            _inputWnd->clear();
        _inputWnd->textCursor().insertText(s);

        _settings.setValue("/plot/x0", sX0);
        _settings.setValue("/plot/x1", sX1);
        _settings.setValue("/plot/fn1", sFn);
        _settings.setValue("/plot/fn2", sFn2);
        _settings.setValue("/plot/fn3", sFn3);
        _settings.setValue("/plot/nFn", nFunctions);
        _settings.setValue("/plot/nSteps", nStrokes);
        _settings.setValue("/plot/options", options);
        _settings.setValue("/plot/overwrite", dlg.checkBoxOverwrite->isChecked());
    }
}

void MainWindow::insertRational() {
    _inputWnd->insertStatement(QString("Rational(%1,%1)").arg(Constants::placeholderChar), 9);
    _inputWnd->moveLeft(4); // behind "Rational("
    statusBar()->showMessage(_placeholderInfo);
}

void MainWindow::insertIf() {
    _inputWnd->insertStatement(QString("if %1:\n%2    %1\n%2else:\n%2    %1").arg(Constants::placeholderChar).arg(Constants::indentChar));
    statusBar()->showMessage(_placeholderInfo);
}

void MainWindow::insertFor() {
    _inputWnd->insertStatement(QString("for %1 in %1:\n%2    %1").arg(Constants::placeholderChar).arg(Constants::indentChar));
    statusBar()->showMessage(_placeholderInfo);
}

void MainWindow::handleGuideLink(const QUrl& url) {
    QString link = url.toString();

    QString html = _guideView->page()->mainFrame()->toHtml();

    if (link.startsWith("mathguide:")) {
        link = link.mid(10);
        if (link.startsWith("listing")) {
            int pos = html.indexOf(link);
            int posBegin = html.indexOf("<pre", pos);
            posBegin = html.indexOf(">", posBegin) + 1;
            int posEnd = html.indexOf("</pre>", posBegin);
            QString listing = html.mid(posBegin, posEnd-posBegin);
            listing = listing.replace("&lt;", "<").replace("&gt;", ">").replace("&amp;", "&")
                             .replace(QRegExp("<span[^>]+>"), "")
                             .replace(QRegExp("</span>"), "");
            _inputWnd->clear();
            _inputWnd->insertStatement(listing);
        }
        else if (link.startsWith("calc")) {
            calc();
        }
        else if (link.startsWith("exe/")) {
            QString exeFile = QString("%1/%2/guide/apps/%3").arg(_dataPath).arg(userLanguageId).arg(link.mid(4));
            statusBar()->showMessage(exeFile);
            QProcess::execute(exeFile);
        }
    }
    else {
        if (link.contains("#")) {
            QWebHistory* history = _guideView->page()->history();
            _guideBackAction->setEnabled(history->canGoBack());
            _guideForwardAction->setEnabled(history->canGoForward());
            //statusBar()->clearMessage();
            _browserProgressBar->setVisible(false);
        }
        /***
        else {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            statusBar()->showMessage(tr("Loading..."));
            _browserProgressBar->setVisible(true);
        }***/
        _guideView->page()->mainFrame()->load(url);
    }
}

void MainWindow::handleGuideHover(const QString& link, const QString& /*title*/, const QString& /*textContent*/) {
    if (link.startsWith("mathguide:")) {
        if (link.mid(10).startsWith("listing"))
            statusBar()->showMessage(tr("insert the listing into the entry window"), 5000);
        else if (link.mid(10).startsWith("calc"))
            statusBar()->showMessage(tr("Interpret contents of entry window"), 5000);
    }
    else if (link.startsWith("file://")) {
        QString shortLink = link;
        statusBar()->showMessage(shortLink.replace(_dataPath, "[mathGUIde]"), 5000);
    }
    else {
        statusBar()->showMessage(link, 5000);
    }
}

void MainWindow::webLoadStarted() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    statusBar()->showMessage(tr("Loading..."));
    _browserProgressBar->setVisible(true);
}

void MainWindow::webLoadProgress(int percent) {
    _browserProgressBar->setValue(percent);
}

void MainWindow::updateGuideVisibility(bool visible) {
    _toggleGuideVisibleAction->setChecked(visible);
    _toggleGuideVisibleAction->setText(visible ? tr("Hide &Guide") : tr("Show &Guide"));
    _toggleGuideVisibleAction->setToolTip(visible ? tr("Hide the Guide") : tr("Show the Guide"));
    addShortcutToTooltip(_toggleGuideVisibleAction);
    _searchAction->setEnabled(visible);
    _searchNextAction->setEnabled(visible);
    _toggleGuideDockingAction->setEnabled(visible);
}

void MainWindow::updateGuideDocked() {
    bool docked = !_guideDock->isFloating();
    _toggleGuideDockingAction->setChecked(!docked);
    _toggleGuideDockingAction->setText(docked ? tr("Float Guide") : tr("Dock Guide"));
    _toggleGuideDockingAction->setToolTip(docked ? tr("Show Guide as separate floating window") : tr("Dock Guide inside the main window"));
}

void MainWindow::updateNavigation(bool) {
    QWebHistory* history = _guideView->page()->history();
    _guideBackAction->setEnabled(history->canGoBack());
    _guideForwardAction->setEnabled(history->canGoForward());
    QApplication::restoreOverrideCursor();
    statusBar()->clearMessage();
    _browserProgressBar->setVisible(false);
}

void MainWindow::setIconStyle(int style, int size) {
    QSize s(size, size);
    _toolbar->setIconSize(s);
    _toolbar->setToolButtonStyle((Qt::ToolButtonStyle)style);
    _browserToolbar->setToolButtonStyle((Qt::ToolButtonStyle)style);
    _browserToolbar->setIconSize(s);
    foreach (GraphWidget* w, _graphWidgets) {
        w->_toolBar->setToolButtonStyle((Qt::ToolButtonStyle)style);
        w->_toolBar->setIconSize(s);
    }
    _settings.setValue("/Toolbar/IconStyle", style);
    _settings.setValue("/Toolbar/IconSize", size);
}

void MainWindow::setLanguage(QString lang) {
    userLanguageId = lang;
    QString demoDir = QString("%1/%2/presentations").arg(_dataPath).arg(lang);
    _settings.setValue("/dirs/presentations", demoDir);
    QMessageBox::information(this,
        tr("mathGUIde"),
        tr("In order to activate the changed language, please close mathGUIde and restart it.")
    );
}

void MainWindow::calc() {
    _inputWnd->selectAll();
    QString pyInput = _inputWnd->toHtml();  // when copying from the record window
    QString pyCalc = _inputWnd->toPlainText();

    pyCalc.replace('\xA0', ' ');        // spaces are given as 0xA0
    pyCalc.replace(QRegExp("\\r"), ""); // "Carriage Return" (MS Windows only)
    pyCalc.replace("^^", "<bit-xor>");  // use ^^ instead of ^ for bitwise xor
    pyCalc.replace("^", "**");          // use ^ instead of ** for exponentiation
    pyCalc.replace("<bit-xor>", "^");
    //QMessageBox::information(this, "pyCalc", pyCalc.replace("\n", "N").replace("\r", "R").replace(" ", ";"));
    QString pyCalc0 = pyCalc;

    // combine multiple statements:
    pyCalc.replace("\n", "\n    ");
    pyCalc = "if True:\n    " + pyCalc + "\n    pass";

    QString output, error;
    statusBar()->showMessage(tr("Calculating..."));
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QTime t = QTime::currentTime();
    _python.runString(pyCalc, output, error);

    if (!error.isEmpty())
        error = _python.editError(error, QString(Constants::errorPrefixChar) + " ");

    QApplication::restoreOverrideCursor();
    int ms = t.msecsTo(QTime::currentTime());
    int cSec = (ms%1000+5)/10; // centiseconds
    QString s = QString(tr("Calculation time for last command: %1.%2%3 sec"))
        .arg(ms/1000).arg((cSec)/10).arg((cSec)%10);
    statusBar()->showMessage(s);

    // check for plotting command oder graph
    if (output.length() > 3 && output[0] == '\'' && output[1] == '@') {
        output = output.split('\'')[1];
        output = output.mid(1, output.length()-1);
        if (output[0] == 'G') {
            // graph
            QStringList list0 = output.split(':');
            int tab = list0[0].mid(1).toInt()-1;
            if (tab >= _maxGraphTabs) tab = _maxGraphTabs-1;
            if (tab < 0)
                tab = _graphTabView->currentIndex();
            _recordWnd->appendTexts(pyInput, "", error);
            QStringList list = list0[1].split('|');
            // example with edge weight: "1;2;3;4|1,2,36;1,3,99;1,4,15;2,4,69"
            // example with node locations: "1,-3.1,5.4;2,-3.1,5.4|1,2"
            QString nodeList = list[0];
            QString edgeList = list[1];

            QStringList nodes = nodeList.split(';');
            QStringList edges = edgeList.split(';');
            _graphWidgets[tab]->clear();
            foreach (QString s, nodes) {
                QStringList p = s.split(',');
                int index = p[0].toInt();
                if (p.size() >= 3)
                    _graphWidgets[tab]->addNode(QPointF(p[1].toDouble(),p[2].toDouble()), index);
                else
                    _graphWidgets[tab]->addNode(QPointF(50.0,50.0), index);
            }
            foreach (QString s, edges) {
                QStringList p = s.split(',');
                int source = p[0].toInt();
                int target = p[1].toInt();
                int weight = 0;
                bool marked = false;
                if (p.size() >= 3)
                    weight = p[2].toInt();
                if (p.size() >= 4)
                    marked = (p[3] != QChar('0'));
                _graphWidgets[tab]->addEdge(source, target, weight, marked);
            }

            _tabView->setCurrentWidget(_graphTabView);
            _graphTabView->setCurrentIndex(tab);
        }
        else if (output[0] == 'P') {
            // plot
            output = output.mid(1, output.length()-1);
            _recordWnd->appendTexts(pyInput, "", error);
            QStringList list = output.split('|');
            QString options = list[0];
            bool lines = options.indexOf('n') < 0;
            bool dots = options.indexOf('d') >= 0;
            bool bars = options.indexOf('b') >= 0;
            double x0 = list[1].toFloat();
            double x1 = list[2].toFloat();
            QString plotValues = list[3];
            QStringList valueList = plotValues.split(';');
            int nStrokes = valueList.count();

            _plotWnd->beginPlot(x0, x1, nStrokes, lines, dots, bars);
            for (int i=0; i<valueList.count(); i++) {
                QStringList valueStrings = valueList[i].split('&');
                QVector<double> values;
                values.resize(valueStrings.count());
                for (int j=0; j<valueStrings.count(); j++)
                    values[j] = valueStrings[j].toFloat();
                _plotWnd->addPlotValues(values);
            }

            QString markPoints = list[4];
            if (markPoints.indexOf(',') >= 0) {
                QStringList s = markPoints.split(';');
                for (int j=0; j<s.count(); j++) {
                    QStringList p = s[j].split(',');
                    if (p.size() == 2) {
                        _plotWnd->addMarker(QPointF(p[0].toDouble(), p[1].toDouble()));
                    }
                }
            }

            _plotWnd->endPlot();
            _tabView->setCurrentWidget(_plotWnd);
        }
    }
    else {
        // echo results of unindented assignments
        if (output.length() == 0 && error.length() == 0) {
            QStringList pyLines = pyCalc0.split("\n");
            if (pyLines.size() == 1) {
                QString s = pyLines[0];
                QString out, err;
                int pos = s.indexOf(QRegExp("\\w+\\s*=")); // unindented assignment
                if (pos == 0) {
                    pos = s.indexOf("=");
                    _python.runString(s.left(pos).simplified(), out, err);
                    output += out.simplified() + "\n";
                }
            }
        }
        //if (!error.isEmpty())
        //    error = QString(Constants::errorPrefixChar) + error;

        _recordWnd->appendTexts(pyInput, output, error);
        _tabView->setCurrentWidget(_recordWnd);
    }
    _inputWnd->setFocus();
}


void MainWindow::selectOptions() {
    OptionsDialog dlg(this);
    dlg.setCurrentFont(_font);
    dlg.setIconStyle(_toolbar->toolButtonStyle());
    dlg.setIconSize(_toolbar->iconSize().width());
    if (userLanguageId == "de")
        dlg.radioButtonGerman->setChecked(true);

    if (dlg.exec()) {
        if (dlg.reset)
            clearSettings();
        else {
            _font = dlg.currentFont;
            _inputWnd->setFont(_font);
            _recordWnd->setFont(_font);
            setIconStyle(dlg.iconStyle(), dlg.iconSize());
            _settings.setValue("/Toolbar/IconStyle", dlg.iconStyle());
            _settings.setValue("/Toolbar/IconSize", dlg.iconSize());

            bool de = dlg.radioButtonGerman->isChecked();
            if (de != (userLanguageId == "de")) {
                setLanguage(de ? "de" : "en");
            }
        }
    }
}

void MainWindow::selectFont() {
    bool ok;
    QFont f = QFontDialog::getFont(&ok, _font, this);
    if (ok) {
#ifdef Q_OS_MAC
        //QMessageBox::information(this, "Font", _font.toString());
#endif
        _font = f;
        _inputWnd->setFont(_font);
        _recordWnd->setFont(_font);
    }
}

void MainWindow::clearSettings() {
    if (QMessageBox::question(this, tr("Reset options"),
                              tr("Reset all Options to defaults and quit mathGUIde?"),
                              QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok) {
        _saveSettings = false;
        _settings.clear();
        _workWnd->restoreGeometry(_settings.value("geometry/workWnd").toByteArray());
        _tabView->restoreGeometry(_settings.value("geometry/tabView").toByteArray());
        _inputWnd->restoreGeometry(_settings.value("geometry/inputWnd").toByteArray());
        _workWnd->restoreState(_settings.value("geometry/splitter").toByteArray());
        _guideDock->restoreGeometry(_settings.value("geometry/guide").toByteArray());
        quit();
    }
}

void MainWindow::recordCursorMoved(bool top, bool btm) {
    _upAction->setEnabled(! top);
    _downAction->setEnabled(! btm);
}
