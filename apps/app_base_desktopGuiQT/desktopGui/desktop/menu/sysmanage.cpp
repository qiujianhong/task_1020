#include "sysmanage.h"
#include "ui_sysmanage.h"
#include"extern.h"

SysManage::SysManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SysManage)
{
    ui->setupUi(this);
}

SysManage::~SysManage()
{
    delete ui;
}

//¹Ø±Õ
void SysManage::on_pushButton_close_clicked()
{
    ScreenCnt = 0;
    this->deleteLater();
}

