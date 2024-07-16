#include "desktop.h"
#include "./ui_desktop.h"
#include <QTimer>
#include"extern.h"

desktop::desktop(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::desktop)
{
    ui->setupUi(this);
//    this->setWindowFlags(Qt::Window| Qt::FramelessWindowHint);        //去掉边框
//    setAttribute(Qt::WA_TranslucentBackground);     //把窗口背景设置为透明

    //主菜单显示
    menu = new Menu(this);
    controlStatus = new ControlStatus();
    paramShow = new ParamShow();
    powerBuy = new PowerBuy();
    ui->gridLayout_show->addWidget(powerBuy);
    ui->gridLayout_show->addWidget(paramShow);
    ui->gridLayout_show->addWidget(controlStatus);
    ui->gridLayout_show->addWidget(menu);
    controlStatus->hide();
    paramShow->hide();
    powerBuy->hide();
    menu->show();

    lastMenuIndex = -1;
    setButtonStyle(0);//默认显示主菜单

    QTimer *timer = new QTimer(this);
    timer->start(1000); // 每次发射timeout信号时间间隔为1秒
    connect(timer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
}

desktop::~desktop()
{
    delete ui;
}


void desktop::inintButtonStyle()
{

    ui->pushButton_menu->setStyleSheet("color:rgb(36, 166, 209);outline: none;border:none;background-color: rgb(200, 200, 200);");
    ui->pushButton_menu->setIcon(QIcon(":/new/imgs/images/icon_03.png"));

    ui->pushButton_controlStatus->setStyleSheet("color:rgb(36, 166, 209);outline: none;border:none;background-color: rgb(200, 200, 200);");
    ui->pushButton_controlStatus->setIcon(QIcon(":/new/imgs/images/icon_08.png"));

    ui->pushButton_paramShow->setStyleSheet("color:rgb(36, 166, 209);outline: none;border:none;background-color: rgb(200, 200, 200);");
    ui->pushButton_paramShow->setIcon(QIcon(":/new/imgs/images/icon_06.png"));

    ui->pushButton_powerBuy->setStyleSheet("color:rgb(36, 166, 209);outline: none;border:none;background-color: rgb(200, 200, 200);");
    ui->pushButton_powerBuy->setIcon(QIcon(":/new/imgs/images/icon_26.png"));

    QFont font;
    font.setFamily(QString::fromUtf8("Abyssinica SIL"));
    font.setPointSize(18);
    font.setBold(true);
    font.setWeight(87);
    //ui->label_time->setFont(font);
    ui->label_4->setFont(font);
}

void desktop::setButtonStyle(int buttonIndex)//设置按钮显示效果
{
    if(lastMenuIndex == buttonIndex)
        return;
    else
        lastMenuIndex = buttonIndex;

    inintButtonStyle();
    switch(buttonIndex)
    {
    case 0://(0, 153, 204)
        ui->pushButton_menu->setStyleSheet("color:white;outline: none;border:none;background-color: rgb(0, 153, 204);");
        ui->pushButton_menu->setIcon(QIcon(":/new/imgs/images/icon0.png"));
        break;
    case 1:
        ui->pushButton_controlStatus->setStyleSheet("color:white;outline: none;border:none;background-color: rgb(0, 153, 204);");
        ui->pushButton_controlStatus->setIcon(QIcon(":/new/imgs/images/icon_25.png"));
        break;
    case 2:
        ui->pushButton_paramShow->setStyleSheet("color:white;outline: none;border:none;background-color: rgb(0, 153, 204);");
        ui->pushButton_paramShow->setIcon(QIcon(":/new/imgs/images/icon_24.png"));
        break;
    case 3:
        ui->pushButton_powerBuy->setStyleSheet("color:white;outline: none;border:none;background-color: rgb(0, 153, 204);");
        ui->pushButton_powerBuy->setIcon(QIcon(":/new/imgs/images/icon_10.png"));
        break;
    default:
        break;
    }
}

void desktop::on_pushButton_menu_pressed()
{
    ScreenCnt = 0;
    setButtonStyle(0);
    controlStatus->hide();
    paramShow->hide();
    powerBuy->hide();
    menu->show();
}

void desktop::on_pushButton_controlStatus_pressed()
{
    ScreenCnt = 0;
    setButtonStyle(1);;
    menu->hide();
    paramShow->hide();
    powerBuy->hide();
    controlStatus->show();
}

void desktop::on_pushButton_paramShow_pressed()
{
    ScreenCnt = 0;
    setButtonStyle(2);
    menu->hide();
    controlStatus->hide();
    powerBuy->hide();
    paramShow->show();
}

void desktop::on_pushButton_powerBuy_pressed()
{
    ScreenCnt = 0;
    setButtonStyle(3);
    menu->hide();
    controlStatus->hide();
    paramShow->hide();
    powerBuy->show();
}

void desktop::timeUpdate()
{

}
