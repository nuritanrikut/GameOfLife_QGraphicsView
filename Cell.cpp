#include "Cell.hpp"

#include <QPainter>
#include <QDebug>

Cell::Cell() : mIsAlive( false )
{
    // We don't want borders
    setPen(Qt::NoPen);
}

void Cell::addNeighbor( Cell *neighbor )
{
    mNeighbors.append( neighbor );
}

void Cell::setAlive( bool is_alive )
{
    // We don't want to call update() if value did not change
    if( is_alive != mIsAlive )
    {
        mIsAlive = is_alive;

        // update() will schedule a repaint on the view
        // Does not necessarily mean it will happen right now
        update();
    }
}

bool Cell::isAlive()
{
    return mIsAlive;
}

void Cell::toggle()
{
    mIsAlive = !mIsAlive;

    update();
}

// This method will be called by QGraphicsScene
// It will be called twice. Once with phase=0, and once with phase=1
// We will calculate next state in phase=0 and update the state in phase=1
void Cell::advance( int phase )
{
    if( phase == 0 )
    {
        int alive_neighbors = 0;
        for( auto neighbor : mNeighbors )
        {
            if( neighbor->isAlive() )
                alive_neighbors++;
        }

        // From https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
        // """
        // At each step in time, the following transitions occur:
        //   1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
        //   2. Any live cell with two or three live neighbours lives on to the next generation.
        //   3. Any live cell with more than three live neighbours dies, as if by overpopulation.
        //   4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
        // """

        if( mIsAlive )
        {
            if( alive_neighbors < 2 )
                mNextState = false;
            else if( alive_neighbors > 3 )
                mNextState = false;
            else
                mNextState = true;
        }
        else
        {
            if( alive_neighbors == 3 )
                mNextState = true;
            else
                mNextState = false;
        }
    }
    else if( phase == 1 )
    {
        setAlive(mNextState);
    }
}

void Cell::paint( QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/ )
{
    QRectF rect = boundingRect();

    // Pen is for shape borders
    painter->setPen( Qt::NoPen );

    // Clear background
    {
        // Brush is for filling inside the shape
        QBrush brush( QColor( "black" ) );
        painter->setBrush( brush );
        painter->drawRect( rect );
    }

    // Draw a white circle if alive
    if( mIsAlive )
    {
        QBrush brush( QColor( "white" ) );
        painter->setBrush( brush );
        painter->drawEllipse( rect );
    }
}
