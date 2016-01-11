#ifndef GRAPHSCENE_H
#define GRAPHSCENE_H

#include <QGraphicsScene>
#include <QPoint>

#include "node.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
QT_END_NAMESPACE

class GraphScene : public QGraphicsScene {
    Q_OBJECT
public:
    enum MouseMode { InsertNode, InsertEdge, MoveNode };

    GraphScene(QMenu* nodeMenu, QMenu* edgeMenu, QObject* parent = 0);
    QString shortGraphDescr();
    int addNode(QPointF pos, int index=1);
    void addEdge(int source, int target, int weight=0, bool marked=false);

public slots:
    void setMouseMode(MouseMode mode);
signals:
    void itemInserted(Node* item);
    void itemSelected(QGraphicsItem* item);
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);
private:
    int firstUnusedNodeIndex(int index=1);
    bool isItemChange(int type);

    QMenu* _nodeMenu;
    QMenu* _edgeMenu;
    MouseMode _mouseMode;
    QGraphicsLineItem* _dragLine;
};
#endif //GRAPHSCENE_H
