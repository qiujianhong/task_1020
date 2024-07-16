#ifndef CURRENTDATA_H
#define CURRENTDATA_H

#include <QWidget>
#include <QTimer>


namespace Ui {
class CurrentData;
}

class CurrentData : public QWidget
{
    Q_OBJECT

public:
    explicit CurrentData(QWidget *parent = nullptr);
    ~CurrentData();

private slots:
    void on_pushButton_close_clicked();

private:
    void timeout();
	
private:
    Ui::CurrentData *ui;
    QTimer *pTimer;
};

#endif // CURRENTDATA_H
