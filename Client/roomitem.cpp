#include "roomitem.h"
#include "ui_roomitem.h"

RoomItem::RoomItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoomItem)
{
    ui->setupUi(this);

    this->setObjectName("RoomItem");
    // 给 Widget 自己设置一个最小高度，防止被 QListWidget 压扁
    this->setMinimumSize(400, 290);
}

RoomItem::~RoomItem()
{
    delete ui;
}

void RoomItem::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this); // 👈 这一段是固定套路，复制即可
}

void RoomItem::setcard(P_ROOMCARD info)
{
    m_roomID=info.roomID;
    ui->lb_name->setText(QString::fromUtf8(info.ownerName));
    ui->lb_title->setText(QString::fromUtf8(info.roomTitle));

    //图片设置：所有图片放在服务器/var/www/images/目录下
    //文件名为 1.jpg, 2.jpg 等,根据info.picID评价路径
    //获取唯一的下载管理器
    QNetworkAccessManager* picManager = NetWorkManager::getinstance().getNetworkManager();
    // 1. 拼接 URL
//    QString url = QString("http://你的服务器IP/images/%1.jpg").arg(info.picID);
//    qDebug()<<url;

    QString image = QString(":/src/image/%1.jpg").arg(info.picID);
    qDebug()<<image;

    // 2. 异步请求图片
    //QNetworkReply* reply = picManager->get(QNetworkRequest(QUrl(url)));

//    // 注意：这里要 connect 具体的这个 reply，而不是 manager
//    connect(reply, &QNetworkReply::finished, [=]() {
//        if (reply->error() == QNetworkReply::NoError) {
//            //获得二进制数据
//            QByteArray bytes = reply->readAll();
//            QPixmap pix;
//            if(pix.loadFromData(bytes)) {
//                //将二进制处理为位图
//                ui->lb_pic->setPixmap(pix.scaled(ui->lb_pic->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
//            }
//        }

//        reply->deleteLater(); // 只删掉这次请求的任务，不删管理器
//    });
    // 使用 border-image 可以让图片自动缩放填充整个 Widget 区域
    // 同时给文字背景加一个从下往上的半透明黑色渐变，能让白字在浅色图上更清晰
    this->setStyleSheet(QString(
        "QWidget#RoomItem {"
        "   border-image: url(%1) 0 0 0 0 stretch stretch;"
        "   border-radius: 12px;"
        "}"
        "QLabel {"
        "   color: white;"               /* 1. 改为白色，对比度更高 */
        "   background: transparent;"
        "   font-weight: bold;"          /* 2. 全局加粗 */
        "   font-family: 'Microsoft YaHei';" /* 使用微软雅黑更美观 */
        "}"
        "#lb_title {"
        "   font-size: 26px;"            /* 3. 标题加大 */
        "   margin-bottom: 2px;"
        "   qproperty-alignment: 'AlignBottom | AlignLeft';"
        "}"
        "#lb_name {"
        "   font-size: 22px;"            /* 4. 名字稍小但也要清晰 */
        "   color: #eeeeee;"             /* 浅灰色区分 */
        "}"
    ).arg(image));

}
