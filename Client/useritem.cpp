#include "useritem.h"
#include "ui_useritem.h"

UserItem::UserItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserItem)
{
    ui->setupUi(this);
}

UserItem::~UserItem()
{
    delete ui;
}

void UserItem::setcard(const char* name)
{
    qDebug()<<"UserItem::setcard：绘制用户"<<endl;
    ui->lb_name->setText(QString::fromUtf8(name));
}
