#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include<QTcpSocket>
#include<QString>
#include"Net.h"
#include<iostream>

//下载图片
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkProxy>
#include<QList>
#include<QString>

using namespace::std;

class NetWorkManager:public QObject
{
        Q_OBJECT

public:
    static NetWorkManager& getinstance();

    void init(QString ip,int port);

    QTcpSocket* getsock(){return m_socket;}

    void slot_HandleData();

    QByteArray& getbuff(){return m_buffer;}

    QNetworkAccessManager* getNetworkManager();

    void GetRooMIn_RQ(uint32_t id);

signals:
    void sig_GetRoomCard(P_ROOMCARD info);

    void sig_GetRoomIn(int,P_ROOMIN_RS,QList<P_USER_INFO>);

    void sig_ReceWord(int,QString,QString);
private:
    QTcpSocket* m_socket;

    explicit NetWorkManager(QObject *parent = nullptr);
    NetWorkManager(const NetWorkManager&)=delete;

    QByteArray m_buffer;

    QNetworkAccessManager* m_picManager;

};

#endif // NETWORKMANAGER_H
