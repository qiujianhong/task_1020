#ifndef GPRSCFG_H
#define GPRSCFG_H

#include <QWidget>

namespace Ui {
class GprsCfg;
}

class GprsCfg : public QWidget
{
    Q_OBJECT

public:
    explicit GprsCfg(QWidget *parent = nullptr);
    ~GprsCfg();

private:
    Ui::GprsCfg *ui;
};

#endif // GPRSCFG_H
