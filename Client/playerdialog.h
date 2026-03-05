#ifndef PLAYERDIALOG_H
#define PLAYERDIALOG_H

#include<string>
#include<iostream>
#include <QDialog>
#include"videoplayer.h"

#include<QTcpSocket>
#include"Net.h"
#include"networkmanager.h"
#include"useritem.h"
#include<QList>

using namespace::std;

QT_BEGIN_NAMESPACE
namespace Ui { class PlayerDialog; }
QT_END_NAMESPACE

class PlayerDialog : public QDialog
{
    Q_OBJECT

public:
    PlayerDialog(QWidget *parent = nullptr);
    ~PlayerDialog();

    // 增加一个公开函数来设置 ID
    void setRoomID(uint32_t id) { this->m_roomID = id; }

public slots:

    void slot_HandleData();//接收服务器消息

    void slot_GetRoomIn(int,P_ROOMIN_RS,QList<P_USER_INFO>);

    void slot_ReceWord(int,QString,QString);
private slots:
    void on_pb_send_clicked();

private:
    Ui::PlayerDialog *ui;

    uint32_t m_roomID;

    //videoPlayer* m_myplayer;

    QTcpSocket* m_socket;

    string m_buffer;
};
#endif // PLAYERDIALOG_H
