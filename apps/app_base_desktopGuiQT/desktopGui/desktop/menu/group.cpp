#include "group.h"
#include "ui_group.h"

Group::Group(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Group)
{
    ui->setupUi(this);
}

Group::~Group()
{
    delete ui;
}
