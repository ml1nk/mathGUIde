#ifndef NODE_H
#define NODE_H

#include <QGraphicsPixmapItem>
#include <QList>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
class QGraphicsScene;
class QTextEdit;
//class QGraphicsSceneMouseEvent;
class QMenu;
class QGraphicsSceneContextMenuEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
QT_END_NAMESPACE

class Edge;

class Node : public QGraphicsItem {
public:
    enum { Type = UserType + 1 };
    Node(QMenu* contextMenu, QGraphicsItem* parent=0, QGraphicsScene* scene=0);
    virtual int type() const { return Type; }
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    void removeEdge(Edge* edge);
    void removeAdjacentEdges();
    void addEdge(Edge* edge);
    int index;
protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
    QMenu*       _contextMenu;
    QList<Edge*> _edges;
};
#endif //NODE_H
