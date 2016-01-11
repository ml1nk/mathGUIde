#include <QtGui>

#include "edge.h"
#include <math.h>

QBrush Edge::normalBrush = QBrush(QColor(0,204,153));
QBrush Edge::markedBrush = QBrush(QColor(101,255,215));
QBrush Edge::selectionBrush = QBrush(QColor(255,48,96));
QPen Edge::normalPen = QPen(normalBrush, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
QPen Edge::markedPen = QPen(markedBrush, 12, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
QPen Edge::selectionPen = QPen(selectionBrush, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

Edge::Edge(QMenu* contextMenu, Node* sourceNode, Node* targetNode, QGraphicsItem* parent, QGraphicsScene* scene)
:   QGraphicsLineItem(parent, scene),
    weight(0),
    marked(false)
{
    _contextMenu = contextMenu;

    _source = sourceNode;
    _target = targetNode;

    _sourceArrow = false;
    _targetArrow = false; // TODO

    setZValue(-1.0);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    //setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

QRectF Edge::boundingRect() const {
    qreal extra = (pen().width() + 30) / 2.0;
    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
            .normalized()
            .adjusted(-extra, -extra, extra, extra)
            | weightTextRect();
}

QPainterPath Edge::shape() const {
    QPointF p0 = line().p1();
    QPointF p1 = line().p2();
    QPointF p2 = p1;
    QPointF p3 = p0;

    QPointF sNorm = p1 - p0;
    double lg = hypot(sNorm.x(), sNorm.y());
    sNorm = sNorm * 4.0 / lg;
    QPointF sOrtho(-sNorm.y(), sNorm.x());

    p1 += sNorm;
    p0 -= sNorm;
    p0 += sOrtho;
    p1 += sOrtho;
    p2 -= sOrtho;
    p3 -= sOrtho;

    QPolygonF poly(5);
    poly << p0 << p1 << p2 << p3 << p0;

    QPainterPath path;
    path.addPolygon(poly);
    path.closeSubpath();

    // Weight text
    path.addRect(weightTextRect());

    return path;
}

QRectF Edge::weightTextRect() const {
    if (weight == 0)
        return QRectF();
    QRect rc = QApplication::fontMetrics().boundingRect(QString("%1").arg(weight));
    QPointF p = (_source->pos() + _target->pos()) / 2;
    return QRectF(rc.left()+p.x(), rc.top()+p.y(), rc.width(), rc.height());
}

void Edge::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    scene()->clearSelection();
    setSelected(true);
    _contextMenu->exec(event->screenPos());
}

void Edge::adjustToNodes() {
    QLineF line(mapFromItem(_source, 0, 0), mapFromItem(_target, 0, 0));
    setLine(line);
}

void Edge::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*) {
    static const double pi = 3.1415926535897932;
    static int arrowSize = 10;
    if (!_source || !_target)
        return;
    QPointF sourcePoint = _source->pos();
    QPointF targetPoint = _target->pos();

    // Draw the line itself
    QLineF line(sourcePoint, targetPoint);
    qreal length = line.length();
    QPointF edgeOffset((line.dx() * 12) / length, (line.dy() * 12) / length);
    sourcePoint = line.p1() + edgeOffset;
    targetPoint= line.p2() - edgeOffset;

    if (marked) {
        painter->setPen(markedPen);
        painter->drawLine(line);
    }
    painter->setPen((option->state & QStyle::State_Selected) ? selectionPen : normalPen);
    painter->drawLine(line);
    // Draw the arrows if there is enough room
    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = 2*pi - angle;
    QPointF sourceArrowP1 = sourcePoint + QPointF(sin(angle + pi/3) * arrowSize,      cos(angle + pi/3) * arrowSize);
    QPointF sourceArrowP2 = sourcePoint + QPointF(sin(angle + pi - pi/3) * arrowSize, cos(angle + pi - pi/3) * arrowSize);   
    QPointF destArrowP1 = targetPoint + QPointF(sin(angle - pi/3) * arrowSize,        cos(angle - pi/3) * arrowSize);
    QPointF destArrowP2 = targetPoint + QPointF(sin(angle - pi + pi/3) * arrowSize,   cos(angle - pi + pi/3) * arrowSize);
    painter->setBrush((option->state & QStyle::State_Selected) ? selectionBrush : normalBrush);
    if (_sourceArrow)
        painter->drawPolygon(QPolygonF() << sourcePoint << sourceArrowP1 << sourceArrowP2);
    if (_targetArrow)
        painter->drawPolygon(QPolygonF() << targetPoint << destArrowP1 << destArrowP2);

    if (weight > 0) {
        QFont font;
        font.setPointSize(12);
        painter->setFont(font);

        QPointF centerPoint = (sourcePoint + targetPoint) / 2;
        painter->setPen(Qt::black);
        painter->setBrush(QBrush(Qt::black));
        painter->drawText(centerPoint, QString("%1").arg(weight));
    }
}
