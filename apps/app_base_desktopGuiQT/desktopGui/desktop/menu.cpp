#include "menu.h"
#include "ui_menu.h"
#include"extern.h"

Menu::Menu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);
}

Menu::~Menu()
{
    delete ui;
}

//实时数据
void Menu::on_pushButton_cd_clicked()
{
    ScreenCnt = 0;
    currentData = new CurrentData(this);
//    currentData->move(120,0);
    currentData->show();
}


//历史数据
void Menu::on_pushButton_hd_clicked()
{
    ScreenCnt = 0;
    historydata = new HistoryData(this);
//    historydata->move(120,0);
    historydata->show();
}

//参数设置
void Menu::on_pushButton_ps_clicked()
{
    ScreenCnt = 0;
    paramset = new ParamSet(this);
//    paramset->move(120,0);
    paramset->show();
}

//终端信息
void Menu::on_pushButton_mnt_clicked()
{
    ScreenCnt = 0;
    mntinfo = new MntInfo(this);
//    mntinfo->move(120,0);
    mntinfo->show();
}

//系统管理
void Menu::on_pushButton_sys_clicked()
{
    ScreenCnt = 0;
    sysmanage = new SysManage(this);
//    sysmanage->move(120,0);
    sysmanage->show();
}

