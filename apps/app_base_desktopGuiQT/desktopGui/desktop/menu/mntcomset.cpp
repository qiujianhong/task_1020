#include "mntcomset.h"
#include "ui_mntcomset.h"
#include<QMessageBox>
#include"extern.h"


MntComSet::MntComSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MntComSet)
{
    ui->setupUi(this);

	pTimer = new QTimer(this);
    pTimer->start(1000);
    connect(pTimer,&QTimer::timeout,this,&MntComSet::timeout);
}

MntComSet::~MntComSet()
{
	delete pTimer;
    delete ui;
}

//定时器
void MntComSet::timeout()
{
    ScreenCnt ++;

    if(ScreenCnt > 180)
    {
        this->deleteLater();
    }

}


//终端地址设置
void MntComSet::on_pushButton_addr_clicked()
{
    ScreenCnt = 0;
	QMessageBox::warning(this,tr("提示"),tr("终端地址设置成功"),QMessageBox::Yes);
//    this->deleteLater();
}

//终端IP设置
void MntComSet::on_pushButton_ip_clicked()
{
    ScreenCnt = 0;
	QMessageBox::warning(this,tr("提示"),tr("IP地址设置成功"),QMessageBox::Yes);
//    this->deleteLater();
}

