#ifndef LOBBYDIALOG_H
#define LOBBYDIALOG_H

#include <QDialog>
#include"networkmanager.h"
#include"Net.h"
#include"roomitem.h"
#include<QListWidgetItem>
#include"playerdialog.h"
#include<QMap>
#include"roomitem.h"
#include<QList>

namespace Ui {
class LobbyDialog;
}

class LobbyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LobbyDialog(QWidget *parent = nullptr);
    ~LobbyDialog();

    void test();

public slots:
    void slot_GetRoomCard(P_ROOMCARD info);

    void slot_onItemDoubleClicked(QListWidgetItem* item);//固定参数


private:
    Ui::LobbyDialog *ui;
    
    QMap<uint32_t,PlayerDialog*>m_playManager;
};

#endif // LOBBYDIALOG_H
