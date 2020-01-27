#include "plateselector.h"
#include "ui_plateselector.h"

Plateselector::Plateselector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Plateselector)
{
    ui->setupUi(this);
}

Plateselector::~Plateselector()
{
    delete ui;
}


