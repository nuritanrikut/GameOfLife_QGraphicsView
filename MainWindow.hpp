#pragma once

#include <QGraphicsView>
#include <QMainWindow>
#include <QSpinBox>
#include <QAtomicInt>
#include <QTimer>
#include <QElapsedTimer>

#include "Cell.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = nullptr );
    ~MainWindow();

public Q_SLOTS:
    void generateCells();
    void startGame();
    void stopGame();
    void onSelectionChanged();
    void onFrame();

private:
    void showEvent( QShowEvent *event ) override;
    void resizeEvent( QResizeEvent *event ) override;

private:
    QGraphicsView *mGraphicsView;
    QGraphicsScene *mGraphicsScene;
    QSpinBox *mSbWidth;
    QSpinBox *mSbHeight;

    QList<Cell *> mCells;

    bool mSelectionFlag;

    QTimer mFrameTimer;
    QElapsedTimer mElapsedTimer;
};
