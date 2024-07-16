#ifndef PASSWORDSET_H
#define PASSWORDSET_H

#include <QWidget>
#include "common.h"


namespace Ui {
class PasswordSet;
}

class PasswordSet : public QWidget
{
    Q_OBJECT

public:
    explicit PasswordSet(QWidget *parent = nullptr);
    ~PasswordSet();

	int focusLineEditName;
    void mousePressEvent(QMouseEvent *event);

private slots:
	void on_pushButton_save_clicked();
	void accepEditTextValue();
	bool eventFilter(QObject *,QEvent *);

private:
    Ui::PasswordSet *ui;

	QString m_inputOld;
	QString m_inputNew1;
	QString m_inputNew2;

	char password[PASSWORD_LEN+1];
};

#endif // PASSWORDSET_H
