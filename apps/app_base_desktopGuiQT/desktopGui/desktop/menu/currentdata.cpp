#include "currentdata.h"
#include "ui_currentdata.h"
#include"extern.h"

CurrentData::CurrentData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentData)
{
    ui->setupUi(this);

    pTimer = new QTimer(this);
    pTimer->start(1000);
    connect(pTimer,&QTimer::timeout,this,&CurrentData::timeout);

    ui->label_Ua->setAttribute(Qt::WA_TranslucentBackground);
    ui->label_Ua->setText("221.5 V");
    ui->label_Ub->setAttribute(Qt::WA_TranslucentBackground);
    ui->label_Ub->setText("221.5 V");
    ui->label_Uc->setAttribute(Qt::WA_TranslucentBackground);
    ui->label_Uc->setText("221.5 V");
}

CurrentData::~CurrentData()
{
    delete pTimer;
    delete ui;
}

//¶¨Ê±Æ÷
void CurrentData::timeout()
{
    ScreenCnt ++;

    if(ScreenCnt > 180)
    {
        this->deleteLater();
    }

}


//¹Ø±Õ
void CurrentData::on_pushButton_close_clicked()
{
    ScreenCnt = 0;
    this->deleteLater();
}
