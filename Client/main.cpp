#define SDL_MAIN_HANDLED // <--- 必须放在最第一行！

#include "playerdialog.h"
#include "Lobbydialog.h"
#include "NetWorkManager.h"

#include <QApplication>
#include<iostream>
using namespace std;

//// 必须加 extern "C"，因为 FFmpeg 是 C 语言写的
//extern "C" {
//    #include "libavcodec/avcodec.h"
//    #include "libavformat/avformat.h"
//    #include "libswscale/swscale.h"
//    #include "libavdevice/avdevice.h"
//}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


//    std::cout<<"Hello FFmpeg: "<<endl;
//    av_register_all();
//    unsigned version=avcodec_version();//编解码器codec
//    std::cout<<"version is: "<<version<<endl;

    //打开大厅
    LobbyDialog l;
    l.show();

    //网络初始化
    NetWorkManager& netManager=NetWorkManager::getinstance();
    netManager.init("192.168.43.14", 9999);

    //l.test();
    //大厅确定连接成功，发出信号获取卡片信息



    return a.exec();
}
