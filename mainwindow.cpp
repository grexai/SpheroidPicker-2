#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imaging.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
   // imaging image;
    auto scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    Mat image = Mat::zeros(500,500,CV_8UC3);
    scene->addPixmap(QPixmap::fromImage(QImage(image.data,image.cols, image.rows, QImage::Format_RGB888)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


