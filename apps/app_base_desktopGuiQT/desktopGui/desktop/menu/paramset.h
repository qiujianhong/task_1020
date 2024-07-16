#ifndef PARAMSET_H
#define PARAMSET_H

#include <QWidget>
#include <QTimer>
#include "mntcomset.h"
#include "passage.h"
#include "meter.h"
#include "pulse.h"
#include "group.h"
#include "mnttime.h"
#include "passwordset.h"



namespace Ui {
class ParamSet;
}

class ParamSet : public QWidget
{
    Q_OBJECT

public:
    explicit ParamSet(QWidget *parent = nullptr);
    ~ParamSet();
	
private slots:
    void on_pushButton_close_clicked();
	void on_pushButton_mntcom_clicked();
	void on_pushButton_passage_clicked();
	void on_pushButton_meter_clicked();
	void on_pushButton_pulse_clicked();
	void on_pushButton_group_clicked();
	void on_pushButton_time_clicked();
	void on_pushButton_password_clicked();

private:
    void timeout();


private:
    Ui::ParamSet *ui;
	QTimer *pTimer;

	void inintButtonStyle();
    void setButtonStyle(int buttonIndex);

    int lastMenuIndex;

	MntComSet *mntcomset;
	Passage *passage;
	Meter *meter;
	Pulse *pulse;
	Group *group;
	MntTime *mnttime;
	PasswordSet *passwordset;
};

#endif // PARAMSET_H
