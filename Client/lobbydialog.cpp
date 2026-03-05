#include "lobbydialog.h"
#include "ui_lobbydialog.h"

LobbyDialog::LobbyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LobbyDialog)
{
    ui->setupUi(this);

    ui->lw_roomList->clear();

    // 1. 设置为图标模式，支持横向平铺
    ui->lw_roomList->setViewMode(QListView::IconMode);

    // 2. 关键！锁定网格大小，必须略大于你的 sizeHint(400, 290)
    // 这样每个房间都会像磁铁一样被吸进这个固定大小的坑位里，绝不会缩小
    ui->lw_roomList->setGridSize(QSize(410, 300));

    // 3. 设置调整模式，确保增加卡片或窗口缩放时会自动重新排列
    ui->lw_roomList->setResizeMode(QListView::Adjust);

    // 4. 设置间距，防止卡片贴在一起
    ui->lw_roomList->setSpacing(10);

    //连接大厅卡片信息信号
    connect(&NetWorkManager::getinstance(),&NetWorkManager::sig_GetRoomCard,
            this,&LobbyDialog::slot_GetRoomCard);
    
    //双击打开卡片信号,房间列表发出信号，大厅窗口接收信号
    connect(ui->lw_roomList,&QListWidget::itemDoubleClicked,
            this,&LobbyDialog::slot_onItemDoubleClicked);

}

LobbyDialog::~LobbyDialog()
{
    delete ui;
}

//一个房间发一个信号处理一个卡片
void LobbyDialog::slot_GetRoomCard(P_ROOMCARD info)
{
    qDebug() << "大厅收到房间卡片: "<< info.roomID<<"准备绘制"<<endl ;
    //绘制房间卡片
    //1.创建卡片实例

    RoomItem* roomcard=new RoomItem();

    //2.将数据显示到卡片上
    roomcard->setcard(info);

    //3.将卡片放入大厅
    QListWidgetItem* item=new QListWidgetItem(ui->lw_roomList);
    item->setSizeHint(QSize(400,290));

    ui->lw_roomList->setItemWidget(item,roomcard);

}



void LobbyDialog::slot_onItemDoubleClicked(QListWidgetItem *item)
{
    //接收到双击卡片的信号，先找到是哪个房间，
    //然后发送请求该房间信息的包给服务端，
    //接着打开新窗口

    //安全类型转换qobject_cast<RoomItem*>，获得被双击过的卡片
    RoomItem* card=qobject_cast<RoomItem*>(ui->lw_roomList->itemWidget(item));

    if(card){
        uint32_t roomid=card->getRoomID();

        PlayerDialog* player=new PlayerDialog(this);

        player->setAttribute(Qt::WA_DeleteOnClose);//窗口关闭时自动销毁

        //player窗口关闭，发出信号，lobby接收到，执行slot,删掉map里的内容
        connect(player,&PlayerDialog::destroyed,this,[=](){
            m_playManager.remove(roomid);
        });



        m_playManager[roomid]=player;
        player->setRoomID(roomid);
        player->show();

        //发送给服务端
        NetWorkManager::getinstance().GetRooMIn_RQ(roomid);
    }
}



//void LobbyDialog::test(){
//    ui->lw_roomList->clear(); // 先清空 UI 里的那些“新建项目”

//    int roomc=2;
//    int size=roomc*sizeof(P_ROOMCARD);
//    int allsize=sizeof(NetPack)+size;

//    QByteArray tempData(allsize,0);
//    char* buff=tempData.data();

//    NetPack* p=(NetPack*)buff;
//    p->msgType=M_ROOMCARD;
//    p->packSize=size;


//    P_ROOMCARD* room=(P_ROOMCARD*)(buff+sizeof(NetPack));
//    strcpy(room->OwnerName,"小紫");
//    strcpy(room->RoomTitle,"游戏直播");
//    room->picID=1;
//    room->roomID=333;

//    strcpy(room[1].OwnerName,"小buafdfsd");
//    strcpy(room[1].RoomTitle,"kafei直播");
//    room[1].picID=2;
//    room[1].roomID=344;


//    NetWorkManager::getinstance().getbuff().append(tempData);
//    NetWorkManager::getinstance().slot_HandleData();

//}
