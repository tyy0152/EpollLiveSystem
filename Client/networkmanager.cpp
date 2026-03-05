#include "networkmanager.h"
#include<QDebug>

NetWorkManager::NetWorkManager(QObject *parent)
{
    m_socket=new QTcpSocket(this);

    m_socket->setProxy(QNetworkProxy::NoProxy);

    connect(m_socket,&QTcpSocket::connected,this,[=](){
        qDebug()<<"NetWorkManager::连接成功"<<endl;
    });


    // 放在构造函数里
    connect(m_socket, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error)
            , this, [=](QAbstractSocket::SocketError error){
        qDebug() << "连接具体错误信息：" << m_socket->errorString();
        qDebug() << "错误代码：" << error;
    });


    //readyRead只通知，来一次通知一次，不保存，slot_HandleData保存
    //如果m_socket接收到数据，会触发readyRead信号，让this收货，触发this中的slot_ReadReady槽函数
    connect(m_socket,&QTcpSocket::readyRead,this,&NetWorkManager::slot_HandleData);

    m_picManager = new QNetworkAccessManager(this);

}


NetWorkManager& NetWorkManager::getinstance()
{
    static NetWorkManager _instance;
    return _instance;
}


void NetWorkManager::init(QString ip,int port)
{
    //socket处于未连接状态
    if(m_socket->state()==QAbstractSocket::UnconnectedState){
        m_socket->connectToHost(ip,port);
    }

}

// 提供一个全局的下载接口
QNetworkAccessManager* NetWorkManager::getNetworkManager() {
    return m_picManager;
}

void NetWorkManager::GetRooMIn_RQ(uint32_t roomid)
{
    //根据找到id的房间信息的请求,M_GETROOM_RQ
    NetPack pack;
    pack.msgType=M_ROOMIN_RQ;
    pack.packSize=0;
    pack.roomID=roomid;
    pack.userID=1;//todo:最好填写，现在还没分配，暂时写0

    //将请求发送给服务端
    m_socket->write((char*)&pack,sizeof(pack));
    m_socket->flush();
    qDebug()<<"NetPack大小"<<sizeof(NetPack)<<endl;

    qDebug()<<"双击卡片，触发进房"<<endl;
}


void NetWorkManager::slot_HandleData()
{
    //QByteArray适合字节流数据
    //m_buffer.append=m_socket->readAll();
    m_buffer.append(m_socket->readAll());//保证不会覆盖原来的缓冲区
    qDebug()<<"网络管理员收到数据包，大小： "<<m_buffer.size();

    //防止有多个包
    while(m_buffer.size() >= sizeof(NetPack)){
    //如果缓冲区空，说明包处理完了，或者本来就没数据，直接退出
        if(m_buffer.size()<=0) {
                return;
        }

//       //没有包头
//        if(m_buffer.size()<sizeof (NetPack)){
//                cout<<"包头不完整"<<endl;
//                return;//m_buffer必须持久，否则这里会丢失数据
//        }

        NetPack* pHead=(NetPack*)m_buffer.data();
        int newSize=m_buffer.size();
        int bodySize=pHead->packSize;
        int allSize=pHead->packSize+sizeof(NetPack);

        //包头+包体长度<=实际缓冲区长度，证明包传输完整
        if(allSize>newSize){
                cout<<"包数据不完整"<<endl;
                return;
        }


        qDebug() << "NetWorkManager::slot_HandleData：开始解析包，类型：" << pHead->msgType << " 期待大小：" << allSize << " 缓冲区大小：" << m_buffer.size()<<endl;
        switch(pHead->msgType){
        case M_ROOM_OUT:
        break;
        case M_ROOMCARD://大厅房间卡片回复
        {
            //服务器一次性发了几个卡片信息，需要客户端分开处理

            //接收到一个房间的信息，提醒大厅界面绘制
            //tip:如果这里读取的数据，不是所有的房间，需要再次读取，怎么处理？
            qDebug()<<"收到卡片信息"<<endl;
            int count=bodySize/sizeof(P_ROOMCARD);

            //等待所有房间传完再读取
            if(count*sizeof(P_ROOMCARD)>newSize-sizeof(NetPack)){
                //还没传完,再读//断点续传？
                qDebug() << "房间卡片 数据包未传完" << endl;
                return;
            }

            P_ROOMCARD* info=(P_ROOMCARD*)(m_buffer.data()+sizeof(NetPack));
            for(int i=0;i<2;i++){
                emit sig_GetRoomCard(info[i]);
            }
            break;
        }
        case M_ROOMIN_RS://进房回复
        {
            qDebug()<<"NetWorkManager::slot_HandleData：进房了,绘制房间"<<endl;
            P_ROOMIN_RS* rs=(P_ROOMIN_RS*)(m_buffer.data()+sizeof(NetPack));
            int count=rs->userCount;

            //等待所有用户传完再读取，现在的缓冲区大小减去固定包头和房信息包头，剩下是用户信息
            if(count*sizeof(P_USER_INFO)>newSize-sizeof(P_ROOMIN_RS)-sizeof(NetPack)){
                //还没传完,再读//断点续传？
                qDebug() << "房间用户 数据包未传完" << endl;
                return;
            }

            P_USER_INFO* peo=(P_USER_INFO*)(m_buffer.data()+sizeof(NetPack)+sizeof(P_ROOMIN_RS));
            QList<P_USER_INFO> users;
            for(int i=0;i<count;i++){
                users.append(peo[i]);
            }
            int id=rs->roomID;

            //不能直接传递指针，所以创建一个临时变量，进行值传递，保证安全
            //获得房内信息，直接转发给对应的直播间playerdialog
            //cout<<"给直播间发送房间信息"<<endl;
            emit sig_GetRoomIn(id,*rs,users);
            break;
        }
        case M_VIDEO_STREAM://4.直播内容——视频
        {
                break;
        }
        case M_AUDIO_STREAM://5.直播内容——音频
        {
                break;
        }
        case M_SEND_WORD:
        {
            //收到消息广播，显示到聊天上
            cout<<"NetWorkManager::slot_HandleData：收到消息"<<endl;
            P_SEND_WORD* send=(P_SEND_WORD*)(m_buffer.data()+sizeof(NetPack));

            int roomID=pHead->roomID;

            //格式化聊天内容
            QString chatName=QString::fromUtf8(send->senderName);
            QString chatLine=QString::fromUtf8(send->chatWord);

            //发送信号给playerdialog
            emit sig_ReceWord(roomID,chatName,chatLine);
                break;
        }
        case M_SEND_EMOJI://7.发送信息（文字或固定表情）
        {
                break;
        }
        case M_GET_ROOMLIST_RQ://8.客户端请求列表
        {
                break;
        }
        default:
                break;
        }

        //读完这个包，将其从缓冲区移除
        //data.erase(m_buffer.begin(), m_buffer.begin() + allSize);
        m_buffer.remove(0,allSize);
    }


}
