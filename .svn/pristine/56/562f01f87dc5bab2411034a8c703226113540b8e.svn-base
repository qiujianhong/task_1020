#ifndef MNTCOMSET_H
#define MNTCOMSET_H

#include <QWidget>
#include <QTimer>


namespace Ui {
class MntComSet;
}

class MntComSet : public QWidget
{
    Q_OBJECT

public:
    explicit MntComSet(QWidget *parent = nullptr);
    ~MntComSet();

private slots:
    void on_pushButton_addr_clicked();
	void on_pushButton_ip_clicked();


private:
    void timeout();


private:
    Ui::MntComSet *ui;

	QTimer *pTimer;
};

#endif // MNTCOMSET_H
