#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include"currentdata.h"
#include"historydata.h"
#include"paramset.h"
#include"mntinfo.h"
#include"sysmanage.h"

namespace Ui {
class Menu;
}

class Menu : public QWidget
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);
    ~Menu();

private slots:
	void on_pushButton_cd_clicked();
    void on_pushButton_hd_clicked();
	void on_pushButton_ps_clicked();
	void on_pushButton_mnt_clicked();
	void on_pushButton_sys_clicked();

private:
    Ui::Menu *ui;
	CurrentData *currentData;
    HistoryData *historydata;
	ParamSet *paramset;
	MntInfo *mntinfo;
	SysManage *sysmanage;

};

#endif // MENU_H
