#include <QtGui>
#include <QGraphicsSceneMouseEvent>
#include <QTransform>

#include "graphscene.h"
#include "edge.h"

GraphScene::GraphScene(QMenu* nodeMenu, QMenu* edgeMenu, QObject* parent)
    : QGraphicsScene(parent)
{
    _nodeMenu = nodeMenu;
    _edgeMenu = edgeMenu;
    _mouseMode = InsertNode;
    _dragLine = 0;
}

QString GraphScene::shortGraphDescr() {
    QString s = "Graph.fromShortRepr(\"";
    // 1st node  2nd node  ... 1st edge ...
    // index,x,y;index,x,y;...|sourceIndex,targetIndex,weight;...
    QStringList nodeList;
    foreach (QGraphicsItem* item, items()) {
        if (item->type() == Node::Type) {
            Node* node = qgraphicsitem_cast<Node*>(item);
            nodeList.append(QString("%1,%2,%3").arg(node->index).arg(node->x()).arg(node->y()));
        }
    }
    s.append(nodeList.join(";"));
    s.append("|");
    QStringList edgeList;
    foreach (QGraphicsItem* item, items()) {
        if (item->type() == Edge::Type) {
            Edge* edge = qgraphicsitem_cast<Edge*>(item);
            if (edge->weight != 0)
                edgeList.append(QString("%1,%2,%3").arg(edge->sourceNode()->index)
                    .arg(edge->targetNode()->index).arg(edge->weight));
            else
                edgeList.append(QString("%1,%2").arg(edge->sourceNode()->index)
                    .arg(edge->targetNode()->index));
        }
    }
    s.append(edgeList.join(";"));
    s.append("\")");
    return s;
}

void GraphScene::setMouseMode(MouseMode mode) {
    _mouseMode = mode;
}

int GraphScene::firstUnusedNodeIndex(int index/*=1*/) {
    QList<int> nodeIndices;
    foreach (QGraphicsItem* item, items()) {
        if (item->type() == Node::Type) {
            Node* node = qgraphicsitem_cast<Node*>(item);
            nodeIndices.append(node->index);
        }
    }
    if (nodeIndices.contains(index)) {
        for (index=1; index<10000; index++) {
            if (!nodeIndices.contains(index))
                break;
        }
    }
    return index;
}

int GraphScene::addNode(QPointF pos, int index/*=1*/) {
    index = firstUnusedNodeIndex(index);
    Node* node = new Node(_nodeMenu);
    node->index = index;
    node->setZValue(index);
    addItem(node);
    node->setPos(pos);
    emit itemInserted(node);
    return index;
}

void GraphScene::addEdge(int source, int target, int weight/*=0*/, bool marked/*=false*/) {
    Node* sourceNode = 0;
    foreach (QGraphicsItem* item, items()) {
        if (item->type() == Node::Type) {
            sourceNode = qgraphicsitem_cast<Node*>(item);
            if (sourceNode->index == source)
                break;
        }
    }
    Node* targetNode = 0;
    foreach (QGraphicsItem* item, items()) {
        if (item->type() == Node::Type) {
            targetNode = qgraphicsitem_cast<Node*>(item);
            if (targetNode->index == target)
                break;
        }
    }
    if (sourceNode == 0 || targetNode == 0)
        return;

    Edge* edge = new Edge(_edgeMenu, sourceNode, targetNode);
    edge->weight = weight;
    edge->marked = marked;
    sourceNode->addEdge(edge);
    targetNode->addEdge(edge);
    edge->setZValue(-1000.0);
    addItem(edge);
    edge->adjustToNodes();
}

void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) {
    if (mouseEvent->button() != Qt::LeftButton)
        return;
    switch (_mouseMode) {
        case InsertNode: {
            addNode(mouseEvent->scenePos());
            break;
        }
        case InsertEdge: {
            QGraphicsItem* item = itemAt(mouseEvent->scenePos(),QTransform());
            if (item && item->type() == Node::Type) {
                _dragLine = new QGraphicsLineItem(QLineF(item->pos(), mouseEvent->scenePos()));
                QPen pen(Edge::normalPen);
                pen.setStyle(Qt::DashLine);
                _dragLine->setPen(pen);
                addItem(_dragLine);
            }
            break;
        }
        default:
            QGraphicsScene::mousePressEvent(mouseEvent);
            break;
    }
    //QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) {
    if (_mouseMode == InsertEdge && _dragLine != 0) {
        QLineF newLine(_dragLine->line().p1(), mouseEvent->scenePos());
        _dragLine->setLine(newLine);
    }
    else if (_mouseMode == MoveNode) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) {
    if (_dragLine != 0 && _mouseMode == InsertEdge) {
        QList<QGraphicsItem*> startItems = items(_dragLine->line().p1());
        if (startItems.count() > 0 && startItems.first() == _dragLine)
            startItems.removeFirst();
        QList<QGraphicsItem*> endItems = items(_dragLine->line().p2());
        if (endItems.count() > 0 && endItems.first() == _dragLine)
            endItems.removeFirst();
        removeItem(_dragLine);
        delete _dragLine;

        if (startItems.count() > 0 && endItems.count() > 0
                && startItems.first()->type() == Node::Type
                && endItems.first()->type() == Node::Type
                && startItems.first() != endItems.first()) {
            Node* sourceNode = qgraphicsitem_cast<Node*>(startItems.first());
            Node* targetNode = qgraphicsitem_cast<Node*>(endItems.first());
            Edge* edge = new Edge(_edgeMenu, sourceNode, targetNode);
            sourceNode->addEdge(edge);
            targetNode->addEdge(edge);
            //edge->setZValue(-1.0);
            addItem(edge);
            edge->adjustToNodes();
        }
    }
    _dragLine = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

bool GraphScene::isItemChange(int type) {
    foreach (QGraphicsItem* item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}
