#include "MainWindow.hpp"

#include <QGraphicsItem>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QRandomGenerator>
#include <QPushButton>

MainWindow::MainWindow( QWidget *parent )
    : QMainWindow( parent ),
      mGraphicsView( new QGraphicsView ),
      mGraphicsScene( new QGraphicsScene ),
      mSbWidth( new QSpinBox ),
      mSbHeight( new QSpinBox ),
      mSelectionFlag( false )
{
    setWindowTitle( "Game of Life" );
    setMinimumSize( 800, 600 );

    auto topLayout = new QHBoxLayout();

    {
        topLayout->addWidget( new QLabel( "Width" ) );

        mSbWidth->setMinimum( 3 );
        mSbWidth->setMaximum( 1000 );
        mSbWidth->setValue( 80 );

        // We will read its value when generating cells

        topLayout->addWidget( mSbWidth );
    }

    {
        topLayout->addWidget( new QLabel( "Height" ) );

        mSbHeight->setMinimum( 3 );
        mSbHeight->setMaximum( 1000 );
        mSbHeight->setValue( 60 );

        // We will read its value when generating cells

        topLayout->addWidget( mSbHeight );
    }

    {
        // Once connections are done and added to layout, we don't need to keep the pointer to button
        // Hence it is defined locally, not as a member variable
        auto button = new QPushButton( "Generate" );

        // We can connect signals to member functions
        connect( button, &QPushButton::clicked, this, &MainWindow::generateCells );

        topLayout->addWidget( button );
    }

    {
        topLayout->addWidget( new QLabel( "Update Interval (ms)" ) );

        // We will receive its value with signal/slots
        // Hence it is defined locally, not as a member variable
        auto sbInterval = new QSpinBox();
        sbInterval->setMinimum( 10 );
        sbInterval->setMaximum( 1000 );

        // We can connect signals to lambda functions
        connect( sbInterval, &QSpinBox::valueChanged, [this]( int value ) {
            mFrameTimer.setInterval( std::chrono::milliseconds( value ) );
        } );

        // Connect before setting the value, so that slot would be called
        sbInterval->setValue( 33 );

        topLayout->addWidget( sbInterval );
    }

    {
        auto button = new QPushButton( "Single Iteration" );
        connect( button, &QPushButton::clicked, mGraphicsScene, &QGraphicsScene::advance );
        topLayout->addWidget( button );
    }

    // SpacerItem will expand horizontally and align everthing else to the left
    topLayout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred ) );

    {
        auto mainLayout = new QVBoxLayout();
        mainLayout->addLayout( topLayout );
        mainLayout->addWidget( mGraphicsView );

        auto mainWidget = new QWidget();
        mainWidget->setLayout( mainLayout );
        setCentralWidget( mainWidget );
    }

    mGraphicsView->setScene( mGraphicsScene );

    // MenuBar is enabled by default
    {
        // Add one top level menu
        // and some sub menus
        auto menuFile = menuBar()->addMenu( "File" );

        auto actionGenerate = menuFile->addAction( "Generate", this, &MainWindow::generateCells );
        auto actionStart = menuFile->addAction( "Start", this, &MainWindow::startGame );
        auto actionStop = menuFile->addAction( "Stop", this, &MainWindow::stopGame );
        auto actionExit = menuFile->addAction( "Exit", [this]() {
            // There are five levels of logging functions
            // qDebug()
            // qInfo()
            // qWarning()
            // qCritical()
            // qFatal()

            qDebug() << "Exit";

            // We will close the main window, thus exit the application
            close();
        } );

        // We can add a ToolBar
        auto toolbar = addToolBar( "File" );

        // We can use the same actions above, or create new ones
        toolbar->addAction( actionGenerate );
        toolbar->addAction( actionStart );
        toolbar->addAction( actionStop );
        toolbar->addAction( actionExit );
    }

    // StatusBar is not created by default
    setStatusBar( new QStatusBar() );

    // This will allow us to toggle cell state by clicking on them
    connect( mGraphicsScene, &QGraphicsScene::selectionChanged, this, &MainWindow::onSelectionChanged );

    // If set to singleShot, timer will fire only once
    mFrameTimer.setSingleShot( false );
    mFrameTimer.stop();
    connect( &mFrameTimer, &QTimer::timeout, this, &MainWindow::onFrame );

    generateCells();

    setFocus();
}

MainWindow::~MainWindow() { }

void MainWindow::generateCells()
{
    qDebug() << "Generating";

    // Clear existing cells
    while( !mCells.empty() )
    {
        auto cell = mCells.back();
        mCells.pop_back();
        mGraphicsScene->removeItem( cell );
        delete cell;
    }

    auto startPosition = QPointF( 0.0, 0.0 );
    auto const cellSize = QSizeF( 10.0, 10.0 );

    auto cellRectangle = QRectF(startPosition, cellSize);

    int gridWidth = mSbWidth->value();
    int gridHeight = mSbHeight->value();

    for( int y = 0; y < gridHeight; y++ )
    {
        cellRectangle.moveLeft(startPosition.x());

        for( int x = 0; x < gridWidth; x++ )
        {
            // Random
            quint32 randomValue = QRandomGenerator::global()->bounded( 2 );
            bool alive = randomValue == 1;

            Cell *cell = new Cell();
            cell->setRect( cellRectangle );
            cell->setAlive( alive );

            // We will click on cells to toggle
            cell->setFlag( QGraphicsItem::ItemIsSelectable );

            mGraphicsScene->addItem( cell );
            mCells.push_back( cell );

            cellRectangle.translate(cellSize.width(), 0);
        }

        cellRectangle.translate(0, cellSize.height());
    }

    int index = 0;
    for( int y = 0; y < gridHeight; y++ )
    {
        for( int x = 0; x < gridWidth; x++ )
        {
            Cell *cell = mCells.at( index );

            int left = y > 0 ? y - 1 : y;
            int right = y < gridHeight - 1 ? y + 1 : y;
            int top = x > 0 ? x - 1 : x;
            int bottom = x < gridWidth - 1 ? x + 1 : x;

            for( int ny = left; ny <= right; ny++ )
            {
                for( int nx = top; nx <= bottom; nx++ )
                {
                    if( ny == y && nx == x )
                        continue;

                    Cell *neighbor = mCells.at( ny * gridWidth + nx );
                    cell->addNeighbor( neighbor );
                }
            }
            index++;
        }
    }

    auto topLeft = mCells.front()->boundingRect().topLeft();
    auto bottomRight = mCells.back()->boundingRect().topLeft();
    mGraphicsScene->setSceneRect( QRectF( topLeft, bottomRight ) );

    // Zoom and pan the view so that all cells are visible
    mGraphicsView->fitInView( mGraphicsScene->sceneRect(), Qt::KeepAspectRatio );

    // This message will be shown for 1sec and disappear
    statusBar()->showMessage( "Generated", 1000 );
}

void MainWindow::startGame()
{
    qDebug() << "Start";

    // Start firing timer events
    mFrameTimer.start();

    // We will calculate framerate with this
    mElapsedTimer.start();
}

void MainWindow::stopGame()
{
    mFrameTimer.stop();

    statusBar()->showMessage( "Stopped" );
}

void MainWindow::onSelectionChanged()
{
    auto items = mGraphicsScene->selectedItems();
    if( items.empty() )
        return;

    if( mSelectionFlag )
    {
        mSelectionFlag = false;
        mGraphicsScene->clearSelection();
        return;
    }

    mSelectionFlag = true;

    for( auto item : items )
    {
        Cell *cell = dynamic_cast<Cell *>( item );
        if( cell )
        {
            cell->toggle();
        }
    }

    mGraphicsScene->clearSelection();
}

void MainWindow::onFrame()
{
    // Advance simulation one step
    // It will call advance(phase) on each cell
    mGraphicsScene->advance();

    auto elapsedMs = mElapsedTimer.restart();
    if( elapsedMs > 0 )
    {
        double fps = 1000.0 / elapsedMs;
        statusBar()->showMessage( QString( "Current FPS: %1" ).arg( fps, 4, 'f', 2, ' ' ) );
    }
}

// This is called when the window becomes visible
void MainWindow::showEvent( QShowEvent * /*event*/ )
{
    // Zoom and pan the view so that all cells are visible
    mGraphicsView->fitInView( mGraphicsScene->sceneRect(), Qt::KeepAspectRatio );
}

// This is called when the window resizes
void MainWindow::resizeEvent( QResizeEvent * /*event*/ )
{
    // Zoom and pan the view so that all cells are visible
    mGraphicsView->fitInView( mGraphicsScene->sceneRect(), Qt::KeepAspectRatio );
}
