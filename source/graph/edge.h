#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsLineItem>

#include "node.h"

QT_BEGIN_NAMESPACE
class QGraphicsItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;
QT_END_NAMESPACE

class Edge : public QGraphicsLineItem {
public:
    enum { Type = UserType + 2 };

    Edge(QMenu* contextMenu, Node* sourceNode, Node* targetNode, QGraphicsItem* parent=0/*, QGraphicsScene* scene=0*/);

    virtual int type() const { return Type; }
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget=0);

    Node* sourceNode() const { return _source; }
    Node* targetNode() const { return _target; }

    static QBrush normalBrush;
    static QPen normalPen;
    static QBrush markedBrush;
    static QPen markedPen;
    static QBrush selectionBrush;
    static QPen selectionPen;

    int    weight;
    bool   marked;
public slots:
    void adjustToNodes();
protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
private:
    QRectF weightTextRect() const;

    QMenu* _contextMenu;
    Node*  _source;
    Node*  _target;
    bool   _sourceArrow;
    bool   _targetArrow;
};

#endif //EDGE_H
