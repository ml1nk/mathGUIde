#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QToolBar>
#include <QPointF>

#include "node.h"

class GraphScene;

QT_BEGIN_NAMESPACE
class QAction;
class QComboBox;
class QActionGroup;
class QGraphicsView;
QT_END_NAMESPACE


class GraphWidget : public QWidget {
    Q_OBJECT
public:
    GraphWidget();

    int addNode(QPointF pos, int index=0);
    void addEdge(int source, int target, int weight=0, bool marked=false);
    void clear();

private slots:
    void deleteItem();
    void setSelectMode();
    void setNodeMode();
    void setEdgeMode();
    void editEdgeWeight();
    void itemInserted(Node* item);
    void itemSelected(QGraphicsItem* item);
    void selectionChanged();
    void exportGraph();
    void help();
signals:
    void graphDefined(QString def);
    void helpRequested();

private:
    void createActions();
    void createContextMenus();
    void createToolbar();

    GraphScene*    _scene;
    QGraphicsView* _view;

    QAction* _insertNodeAction;
    QAction* _insertEdgeAction;
    QAction* _editEdgeWeightAction;
    QAction* _selectAction;
    QAction* _deleteAction;
    QAction* _exportGraphAction;
    QAction* _helpAction;

    QActionGroup* _mouseActionGroup;

    QMenu* _nodeMenu;
    QMenu* _edgeMenu;
public:
    QToolBar* _toolBar;
};
#endif // GRAPHWIDGET_H
