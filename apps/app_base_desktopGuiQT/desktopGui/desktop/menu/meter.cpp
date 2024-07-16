#include "meter.h"
#include "ui_meter.h"

Meter::Meter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Meter)
{
    ui->setupUi(this);
}

Meter::~Meter()
{
    delete ui;
}
