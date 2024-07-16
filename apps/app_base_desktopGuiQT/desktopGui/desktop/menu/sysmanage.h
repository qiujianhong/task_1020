#ifndef SYSMANAGE_H
#define SYSMANAGE_H

#include <QWidget>

namespace Ui {
class SysManage;
}

class SysManage : public QWidget
{
    Q_OBJECT

public:
    explicit SysManage(QWidget *parent = nullptr);
    ~SysManage();

private slots:
    void on_pushButton_close_clicked();

private:
    Ui::SysManage *ui;
};

#endif // SYSMANAGE_H
