#include "mntinfo.h"
#include "ui_mntinfo.h"
#include"extern.h"


MntInfo::MntInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MntInfo)
{
    ui->setupUi(this);
}

MntInfo::~MntInfo()
{
    delete ui;
}

//¹Ø±Õ
void MntInfo::on_pushButton_close_clicked()
{
    ScreenCnt = 0;
    this->deleteLater();
}

