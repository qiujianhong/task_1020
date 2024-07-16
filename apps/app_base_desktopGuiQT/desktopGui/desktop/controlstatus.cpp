#include "controlstatus.h"
#include "ui_controlstatus.h"

ControlStatus::ControlStatus(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlStatus)
{
    ui->setupUi(this);
}

ControlStatus::~ControlStatus()
{
    delete ui;
}
