#include "paramset.h"
#include "ui_paramset.h"
#include"extern.h"

ParamSet::ParamSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParamSet)
{
    ui->setupUi(this);

	pTimer = new QTimer(this);
    pTimer->start(1000);
    connect(pTimer,&QTimer::timeout,this,&ParamSet::timeout);

	mntcomset = new MntComSet(this);
	ui->gridLayout->addWidget(mntcomset);
	passage = new Passage(this);
	ui->gridLayout->addWidget(passage);
	meter = new Meter(this);
	ui->gridLayout->addWidget(meter);
	pulse = new Pulse(this);
	ui->gridLayout->addWidget(pulse);
	group = new Group(this);
	ui->gridLayout->addWidget(group);
	mnttime = new MntTime(this);
	ui->gridLayout->addWidget(mnttime);
	passwordset = new PasswordSet(this);
	ui->gridLayout->addWidget(passwordset);

	lastMenuIndex = -1;
	setButtonStyle(0);

}

ParamSet::~ParamSet()
{
	delete pTimer;
    delete ui;
}

//定时器
void ParamSet::timeout()
{
    ScreenCnt ++;

    if(ScreenCnt > 180)
    {
        this->deleteLater();
    }

}

void ParamSet::inintButtonStyle()
{

    ui->pushButton_mntcom->setStyleSheet("color:rgb(36, 166, 209);outline: none;border:none;background-color: rgb(200, 200, 200);");
    ui->pushButton_passage->setStyleSheet("color:rgb(36, 166, 209);outline: none;border:none;background-color: rgb(200, 200, 200);");
	ui->pushButton_meter->setStyleSheet("color:rgb(36, 166, 209);outline: none;border:none;background-color: rgb(200, 200, 200);");
    ui->pushButton_pulse->setStyleSheet("color:rgb(36, 166, 209);outline: none;border:none;background-color: rgb(200, 200, 200);");
	ui->pushButton_group->setStyleSheet("color:rgb(36, 166, 209);outline: none;border:none;background-color: rgb(200, 200, 200);");
	ui->pushButton_time->setStyleSheet("color:rgb(36, 166, 209);outline: none;border:none;background-color: rgb(200, 200, 200);");
	ui->pushButton_password->setStyleSheet("color:rgb(36, 166, 209);outline: none;border:none;background-color: rgb(200, 200, 200);");
	

}


void ParamSet::setButtonStyle(int buttonIndex)//设置按钮显示效果
{
    if(lastMenuIndex == buttonIndex)
        return;
    else
        lastMenuIndex = buttonIndex;

    inintButtonStyle();
    switch(buttonIndex)
    {
    case 0://(0, 153, 204)
        ui->pushButton_mntcom->setStyleSheet("color:white;outline: none;border:none;background-color: rgb(0, 153, 204);");
		mntcomset->show();
		passage->hide();
		meter->hide();
		pulse->hide();
		group->hide();
		mnttime->hide();
		passwordset->hide();
        break;
    case 1:
        ui->pushButton_passage->setStyleSheet("color:white;outline: none;border:none;background-color: rgb(0, 153, 204);");
		mntcomset->hide();
		passage->show();
		meter->hide();
		pulse->hide();
		group->hide();
		mnttime->hide();
		passwordset->hide();
        break;
	case 2://(0, 153, 204)
        ui->pushButton_meter->setStyleSheet("color:white;outline: none;border:none;background-color: rgb(0, 153, 204);");
		mntcomset->hide();
		passage->hide();
		meter->show();
		pulse->hide();
		group->hide();
		mnttime->hide();
		passwordset->hide();
        break;
    case 3:
        ui->pushButton_pulse->setStyleSheet("color:white;outline: none;border:none;background-color: rgb(0, 153, 204);");
		mntcomset->hide();
		passage->hide();
		meter->hide();
		pulse->show();
		group->hide();
		mnttime->hide();
		passwordset->hide();
        break;
	case 4://(0, 153, 204)
        ui->pushButton_group->setStyleSheet("color:white;outline: none;border:none;background-color: rgb(0, 153, 204);");
		mntcomset->hide();
		passage->hide();
		meter->hide();
		pulse->hide();
		group->show();
		mnttime->hide();
		passwordset->hide();
        break;
    case 5:
        ui->pushButton_time->setStyleSheet("color:white;outline: none;border:none;background-color: rgb(0, 153, 204);");
		mntcomset->hide();
		passage->hide();
		meter->hide();
		pulse->hide();
		group->hide();
		mnttime->show();
		passwordset->hide();
        break;
	case 6:
        ui->pushButton_password->setStyleSheet("color:white;outline: none;border:none;background-color: rgb(0, 153, 204);");
		mntcomset->hide();
		passage->hide();
		meter->hide();
		pulse->hide();
		group->hide();
		mnttime->hide();
	    passwordset->show();
        break;
    default:
        break;
    }
}



//关闭
void ParamSet::on_pushButton_close_clicked()
{
    ScreenCnt = 0;
    this->deleteLater();
}

void ParamSet::on_pushButton_mntcom_clicked()
{
    ScreenCnt = 0;
	setButtonStyle(0);
//    mntcomset->move(120,0);
}

void ParamSet::on_pushButton_passage_clicked()
{
    ScreenCnt = 0;
	setButtonStyle(1);
//    passage->move(120,0);
    

}

void ParamSet::on_pushButton_meter_clicked()
{
    ScreenCnt = 0;
	setButtonStyle(2);
//    meter->move(120,0);
    

}

void ParamSet::on_pushButton_pulse_clicked()
{
    ScreenCnt = 0;
	setButtonStyle(3);
//    pulse->move(120,0);
    

}

void ParamSet::on_pushButton_group_clicked()
{
    ScreenCnt = 0;
	setButtonStyle(4);
//    group->move(120,0);
    

}

void ParamSet::on_pushButton_time_clicked()
{
    ScreenCnt = 0;
	setButtonStyle(5);
//    mnttime->move(120,0);
    

}

void ParamSet::on_pushButton_password_clicked()
{
    ScreenCnt = 0;
	setButtonStyle(6);
//    passwordset->move(120,0);
	
}


