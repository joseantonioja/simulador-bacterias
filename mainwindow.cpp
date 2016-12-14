#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <iostream>
#include <QString>
#include <QMessageBox>
#include <algorithm>
#include <fstream>
#include <QThread>
#include <QTimer>
#define MAX_W_GRID 1280
#define MAX_H_GRID 580
#define COLOR_GOOD_B "#070537"
#define COLOR_BAD_B "#d0cefa"
#define COLOR_TOXIN "#e31d10"
#define COLOR_FOOD "#6ae310"
#define COLOR_BLANK "#ffffff"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btnStart->setIcon(QIcon("play.png"));
    this->squareSide=7;
    this->startEvolving=false;
    this->timer = new QTimer(this);
    ui->btnStart->setEnabled(false);
    connect(this->timer, SIGNAL(timeout()), this, SLOT(evolveCells()));
    ui->tableCells->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tableCells->setStyleSheet("QTableView {selection-background-color:transparent}");
    ui->tableCells->setShowGrid(true);
    ui->tableCells->horizontalHeader()->hide();
    ui->tableCells->verticalHeader()->hide();
    ui->tableCells->verticalHeader()->setDefaultSectionSize(this->squareSide);
    ui->tableCells->horizontalHeader()->setDefaultSectionSize(this->squareSide);
    ui->tableCells->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resetGrid(){
    int tmpW = this->squareSide*this->cells->cols;
    int tmpH = this->squareSide*this->cells->rows;
    ui->tableCells->setGeometry(10, 190, min(tmpW, MAX_W_GRID), min( tmpH , MAX_H_GRID ));
    if(tmpW<MAX_W_GRID)
        ui->tableCells->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    else
        ui->tableCells->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    if(tmpH<MAX_H_GRID)
        ui->tableCells->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    else
        ui->tableCells->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->tableCells->horizontalHeader()->setDefaultSectionSize(this->squareSide);
    ui->tableCells->verticalHeader()->setDefaultSectionSize(this->squareSide);
    ui->tableCells->setRowCount(this->cells->rows);
    ui->tableCells->setColumnCount(this->cells->cols);
    ui->lblGen->setText("0");
    cout << "Resetting grid"<<endl;
    this->cells->printCells();
    ui->lblBacterias->setText(QString::number(this->cells->counters[BACTERIUM]));
    ui->lblFood->setText(QString::number(this->cells->counters[FOOD]));
    ui->lblToxins->setText(QString::number(this->cells->counters[TOXIN]));
    for(int i=0; i<this->cells->rows; i++)
        for(int j=0; j< this->cells->cols; j++){
            ui->tableCells->setItem(i, j, new QTableWidgetItem);
            ui->tableCells->item(i, j)->setSelected(false);
            ui->tableCells->item(i, j)->setFlags(ui->tableCells->item(i, j)->flags()&= ~Qt::ItemIsEditable);
        }
    this->paintGrid();
    ui->tableCells->setVisible(true);

}

void MainWindow::paintGrid(){
    for(int i=0; i<this->cells->rows; i++)
        for(int j=0; j< this->cells->cols; j++){
            if(this->cells->cells[i][j]->type==BLANK)
                ui->tableCells->item(i, j)->setBackground(QBrush(QColor(COLOR_BLANK)));
            else if(this->cells->cells[i][j]->type==BACTERIUM){
                if(this->cells->cells[i][j]->energy < this->cells->minEnergy)
                    ui->tableCells->item(i, j)->setBackground(QBrush(QColor(COLOR_BAD_B)));
                else
                    ui->tableCells->item(i, j)->setBackground(QBrush(QColor(COLOR_GOOD_B)));
            }
            else if(this->cells->cells[i][j]->type == FOOD)
                ui->tableCells->item(i, j)->setBackground(QBrush(QColor(COLOR_FOOD)));
            else
                ui->tableCells->item(i, j)->setBackground(QBrush(QColor(COLOR_TOXIN)));
        }
}
void MainWindow::on_btnStart_clicked()
{
    if(this->startEvolving){
        ui->btnStart->setIcon(QIcon("play.png"));
        this->timer->stop();
        cout << "Stopping timer"<<endl;
        ui->btnAceptar->setEnabled(true);
    }
    else{
        ui->btnStart->setIcon(QIcon("pause.png"));
        this->timer->start(1000);
        cout << "Starting timer "<<endl;
        ui->btnAceptar->setEnabled(false);
    }
    this->startEvolving = !this->startEvolving;
}


void MainWindow::on_btnAceptar_clicked()
{
    cout << "Giga wat"<<endl;
    this->squareSide =ui->spinPunto->value();
    int rows = ui->spinH->value();
    int cols = ui->spinW->value();
    int breedLapse = ui->spinBreed->value();
    int boostFeed = ui->spinEFood->value();
    int initialTime = ui->spinEStart->value();
    int toxicDamage = ui->spinDToxin->value();
    int toxicLapse = ui->spinLToxin->value();
    int eLostIteration = ui->spinEIt->value();
    int mEnergy = ui->spinMinE->value();
    if(ui->boxAleatorio->isChecked()){
        double probBact = ui->spinPBac->value();
        double probFood = ui->spinPFood->value();
        double probToxin = ui->spinPToxin->value();
        if(probBact + probFood + probToxin <= 1){
            this->cells = new Colony(rows, cols, breedLapse, boostFeed, probFood, probBact, probToxin, initialTime, toxicDamage, toxicLapse, mEnergy, eLostIteration);
            //Colony::Colony(int r, int c, int bl, int bf, double probFood, double probBact, double probToxin, int initialTime, int td , int tt, int me, int el){
            this->resetGrid();
            ui->btnStart->setEnabled(true);
        }
        else{
            QMessageBox msgBox(QMessageBox::Warning, "Error", "probBact + probFood + probToxin <= 1",QMessageBox::Ok,this);
            msgBox.exec();
        }
    }
    else{
        this->cells = new Colony(rows, cols, breedLapse, boostFeed, -1, -1, -1, initialTime, toxicDamage, toxicLapse, mEnergy, eLostIteration);
        //Colony::Colony(int r, int c, int bl, int bf, double probFood, double probBact, double probToxin, int initialTime, int td , int tt, int me, int el){
        this->resetGrid();
        ui->btnStart->setEnabled(true);
    }
    /*
    breedLapse = bl;
    boostFeeding = bf;
    rows = r;
    cols = c;
    offset = 0;
    toxicTime = tt;
    toxinDamage = td;
    initialTl = initialTime;
    minEnergy = me;
    energyLostIteration = el;*/
    /*if(ui->boxAleatorio->isChecked()){
       double probBact = ui->spinPBac->value();
       double probFood = ui->spinPFood->value();
       if(probBact+probFood< 1){
          this->cells = new Colony(rows, cols, breedLapse, boostFeedMin, boostFeedMax, probFood, probBact, initialTime);
           this->resetGrid();
           ui->btnStart->setEnabled(true);
       }
       else
           cout << "What's wrong with you"<<endl;
     }
     else{
     //int r, int c, int bl, int bf, double probFood, double probBact, int initialTime
        this->cells = new Colony(rows, cols, breedLapse, boostFeedMin, boostFeedMax, -1, -1, initialTime);
        this->resetGrid();
        ui->btnStart->setEnabled(true);
     }*/
}

void MainWindow::on_tableCells_cellClicked(int row, int column)
{
    cout << "Clicked "<<  (char)(cells->cells[row][column]->type + '0')<<endl;
    this->cells->cells[row][column]->age = 0;
    this->cells->cells[row][column]->energy = 0;
    if(this->cells->cells[row][column]->type==BLANK){
        this->cells->cells[row][column]->type = BACTERIUM;
        this->cells->cells[row][column]->energy = this->cells->initialTl;
        ui->tableCells->item(row, column)->setBackground(QBrush(QColor(COLOR_GOOD_B)));
    }
    else if(this->cells->cells[row][column]->type==BACTERIUM){
        this->cells->cells[row][column]->type = FOOD;
        ui->tableCells->item(row, column)->setBackground(QBrush(QColor(COLOR_FOOD)));
    }
    else if(this->cells->cells[row][column]->type == FOOD){
        this->cells->cells[row][column]->type = TOXIN;
        ui->tableCells->item(row, column)->setBackground(QBrush(QColor(COLOR_TOXIN)));
    }
    else{
        this->cells->cells[row][column]->type = BLANK;
        ui->tableCells->item(row, column)->setBackground(QBrush(QColor(COLOR_BLANK)));
    }
    cout << "Pasa a:"<< (char)(cells->cells[row][column]->type + '0') << endl;
}

void MainWindow::evolveCells(){
    this->cells->gen+=1;
    ui->lblGen->setText(QString::number(this->cells->gen));
    this->cells->eatAndBreed();
    //this->cells->printCells();
    ui->lblBacterias->setText(QString::number(this->cells->counters[BACTERIUM]));
    ui->lblFood->setText(QString::number(this->cells->counters[FOOD]));
    ui->lblToxins->setText(QString::number(this->cells->counters[TOXIN]));
    paintGrid();
    if(this->cells->counters[BACTERIUM]==0){
        QMessageBox msgBox(QMessageBox::Warning, "Parar", "No hay bacterias en la colonia",QMessageBox::Ok,this);
        msgBox.exec();
        ui->btnAceptar->setEnabled(true);
        ui->btnStart->setEnabled(false);
        ui->btnStart->setIcon(QIcon("play.png"));
        this->timer->stop();
        return;
    }else{
        QThread::currentThread()->usleep(500);
        this->cells->move();
        //this->cells->printCells();
        paintGrid();
        this->cells->decreaseEnergy();
        ui->lblGen->setText(QString::number(this->cells->gen));
        return;
    }
}
