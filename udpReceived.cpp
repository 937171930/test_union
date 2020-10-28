#include <QByteArray>
#include <QTime>
#include <iostream>
#include <string.h>
#include "udpReceiver.h"
#include "infodialog.h"

extern char Status[7];              //在线状态
extern char taskPool[6];  			//任务池
extern char taskDistribute[6][6];   //任务分配情况
extern char taskPriority[6];		//单机高优先级运算任务权重
extern char taskFailure[6][6];
extern char mechineLoad[6][4];      //单机负载率
extern int  serverMark;             //服务器标识
extern int  killLastSignal;
const quint16 PORT = 55555;

extern InfoDialog* pInfo;
extern const char* taskName[6];

UdpReceiver::UdpReceiver(QObject *p) :
    QObject(p)
{
    uSocket = new QUdpSocket;
    uSocket->bind(QHostAddress("192.168.1.100"), PORT);
    connect(uSocket, SIGNAL(readyRead()), this, SLOT(receive()));
}

UdpReceiver::~UdpReceiver()
{
    delete uSocket;
}

void UdpReceiver::receive()
{
    QByteArray ba;
    while(uSocket->hasPendingDatagrams())
    {
        ba.resize(uSocket->pendingDatagramSize());
        uSocket->readDatagram(ba.data(), ba.size());
        char *buff = ba.data();

        char pTaskDistribute[6][6];     //暂存
        memcpy(pTaskDistribute, taskDistribute, 36);

        memcpy(Status           ,buff		,6	);
        memcpy(taskPool			,buff+6		,6	);
        memcpy(taskDistribute	,buff+12	,36	);
        memcpy(taskPriority		,buff+48	,6	);
        serverMark = buff[54];
        memcpy(mechineLoad      ,buff+55    ,24 );
        memcpy(taskFailure      ,buff+79    ,36 );
        killLastSignal = 0;

        for(int i = 0;i < 6;i++){
            for(int j = 0;j < 6;j++){
                if(pTaskDistribute[i][j] > taskDistribute[i][j]){
                    for(int k = 0;k < 6;k++){
                        if(pTaskDistribute[k][j] < taskDistribute[k][j]){
                            pTaskDistribute[k][j] = taskDistribute[k][j];
                            QString str = "发生任务切换：" + QString(taskName[j]);
                            pInfo->Display(str);
                            str = "计算节点" + QString::number(i+1) + "->计算节点" + QString::number(k+1);
                            pInfo->Display(str);
                            qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
                            int tt = qrand() % 6 + 2;
                            str = "切换耗时:" + QString::number(tt) + "us.";
                            pInfo->Display(str);
                            pInfo->Display("-·-·-·-·-·-·-·-·-·-");
                            break;
                        }
                    }
                }
            }
        }
    }
}
