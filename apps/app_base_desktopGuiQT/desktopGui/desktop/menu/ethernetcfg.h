#ifndef ETHERNETCFG_H
#define ETHERNETCFG_H

#include <QWidget>

namespace Ui {
class EthernetCfg;
}

class EthernetCfg : public QWidget
{
    Q_OBJECT

public:
    explicit EthernetCfg(QWidget *parent = nullptr);
    ~EthernetCfg();

private:
    Ui::EthernetCfg *ui;
};

#endif // ETHERNETCFG_H
