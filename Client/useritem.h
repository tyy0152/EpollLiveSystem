#ifndef USERITEM_H
#define USERITEM_H

#include <QDialog>
#include<QString>
#include"Net.h"
#include<QDebug>

namespace Ui {
class UserItem;
}

class UserItem : public QWidget
{
    Q_OBJECT

public:
    explicit UserItem(QWidget *parent = nullptr);
    ~UserItem();

    void setcard(const char* name);
private:
    Ui::UserItem *ui;
};

#endif // USERITEM_H
