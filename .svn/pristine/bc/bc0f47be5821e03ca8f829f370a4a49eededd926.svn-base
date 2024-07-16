#ifndef PASSAGE_H
#define PASSAGE_H

#include <QWidget>
#include "gprscfg.h"
#include "ethernetcfg.h"


namespace Ui {
class Passage;
}

class Passage : public QWidget
{
    Q_OBJECT

public:
    explicit Passage(QWidget *parent = nullptr);
    ~Passage();

private slots:
	void on_pushButton_ethernet_clicked();
    void on_pushButton_gprs_clicked();


private:
    Ui::Passage *ui;

	void inintButtonStyle();
    void setButtonStyle(int buttonIndex);

    int lastMenuIndex;

	GprsCfg *gprscfg;
	EthernetCfg *ethernetcfg;
		
};

#endif // PASSAGE_H
