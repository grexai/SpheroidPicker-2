#include "plateselector.h"
#include "ui_plateselector.h"
#include <iostream>

Plateselector::Plateselector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Plateselector)
{
    ui->setupUi(this);
    QPixmap pixmap("384wp.jpg");
    pixmap.scaledToWidth(120);
    ui->s_label->setPixmap(pixmap);
    ui->s_label->setFixedSize(120,80);
}

Plateselector::~Plateselector()
{
    delete ui;
}

void Plateselector::select_source_plate()
{
    this->m_selected_source = this->ui->s_p_selector->currentIndex();
}

void Plateselector::on_s_p_selector_currentIndexChanged(int index)
{
    this->m_selected_source = this->ui->s_p_selector->currentIndex();

    signal_s_p_changed();
}

void Plateselector::on_t_p_selector_currentIndexChanged(int index)
{
    this->m_selected_target = this->ui->t_p_selector->currentIndex();
    std::cout << "plateselector plate value" << this->m_selected_target << std::endl;
    signal_t_p_changed();
}
