#include "desktop.h"
#include <QResource>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "msgthread.h"

int ScreenCnt = 0;			//��������
unsigned int gMqttIp;     	//mqtt IP�Ͷ˿�
unsigned short gMqttPort;

#include "common.h"

int main(int argc, char *argv[])
{
    int ret = 0;

    ret = init_main(argc, argv);
    if(ret < 0)
    {
        return -1;
    }

    qputenv("QT_IM_MODULE", QByteArray("tgtsml"));		//�������
    QApplication a(argc, argv);

    QResource::registerResource(a.applicationDirPath() + "/skin.rcc");      //����ͼƬ��Դ�ļ�

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "desktopGui_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

//    MsgThread *msgThread = new MsgThread();
//    msgThread->start();

    desktop w;
    w.show();
    return a.exec();
}
