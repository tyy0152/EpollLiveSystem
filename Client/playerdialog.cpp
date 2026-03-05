#include "playerdialog.h"
#include "ui_playerdialog.h"
#include"Net.h"
#include <QNetworkProxy>

//#define _DEF_FILE_PATH "A:/TyyVideo/mmexport1685425970543.mp4"
#define _DEF_FILE_PATH "http://vjs.zencdn.net/v/oceans.mp4"
//#define _DEF_FILE_PATH "http://devimages.apple.com/iphone/samples/bipbop/bipbopall.m3u8"
//#define _DEF_FILE_PATH "https://www.w3school.com.cn/i/horse.mp3"//纯音频
//#define _DEF_FILE_PATH "http://live.xmcdn.com/live/1005/64.m3u8"//纯音频


PlayerDialog::PlayerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PlayerDialog)
{
    ui->setupUi(this);

    m_socket=NetWorkManager::getinstance().getsock();


    //进房信号连接
    connect(&NetWorkManager::getinstance(),&NetWorkManager::sig_GetRoomIn,
           this,&PlayerDialog::slot_GetRoomIn);

    //转发信息连接
    connect(&NetWorkManager::getinstance(),&NetWorkManager::sig_ReceWord,
            this,&PlayerDialog::slot_ReceWord);
}

PlayerDialog::~PlayerDialog()
{
    delete ui;

}


//void PlayerDialog::on_pb_start_clicked()
//{
//    //点击就开始播放---从音频文件里面获取图片进行播放
//    m_myplayer->start();
//}

//void PlayerDialog::slot_setImage(QImage image)
//{
//    //pixmap（用于显示，有硬件支持渲染） image(内存里数据转化）
//    //image进行缩放处理
//    QPixmap pixmap=QPixmap::fromImage(image.scaled(ui->lb_show->size(),Qt::KeepAspectRatio));
//            //直接将image放入可能会不适配,KeepAspectRatio是保持原比例
//    ui->lb_show->setPixmap(pixmap);
//}

//qt线程QThread 定义子类 start() run()


void PlayerDialog::slot_HandleData()
{
    // 服务器一发数据过来，马上全盘接收
    QByteArray data = m_socket->readAll();

    m_buffer.append(data.data(),data.size());



}

void PlayerDialog::slot_GetRoomIn(int id,P_ROOMIN_RS rs,QList<P_USER_INFO> users)
{
    if(this->m_roomID!=id){
        cout<<"直播间：发送用户错误"<<endl;
        return;
    }
    qDebug() << "PlayerDialog::slot_GetRoomIn:直播间: "<< rs.roomID <<"准备绘制"<<endl ;
    //将图片信息展示到ui上

    ui->lb_title->setText(QString::fromUtf8(rs.ownerName)+ "的直播间");

    ui->lw_user->clear();

    for(const auto& user:users){
        UserItem* card=new UserItem();
        card->setcard(user.name);

        // 把卡片塞进 QListWidget
        QListWidgetItem* item = new QListWidgetItem(ui->lw_user);
        item->setSizeHint(QSize(100, 60)); // 设置卡片的大小
        ui->lw_user->setItemWidget(item, card);
    }

}

void PlayerDialog::slot_ReceWord(int roomID, QString chatName, QString chatLine)
{
    if(this->m_roomID!=roomID){
        cout<<"直播间：发送用户错误"<<endl;
        return;
    }

    qDebug() << "PlayerDialog::slot_SendWord:直播间收到消息 "<<endl ;

    QString chatTalk="【"+chatName+"】: "+chatLine;
    QListWidgetItem* item=new QListWidgetItem(chatTalk,ui->lw_talk);
    item->setTextColor(Qt::black);

    ui->lw_talk->scrollToBottom();
}

//发送聊天框内容
void PlayerDialog::on_pb_send_clicked()
{
    qDebug() << "PlayerDialog::slot_SendWord:直播间发送消息 "<<endl ;

    QString con=ui->to_text->toPlainText();

    if(con.isEmpty())return;

    int allSize=sizeof(NetPack)+sizeof(P_SEND_WORD);
    vector<char> temp(allSize);

    NetPack* p1=(NetPack*)temp.data();
    p1->msgType=M_SEND_WORD;
    p1->packSize=sizeof(P_SEND_WORD);
    p1->userID=1;
    p1->roomID=m_roomID;

    P_SEND_WORD* send=(P_SEND_WORD*)(temp.data()+sizeof(NetPack));
    strcpy(send->chatWord,con.toUtf8().data());
    strcpy(send->senderName,"tan");

    m_socket->write(temp.data(),temp.size());
//    //格式化聊天内容
//    QString chatLine=QString("【tan】：%1").arg(con);

//    //本地上传
//    QListWidgetItem* item=new QListWidgetItem(chatLine,ui->lw_talk);
//    item->setTextColor(Qt::blue);

    //清空输入框,让光标重新回到输入框，方便直接打下一句话
    ui->to_text->clear();
    ui->to_text->setFocus();

    //自动滚到最下方最新消息
    ui->lw_talk->scrollToBottom();
}

