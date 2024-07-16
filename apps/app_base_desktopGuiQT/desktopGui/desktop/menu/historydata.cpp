#include "historydata.h"
#include "ui_historydata.h"
#include"extern.h"
#include <QDateTime>
#include <QDate>


HistoryData::HistoryData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistoryData)
{
    ui->setupUi(this);

	QDateTime curDateTime=QDateTime::currentDateTime();
	ui->dateEdit->setDate(curDateTime.date());

	pTimer = new QTimer(this);
    pTimer->start(1000);
    connect(pTimer,&QTimer::timeout,this,&HistoryData::timeout);
}

HistoryData::~HistoryData()
{
	delete pTimer;
    delete ui;
}

//定时器
void HistoryData::timeout()
{
    ScreenCnt ++;

    if(ScreenCnt > 180)
    {
        this->deleteLater();
    }

}


//关闭
void HistoryData::on_pushButton_close_clicked()
{
    ScreenCnt = 0;
    this->deleteLater();
}

