#ifndef HISTORYDATA_H
#define HISTORYDATA_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class HistoryData;
}

class HistoryData : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryData(QWidget *parent = nullptr);
    ~HistoryData();
	
private slots:
    void on_pushButton_close_clicked();
//	void on_pushButton_clicked();
//	void on_pushButton_2_clicked();

private:
    void timeout();

private:
    Ui::HistoryData *ui;
	unsigned int meterId;
	unsigned int dataType;
	QTimer *pTimer;
};

#endif // HISTORYDATA_H
