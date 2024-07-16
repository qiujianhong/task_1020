#include "passage.h"
#include "ui_passage.h"
#include"extern.h"


Passage::Passage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Passage)
{
    ui->setupUi(this);

	//菜单显示
    ethernetcfg = new EthernetCfg(this);
    gprscfg = new GprsCfg();
    ui->gridLayout->addWidget(ethernetcfg);
    ui->gridLayout->addWidget(gprscfg);

    lastMenuIndex = -1;
    setButtonStyle(0);//默认显示以太网
}

Passage::~Passage()
{
    delete ui;
}

void Passage::inintButtonStyle()
{

    ui->pushButton_ethernet->setStyleSheet("color:rgb(36, 166, 209);outline: none;border:none;background-color: rgb(200, 200, 200);");

    ui->pushButton_gprs->setStyleSheet("color:rgb(36, 166, 209);outline: none;border:none;background-color: rgb(200, 200, 200);");

}


void Passage::setButtonStyle(int buttonIndex)//设置按钮显示效果
{
    if(lastMenuIndex == buttonIndex)
        return;
    else
        lastMenuIndex = buttonIndex;

    inintButtonStyle();
    switch(buttonIndex)
    {
    case 0://(0, 153, 204)
        ui->pushButton_ethernet->setStyleSheet("color:white;outline: none;border:none;background-color: rgb(0, 153, 204);");
		 gprscfg->hide();
    	ethernetcfg->show();
        break;
    case 1:
        ui->pushButton_gprs->setStyleSheet("color:white;outline: none;border:none;background-color: rgb(0, 153, 204);");
		gprscfg->show();
    	ethernetcfg->hide();
        break;
    default:
        break;
    }
}

void Passage::on_pushButton_ethernet_clicked()
{
    ScreenCnt = 0;
    setButtonStyle(0);
}

void Passage::on_pushButton_gprs_clicked()
{
    ScreenCnt = 0;
    setButtonStyle(1);
    
}



