#ifndef ROOMITEM_H
#define ROOMITEM_H

#include <QDialog>
#include"Net.h"
#include"networkmanager.h"

#include <QPainter>
#include <QStyleOption>

namespace Ui {
class RoomItem;
}

class RoomItem : public QWidget
{
    Q_OBJECT

public:
    explicit RoomItem(QWidget *parent = nullptr);
    ~RoomItem();

    void setcard(P_ROOMCARD info);

    uint32_t getRoomID(){return m_roomID;}

private:
    Ui::RoomItem *ui;

    uint32_t m_roomID;
protected:
    void paintEvent(QPaintEvent *event) override; // 声明绘画事件
};

#endif // ROOMITEM_H
