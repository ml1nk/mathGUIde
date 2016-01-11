#include <QtGui>
#include <QLabel>
#include <QMenu>
#include <QInputDialog>
#include <QGraphicsView>
#include <QVBoxLayout>

//#include <QMessageBox> // TODO !!!

#include "graphwidget.h"
#include "node.h"
#include "edge.h"
#include "graphscene.h"

GraphWidget::GraphWidget() {
    //setBackgroundRole(QPalette::Button);
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Button);

    createActions();
    createContextMenus();

    _scene = new GraphScene(_nodeMenu, _edgeMenu);
    //_scene->setSceneRect(this->rect());
    //_scene->setSceneRect(QRectF(0, 0, 1000, 1000));
    connect(_scene, SIGNAL(itemInserted(Node*)), this, SLOT(itemInserted(Node*)));
    connect(_scene, SIGNAL(itemSelected(QGraphicsItem*)), this, SLOT(itemSelected(QGraphicsItem*)));
    connect(_scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    createToolbar();

    _view = new QGraphicsView(_scene);
    _view->setRenderHint(QPainter::Antialiasing);
    _view->setRenderHint(QPainter::TextAntialiasing);

    _scene->setSceneRect(_view->rect());

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_toolBar);
    layout->addWidget(_view);
    setLayout(layout);
}


int GraphWidget::addNode(QPointF pos, int index/*=0*/) {
    return _scene->addNode(pos, index);
}

void GraphWidget::addEdge(int source, int target, int weight/*=0*/, bool marked/*=false*/) {
    _scene->addEdge(source, target, weight, marked);
}

void GraphWidget::clear() {
    _scene->clear();
}

void GraphWidget::deleteItem() {
    foreach (QGraphicsItem* item, _scene->selectedItems()) {
        if (item->type() == Node::Type) {
            qgraphicsitem_cast<Node*>(item)->removeAdjacentEdges();
        }
        _scene->removeItem(item);
    }
}

void GraphWidget::editEdgeWeight() {
    if (_scene->selectedItems().isEmpty())
        return;
    Edge* selectedEdge = qgraphicsitem_cast<Edge*>(_scene->selectedItems()[0]);
    bool ok;
    int w = QInputDialog::getInt(this, tr("Weight"),
            tr("Weight of the selected edge:"),
            selectedEdge->weight,  0, 2147483647, 1, &ok);
    if (ok) {
        selectedEdge->weight = w;
    }
}


void GraphWidget::setSelectMode() {
    _scene->setMouseMode(GraphScene::MoveNode);
    setCursor(Qt::PointingHandCursor);
}
void GraphWidget::setNodeMode() {
    _scene->setMouseMode(GraphScene::InsertNode);
    setCursor(Qt::ArrowCursor);
}
void GraphWidget::setEdgeMode() {
    _scene->setMouseMode(GraphScene::InsertEdge);
    setCursor(Qt::ArrowCursor);
}


void GraphWidget::exportGraph() {
    emit graphDefined(_scene->shortGraphDescr());
}

void GraphWidget::help() {
    emit helpRequested();
}

void GraphWidget::itemInserted(Node* /*item*/) {
    //_scene->setMouseMode(GraphScene::MouseMode(mouseActionTypeGroup->checkedId()));
}

void GraphWidget::itemSelected(QGraphicsItem* /*item*/) {
}

void GraphWidget::selectionChanged() {
    _deleteAction->setEnabled(_scene->selectedItems().count() > 0);
    _editEdgeWeightAction->setEnabled(_scene->selectedItems().count() == 1 && _scene->selectedItems()[0]->type() == Edge::Type);
}

static void addShortcutToTooltip(QAction* action) {
    action->setToolTip(QString("%1 (%2)").arg(action->text().replace("&", "")).arg(action->shortcut().toString(QKeySequence::NativeText)));
}

void GraphWidget::createActions() {
    _deleteAction = new QAction(tr("&Delete"), this);
    _deleteAction->setShortcut(tr("Delete"));
    addShortcutToTooltip(_deleteAction);
    _deleteAction->setIcon(QIcon(":/img/graph/delete.png"));
    _deleteAction->setStatusTip(tr("Delete item from graph"));
    _deleteAction->setEnabled(false);
    connect(_deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));

    _editEdgeWeightAction = new QAction(tr("&Weight..."), this);
    _editEdgeWeightAction->setShortcut(tr("Ctrl+W"));
    addShortcutToTooltip(_editEdgeWeightAction);
    _editEdgeWeightAction->setIcon(QIcon(":/img/graph/weight.png"));
    _editEdgeWeightAction->setStatusTip(tr("Edit weight of edge"));
    _editEdgeWeightAction->setEnabled(false);
    connect(_editEdgeWeightAction, SIGNAL(triggered()), this, SLOT(editEdgeWeight()));

    _selectAction = new QAction(tr("&Select"), this);
    _selectAction->setShortcut(tr("Ctrl+S"));
    addShortcutToTooltip(_selectAction);
    _selectAction->setIcon(QIcon(":/img/graph/hand.png"));
    _selectAction->setStatusTip(tr("Activate selection tool"));
    _selectAction->setCheckable(true);
    connect(_selectAction, SIGNAL(triggered()), this, SLOT(setSelectMode()));

    _insertNodeAction = new QAction(tr("Insert &Node"), this);
    _insertNodeAction->setShortcut(tr("Ctrl+N"));
    addShortcutToTooltip(_insertNodeAction);
    _insertNodeAction->setIcon(QIcon(":/img/graph/node.png"));
    _insertNodeAction->setStatusTip(tr("Activate node insertion tool"));
    _insertNodeAction->setCheckable(true);
    _insertNodeAction->setChecked(true);
    connect(_insertNodeAction, SIGNAL(triggered()), this, SLOT(setNodeMode()));

    _insertEdgeAction = new QAction(tr("Insert &Edge"), this);
    _insertEdgeAction->setShortcut(tr("Ctrl+E"));
    addShortcutToTooltip(_insertEdgeAction);
    _insertEdgeAction->setIcon(QIcon(":/img/graph/edge.png"));
    _insertEdgeAction->setStatusTip(tr("Activate edge insertion tool"));
    _insertEdgeAction->setCheckable(true);
    connect(_insertEdgeAction, SIGNAL(triggered()), this, SLOT(setEdgeMode()));

    _exportGraphAction = new QAction(tr("Export Definition"), this);
    _exportGraphAction->setShortcut(tr("Ctrl+D"));
    addShortcutToTooltip(_exportGraphAction);
    _exportGraphAction->setIcon(QIcon(":/img/graph/export.png"));
    _exportGraphAction->setStatusTip(tr("Paste a definition of the graph to the entry window"));
    connect(_exportGraphAction, SIGNAL(triggered()), this, SLOT(exportGraph()));

    _helpAction = new QAction(tr("Help"), this);
    _helpAction->setIcon(QIcon(":/img/help.png"));
    _helpAction->setStatusTip(tr("Help on usage of the graph editor"));
    connect(_helpAction, SIGNAL(triggered()), this, SLOT(help()));

    _mouseActionGroup = new QActionGroup(this);
    _mouseActionGroup->addAction(_selectAction);
    _mouseActionGroup->addAction(_insertNodeAction);
    _mouseActionGroup->addAction(_insertEdgeAction);
}

void GraphWidget::createContextMenus() {
    _nodeMenu = new QMenu(this);
    _nodeMenu->addAction(_deleteAction);

    _edgeMenu = new QMenu(this);
    _edgeMenu->addAction(_deleteAction);
    _edgeMenu->addAction(_editEdgeWeightAction);
}

void GraphWidget::createToolbar() {
    _toolBar = new QToolBar(this);
    _toolBar->setBackgroundRole(QPalette::Dark);
    _toolBar->addAction(_deleteAction);
    _toolBar->addSeparator();

    _toolBar->addAction(_selectAction);
    _toolBar->addAction(_insertNodeAction);
    _toolBar->addAction(_insertEdgeAction);
    _toolBar->addAction(_editEdgeWeightAction);

    _toolBar->addSeparator();
    _toolBar->addAction(_exportGraphAction);

    _toolBar->addSeparator();
    _toolBar->addAction(_helpAction);
}
