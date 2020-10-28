#include "mainwindow.h"
#include "udpReceiver.h"
#include "infodialog.h"

#include <QApplication>
#include <QFontDatabase>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDesktopWidget>

MainWindow *w;
QFile *file = new QFile(QDir::currentPath()+"/"+"log");//此路径下没有就会自己创建一个

extern InfoDialog* pInfo;
extern InjectDialog* pInject;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    UdpReceiver ur;

    //QSS
    QFile file("../test_windows/css.qss");
    qDebug() << file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qDebug() << styleSheet;
    qApp->setStyleSheet(styleSheet);

    //chinese support
    int id = QFontDatabase::addApplicationFont("../DroidSansFallback.ttf");
    QString msyh = QFontDatabase::applicationFontFamilies (id).at(0);
    QFont font(msyh,10);
    qDebug()<<msyh<<endl;
    font.setPointSize(12);

    w = new MainWindow();
    w->setFont(font);
    w->move(0,0);
    w->show();

    pInfo = new InfoDialog();
    pInfo->setFont(font);
    pInfo->move(w->width(),520);
    pInfo->addCloseButton();
    pInfo->show();

    pInject = new InjectDialog();
    pInject->move(w->width(),0);
    pInject->show();

    return a.exec();
}
