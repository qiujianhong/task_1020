#ifndef CONTROLSTATUS_H
#define CONTROLSTATUS_H

#include <QWidget>

namespace Ui {
class ControlStatus;
}

class ControlStatus : public QWidget
{
    Q_OBJECT

public:
    explicit ControlStatus(QWidget *parent = nullptr);
    ~ControlStatus();

private:
    Ui::ControlStatus *ui;
};

#endif // CONTROLSTATUS_H
