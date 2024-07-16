#ifndef METER_H
#define METER_H

#include <QWidget>

namespace Ui {
class Meter;
}

class Meter : public QWidget
{
    Q_OBJECT

public:
    explicit Meter(QWidget *parent = nullptr);
    ~Meter();

private:
    Ui::Meter *ui;
};

#endif // METER_H
