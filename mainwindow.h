#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "colony.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnStart_clicked();

    void on_btnAceptar_clicked();

    void on_tableCells_cellClicked(int row, int column);

    void evolveCells();

private:
    Ui::MainWindow *ui;
    int squareSide;
    bool startEvolving;
    Colony *cells;
    void resetGrid();
    void paintGrid();
    QTimer *timer;
};

#endif // MAINWINDOW_H
