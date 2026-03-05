#ifndef NET_H
#define NET_H

#include <stdint.h>

#pragma pack(push, 1)//1字节对齐

#define MM_NAME_MAX         10         //名字最大长度
#define MM_USER_COUNT_MAX   100000     //直播间在线用户最大数十万
#define MM_WORD_SEND_MAX    100        //发送文字最大长度
#define MM_PAGE_SIZE_DEFAULT   20         // 默认分页数量
#define MM_ROOMT_MAX        30          //房间标题最大长度

#define MM_ROOMIN_RS_NO      0
#define MM_ROOMIN_RS_YES     1

#define MM_GETROOM_RS_NO     0
#define MM_GETROOM_RS_YES    1
#include<cstdint>
enum MsgType{

    //---基础事件---
    //1.进房请求//2.进房回复
    M_ROOMIN_RQ,
    M_ROOMIN_RS,

    M_ROOMCARD,//大厅卡片信息

    //3.退房
    M_ROOM_OUT,

    //---实时直播更新---
    //4.直播内容——视频
    M_VIDEO_STREAM,
    //5.直播内容——音频
    M_AUDIO_STREAM,

    //---用户交流---
    //6.发送信息（文字或固定表情）
    M_SEND_WORD,
    //7.发送信息（文字或固定表情）
    M_SEND_EMOJI,

    //---列表刷新---
    //分页拉取和状态同步
    //下拉刷新 (首页)：给我第 0 到 第 19 号房间。
    //一直下滑 (加载更多)：给我第 20 到 第 39 号房间。
    M_GET_ROOMLIST_RQ, // 客户端请求列表 (不管是下拉还是上滑，都用这个)
    M_GET_ROOMLIST_RS, // 服务器回复列表

};

struct NetPack{  //总包头
    uint32_t packSize;//包体总大小
    uint32_t msgType;//信息类型
    uint32_t roomID;//发给哪个房间
    uint32_t userID;//谁发的
};


//房间卡片信息
struct P_ROOMCARD{
    uint32_t roomID;//房间id
    uint32_t picID;//首页图片id
    char ownerName[MM_NAME_MAX];//房主名字
    char roomTitle[MM_ROOMT_MAX];//房间标题
//    uint32_t userCount;//在房人数，最大十万MM_USER_COUNT_MAX
//    int state; // 0=关闭, 1=开启
};

//用户卡片信息
struct P_USER_INFO{
    uint32_t userID;//用户id
    uint32_t isOwner;//是否是房主，房主1，普通用户0
    char name[MM_NAME_MAX];//用户名字
};


//----------------------------具体功能--------------------------------------

////1.进房请求——用户端点击后发送NetPack给服务端，服务端判断是否可以进去，返回rq
//uint32_t packSize=0;


//todo:进房是是在原窗口直接改变页面还是打开一个新的窗口？
////2.进房回复——返回是否可以加入直播间，
/// 入房成功——返回给我直播间信息，刷新在房内所有人的用户列表
struct P_ROOMIN_RS{
    bool result;//成功或失败
    uint32_t roomID;//房间id
    uint32_t ownerID;//房主id
    char ownerName[MM_NAME_MAX];//房主名字
    char roomTitle[MM_ROOMT_MAX];//房间标题
    uint32_t userCount;//在房人数，最大十万MM_USER_COUNT_MAX
    uint32_t state; // 0=关闭, 1=开启
    //所有在房用户信息//todo:所有用户的信息怎么写在这里？
    //todo:可不可以调用P_GET_USERLIST刷新观看用户列表?
    //UserInfo user;
    // 后面紧接着内存里存放：UserInfo[0] + UserInfo[1] + ... + UserInfo[n]
    // 读取时根据 userCount 循环读取
};

//struct P_USER_IN {
//    P_USER_INFO user; //谁进来了
//};

////8.刷新观看用户列表——todo：全量通知？增量通知？
//struct P_GET_USERLIST{
//    uint32_t userCount;//在房人数，最大十万MM_USER_COUNT_MAX
//    //所有在房用户信息//todo:所有用户的信息怎么写在这里？
//    //UserInfo user;
//    // 后面紧接着内存里存放：UserInfo[0] + UserInfo[1] + ... + UserInfo[n]
//    // 读取时根据 userCount 循环读取
//};

////3.退房——也要刷新用户列表
//struct P_USER_OUT {
//    uint32_t userId; //谁走了
//};

////---实时直播更新---
////4.直播内容——视频
struct P_VIDEO_STREAM{
    int64_t timestamp; // (8字节) 极其重要！音画同步全靠它。对应 FFmpeg 的 pts。
    int32_t isKeyFrame;// (4字节) 是不是关键帧(I帧)？1=是，0=否。方便服务端做丢帧策略。
    int32_t width;     // (4字节) 宽 (可选，防止分辨率变了)
    int32_t height;    // (4字节) 高 (可选)

    //  这里不要定义 char data[...]！
    // 后面紧跟H264数据，我们在发包的时候手动拼上去。
};

////5.直播内容——音频
struct P_AUDIO_STREAM{
    int64_t timestamp;  // (8字节) 时间戳 (PTS)，依然极其重要，用来跟视频对齐

    // 下面这些是音频特有的参数 (相当于音频的“宽和高”)
    // 虽然通常整个直播过程中这些不会变，但为了安全，每帧都带上或者至少第一帧带上
    int32_t sampleRate; // (4字节) 采样率 (例如 44100, 48000)
    int32_t channels;   // (4字节) 通道数 (1=单声道, 2=立体声)
    int32_t sampleSize; // (4字节) 采样大小 (例如 16 bit)

    // 音频裸数据 (PCM/AAC) 紧贴在结构体后面
};

////---用户交流---//需要有广播，一个人发的，所有人都要知道
////6.发送信息（文字或固定表情）
struct P_SEND_WORD{
    char senderName[MM_NAME_MAX];
    char chatWord[MM_WORD_SEND_MAX];
};

////7.发送信息（文字或固定表情）
//uint32_t packSize=sizeof(M_SEND_EMOJI);
//uint32_t msgType=M_SEND_EMOJI;
struct P_SEND_EMOJI{
    int emojiID;//发送默认表情包序号
};

////---列表刷新---

// 客户端请求列表 (不管是下拉还是上滑，都用这个)
//uint32_t packSize=sizeof(P_GETROOM_RQ);
//uint32_t msgType=M_GET_ROOMLIST_RQ;
//struct P_GETROOM_RQ{
//    int beginIndex;//从第几个开始拿取//todo：那客户端需要记录上次从哪里开始拿的？
//    int count;//拿多少个？todo：默认8个一次
//};


// 服务器回复列表
//uint32_t packSize=sizeof(P_GETROOM_RS);
//uint32_t msgType=M_GET_ROOMLIST_RS;
//struct P_GETROOM_RS{
////    int roomID1;//
////    int roomID2;//拉去从id1到id2的房间，例如9-16号
//    //某个房间不直播后会断号，不存在10、11直播间
//    int result;//0失败，1成功
//    int totalCount; // 服务器总共有多少个房间，超过了就无法刷新了
//    int count;//为了计算后面应该读取多少个卡片信息
//    // 后面紧跟P_ROOM_INFO
//};


#pragma pack(pop)   // 恢复默认对齐


#endif // NET_H




/*直播流建立流程:

进房握手：客户端发送 M_GETROOM_RQ。

获取环境：服务端回复 M_GETROOM_RS（包含房间信息+观众列表）。客户端弹出 PlayerDialog 并绘制 UI。

启动流传输：服务端在确认你进房后，开始源源不断地向你的 FD 发送 M_VIDEO_STREAM 和 M_AUDIO_STREAM 包。

持续消费：客户端的 slot_HandleData 收到视频包后，不转发给对话框，而是直接送入**解码器（如 FFmpeg）**或显示组件。*/
