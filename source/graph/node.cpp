#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

#include "node.h"
#include "edge.h"

Node::Node(QMenu* contextMenu, QGraphicsItem* parent/*, QGraphicsScene* scene*/)
    : QGraphicsItem(parent/*,scene*/)
{
    _contextMenu = contextMenu;
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QRectF Node::boundingRect() const {
    qreal adjust = 6;
    return QRectF(-15 - adjust, -15 - adjust,
                  33 + adjust, 33 + adjust);
}

QPainterPath Node::shape() const {
    QPainterPath path;
    path.addEllipse(-15, -15, 30, 30);
    return path;
}

void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::lightGray);
    painter->drawEllipse(-13, -13, 30, 30);

    QRadialGradient gradient(-5, -5, 15);
    gradient.setCenter(3, 3);
    gradient.setFocalPoint(5, 5);
    if (option->state & QStyle::State_Selected) {
        gradient.setColorAt(1, QColor(Qt::red).light(150));
        gradient.setColorAt(0, QColor(Qt::darkRed).light(150));
    }
    else {
        if (index < 0) {
            gradient.setColorAt(0, QColor(224,224,224));
            gradient.setColorAt(1, QColor(224,224,224));
        }
        else {
            gradient.setColorAt(0, QColor(128,160,208));
            gradient.setColorAt(1, QColor(54,96,144));
        }
    }
    painter->setBrush(gradient);
    painter->setPen(QPen(Qt::gray, 0));
    painter->drawEllipse(-15, -15, 30, 30);

    //painter->setFont(QApplication::font());
    QFont font;
    font.setPointSize(14);
    painter->setFont(font);
    QPointF centerPoint;
    if (index < 0)
        painter->setPen(Qt::darkGray);
    else
        painter->setPen(Qt::white);
    painter->setBrush(QBrush(Qt::white));
    painter->drawText (QRectF(-15,-15,30,30), Qt::AlignCenter, QString("%1").arg(index));
}

void Node::removeEdge(Edge* edge) {
    int index = _edges.indexOf(edge);
    if (index >= 0)
        _edges.removeAt(index);
}

void Node::removeAdjacentEdges() {
    foreach (Edge* edge, _edges) {
        edge->sourceNode()->removeEdge(edge);
        edge->targetNode()->removeEdge(edge);
        scene()->removeItem(edge);
        delete edge;
    }
}

void Node::addEdge(Edge* edge) {
    _edges.append(edge);
}

void Node::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    scene()->clearSelection();
    setSelected(true);
    _contextMenu->exec(event->screenPos());
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (Edge* edge, _edges) {
            edge->adjustToNodes();
        }
    }
    return value;
}
