#include "paramshow.h"
#include "ui_paramshow.h"

ParamShow::ParamShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParamShow)
{
    ui->setupUi(this);
}

ParamShow::~ParamShow()
{
    delete ui;
}
