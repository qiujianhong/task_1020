#include "mnttime.h"
#include "ui_mnttime.h"

MntTime::MntTime(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MntTime)
{
    ui->setupUi(this);
}

MntTime::~MntTime()
{
    delete ui;
}
