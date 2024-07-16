#ifndef DESKTOP_H
#define DESKTOP_H

#include <QWidget>
#include <QDateTime>
#include "menu.h"
#include "paramshow.h"
#include "powerbuy.h"
#include "controlstatus.h"

QT_BEGIN_NAMESPACE
namespace Ui { class desktop; }
QT_END_NAMESPACE

class desktop : public QWidget
{
    Q_OBJECT

public:
    desktop(QWidget *parent = nullptr);
    ~desktop();

private slots:
    void on_pushButton_menu_pressed();
    void on_pushButton_powerBuy_pressed();
    void on_pushButton_controlStatus_pressed();
    void on_pushButton_paramShow_pressed();
    void timeUpdate();


private:
    Ui::desktop *ui;
    void inintButtonStyle();
    void setButtonStyle(int buttonIndex);

    Menu *menu;
    ControlStatus *controlStatus;
    ParamShow *paramShow;
    PowerBuy *powerBuy;

    int Ret;
    int g_nUpdate_Flag;
    int lastMenuIndex;

    QDateTime CurrentTime;
    QString Timestr;
    QString proportion;
};
#endif // DESKTOP_H
