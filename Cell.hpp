#pragma once

#include <QGraphicsRectItem>
#include <QList>

class Cell : public QGraphicsRectItem
{
public:
    Cell();

    void addNeighbor( Cell *neighbor );

    void setAlive( bool is_alive );
    bool isAlive();
    void toggle();

protected:
    void advance(int phase) override;
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) override;

private:
    QList<Cell *> mNeighbors;
    bool mIsAlive;
    bool mNextState;
};
