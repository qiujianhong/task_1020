#include "passwordset.h"
#include "ui_passwordset.h"
#include<QMessageBox>
#include <QImage>
#include <QPalette>


#include "framecomm.h"

#include "storage.h"  
#include"extern.h"


PasswordSet::PasswordSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PasswordSet)
{
    ui->setupUi(this);

	QImage image1;
    image1.load(":/new/imgs/images/password (1).png");
    QImage image2 = image1.scaled(631, 310);

    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(image2));
    this->setPalette(palette);

	int nRet = read_pdata_xram(GUI_PRIVATE_DATA, password, PRIVATE_PASSWORD, PASSWORD_LEN);
    if (nRet < 0)
    {
        memcpy(password, PASSWORD_STR, PASSWORD_LEN + 1);
    }

	focusLineEditName = 0;
}

PasswordSet::~PasswordSet()
{
    delete ui;
}

void PasswordSet::accepEditTextValue()
{
    switch(focusLineEditName)
    {
    case 1:
        ScreenCnt = 0;
        ui->lineEdit_old->setText(m_inputOld);
        break;
    case 2:
        ScreenCnt = 0;
        ui->lineEdit_new1->setText(m_inputNew1);
        break;
	case 3:
        ScreenCnt = 0;
        ui->lineEdit_new2->setText(m_inputNew2);
        break;
    default:
        break;
    }
}

bool PasswordSet::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type()==QEvent::FocusIn)
    {
        if (watched==ui->lineEdit_old)
        {
            focusLineEditName = 1;
            m_inputOld = ui->lineEdit_old->text();
            
            return QWidget::eventFilter(watched,event);
        }

        if (watched==ui->lineEdit_new1)
        {
            focusLineEditName = 2;
            m_inputNew1 = ui->lineEdit_new1->text();
            
            return QWidget::eventFilter(watched,event);
        }

		if (watched==ui->lineEdit_new2)
        {
            focusLineEditName = 3;
            m_inputNew2 = ui->lineEdit_new2->text();
            
            return QWidget::eventFilter(watched,event);
        }
    }
    return QWidget::eventFilter(watched,event);
}


//保存设置
void PasswordSet::on_pushButton_save_clicked()
{
    ScreenCnt = 0;
	if(ui->lineEdit_old->text().length() != PASSWORD_LEN)
	{
//		GUI_FMT_DEBUG("m_inputOld.length(%s) %d::PASSWORD_LEN%d\n", m_inputNew1.toLatin1().data(), m_inputOld.length() , PASSWORD_LEN);
		QMessageBox::warning(this,tr("警告"),tr("旧密码长度错误"),QMessageBox::Yes);
		return ;
	}

	if(ui->lineEdit_new1->text().length() != PASSWORD_LEN)
	{
		QMessageBox::warning(this,tr("警告"),tr("密码长度错误"),QMessageBox::Yes);
		return ;
	}

	if(ui->lineEdit_new2->text().length() != PASSWORD_LEN)
	{
		QMessageBox::warning(this,tr("警告"),tr("再次输入密码长度错误"),QMessageBox::Yes);
		return ;
	}

	if(ui->lineEdit_old->text() != QString::fromLocal8Bit(password))
	{
        QMessageBox::warning(this,tr("警告"),tr("旧密码输入错误"),QMessageBox::Yes);
		return ;
    }

	if(ui->lineEdit_new1->text() != ui->lineEdit_new2->text())
	{
        QMessageBox::warning(this,tr("警告"),tr("两次输入的密码不一致"),QMessageBox::Yes);
		return ;
    }

	int nRet = write_pdata_xram(GUI_PRIVATE_DATA, ui->lineEdit_new1->text().toLatin1().data(), PRIVATE_PASSWORD, PASSWORD_LEN);
    if (nRet >= 0)
    {
    	memcpy(password, ui->lineEdit_new1->text().toLatin1().data(), PASSWORD_LEN);
        QMessageBox::warning(this,tr("提示"),tr("密码修改成功"),QMessageBox::Yes);
    }
    else
    {
        QMessageBox::warning(this,tr("警告"),tr("修改密码失败"),QMessageBox::Yes);
    }
//    this->deleteLater();
}

void PasswordSet::mousePressEvent(QMouseEvent *event)
{
    if(event == NULL)
	{
		GUI_FMT_DEBUG("Login::mousePressEvent\n");//for removing warning!
	}
    ScreenCnt = 0;
}


