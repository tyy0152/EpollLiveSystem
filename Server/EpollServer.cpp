#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/epoll.h>
#include<vector>
#include<unordered_map>
#include<algorithm>

using namespace::std;

#define PORT 9999
#define MAX_EVENTS 1000//max events

#include"Net.h"//引入协议头

// 核心数据结构：每个客户端fd对应一个专属的接收缓冲区（缓冲区隔离）
std::unordered_map<int,std::vector<char>> g_clientBuffer;
// 键：roomID, 值：该房间内所有客户端的 fd 列表,房间 ID -> 成员
std::unordered_map<int, std::vector<int>> g_roomMembers;

void setNonBlocking(int sock){
	int opts=fcntl(sock,F_GETFL);

	if(opts<0){
		perror("fcntl sock GETFL");
		exit(1);
	}

	opts=opts | O_NONBLOCK;
	if(fcntl(sock,F_SETFL,opts)<0){
		perror("fcntl sock SETFL");
		exit(1);
	}
}


//
void HandleData(std::vector<char>& pack,int client_fd){
        //防止缓冲区有多个包
	cout<<"NetPack大小:"<<sizeof(NetPack)<<endl;
        while(pack.size()>sizeof(NetPack)){
		cout<<"收到包大小"<<pack.size()<<endl;
                NetPack* p=(NetPack*)pack.data();

		int newSize=pack.size();
		int bodySize=p->packSize;
		int allSize=p->packSize+sizeof(NetPack);

		//包头+包体长度<=实际缓冲区长度，证明包传输完整
                if(allSize>newSize){
                        cout<<"包数据不完整"<<endl;
                        return;
                }

                switch(p->msgType){
//                case M_ROOMCARD://1.进房请求——没有包体
//                //回复P_ROOMIN_RS，给用户刷新ui界面列表
//		{
//			std::cout << "[业务处理] fd: " << client_fd << " 发来了大厅房间卡片请求!" << std::endl;
//			int roomc=2;
//			int size=roomc*sizeof(P_ROOMCARD);
//    			int allsize=sizeof(NetPack)+size;
//
//   			std::vector<char> tempData(allsize,0);
//    			char* buff=tempData.data();
//
//    			NetPack* p=(NetPack*)buff;
//    			p->msgType=M_ROOMCARD;
//    			p->packSize=size;
//
//
//    			P_ROOMCARD* room=(P_ROOMCARD*)(buff+sizeof(NetPack));
//    			strcpy(room->OwnerName,"小紫");
//    			strcpy(room->RoomTitle,"游戏直播");
//   		 	room->picID=1;
//    			room->roomID=333;
//
//    			strcpy(room[1].OwnerName,"小buafdfsd");
////    			strcpy(room[1].RoomTitle,"kafei直播");
//    			room[1].picID=2;
//   			room[1].roomID=344;
//
//			//发送数据
//			int sendRet = send(client_fd, temp, allsize, 0);
//			if (sendRet > 0) {
//				std::cout << ">> 成功发送房间信息，大小: " << sendRet << " 字节" << std::endl;
//			}//
//			//释放内存
//			delete[] temp;
//        		break;
//    		}
                case M_ROOM_OUT://3.退房——没有包体
	                std::cout << "[业务处理] fd: " << client_fd <<"退出房间"<<endl;
		break;
		case M_ROOMIN_RQ:
		{	
			//todo:该用户进房后，需添加到g_roomMembers[roomID]

			//todo:根据房间id，去数据库中查找对应信息，填写M_GETROOM_RS，发给该客户端
			//暂时不写数据库，假设一个数据,3个观众
			
			cout << "[业务处理] fd: " << client_fd << " 发>来进房请求, 房间ID: " << p->roomID << endl;

			int bodysize=3*sizeof(P_USER_INFO)+sizeof(P_ROOMIN_RS);
			int allsize=sizeof(NetPack)+bodysize;
			char* temp=new char[allsize];

			NetPack* pack1=(NetPack*)temp;
			pack1->msgType=M_ROOMIN_RS;
    			pack1->packSize=bodysize;
    			pack1->roomID=p->roomID;
   			pack1->userID=p->userID;

			//todo
			//g_roomMembers[pack1->roomID].push_back(client_fd);			
			auto& fds=g_roomMembers[pack1->roomID];
			if(std::find(fds.begin(),fds.end(),client_fd)==fds.end()){
				fds.push_back(client_fd);
			}

                        P_ROOMIN_RS* pack2 = (P_ROOMIN_RS*)(temp + sizeof(NetPack));
                        pack2->roomID = p->roomID;
			pack2->result=1;
                        pack2->ownerID = 233;
                        strcpy(pack2->ownerName, "xiaoxiaodefangjian");
                        pack2->userCount =3;
			
			//观众信息
			P_USER_INFO* userp=(P_USER_INFO*)(temp+sizeof(NetPack)+sizeof(P_ROOMIN_RS));
			userp[0].userID=13;
			userp[0].isOwner=0;
			strcpy(userp[0].name,"小易");

			userp[1].userID=14;
                        userp[1].isOwner=0;
                        strcpy(userp[1].name,"小ba");

			userp[2].userID=15;
                        userp[2].isOwner=0;
                        strcpy(userp[2].name,"小黑");

			send(client_fd,temp,allsize,0);
			delete[] temp;
			break;
		}
                case M_VIDEO_STREAM://4.直播内容——视频
                {
                        P_VIDEO_STREAM* pvid=(P_VIDEO_STREAM*)(pack.data()+sizeof (NetPack));
                        int psize=p->packSize-sizeof (P_VIDEO_STREAM) ;
                        char* data=(pack.data()+sizeof (NetPack)+sizeof (P_VIDEO_STREAM));
                        break;
                }
                case M_AUDIO_STREAM://5.直播内容——音频
{
                        P_AUDIO_STREAM* pvid=(P_AUDIO_STREAM*)(pack.data()+sizeof (NetPack));
                        int psize=p->packSize-sizeof (P_AUDIO_STREAM) ;
                        char* data=(pack.data()+sizeof (NetPack)+sizeof (P_AUDIO_STREAM));
                        //char* data=new char[psize];
                        //data=(pack.data()+sizeof (NetPack)+sizeof (P_AUDIO_STREAM));
                        break;
                }
                case M_SEND_WORD://6.发送信息（文字或固定表情）
                {
                        P_SEND_WORD* pWord=(P_SEND_WORD*)(pack.data()+sizeof (NetPack));
                        int roomID=p->roomID;

			cout<<"[广播中] 房间: " << roomID << " 用户: " << pWord->senderName << " 内容: " << pWord->chatWord << std::endl;
			
			if(g_roomMembers.find(roomID)!=g_roomMembers.end()){
				const auto& fds=g_roomMembers[roomID];
				
				for(int onlineFD:fds){
					send(onlineFD,pack.data(),allSize,0);
				}
			}
			break;
                }
		case M_SEND_EMOJI://7.发送信息（文字或固定表情）
                {
                        P_SEND_EMOJI* pemo=(P_SEND_EMOJI*)(pack.data()+sizeof (NetPack));
                        break;
                }
                case M_GET_ROOMLIST_RQ://8.客户端请求列表
                {
                        
                        break;
                }
                default:
			std::cout << "收到未知包类型: " << p->msgType << std::endl;
                        break;
                }

        //读完这个包，将其从缓冲区移除
        //pack.erase(0,p->packSize+sizeof(NetPack));
        // 必须用迭代器：从 begin 开始，删到 begin + totalLen 的位置
        pack.erase(pack.begin(), pack.begin() + allSize);
        }

}

int main(){

	//1.create listen socket
	int listen_fd=socket(AF_INET,SOCK_STREAM,0);
	if(listen_fd<0){
		perror("socket");
		return -1;
	}

	// 端口复用 (秋招面试官很喜欢看这个，防止程序重启时端口被占用)
    	int opt = 1;
    	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	//2.bind 
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=INADDR_ANY;
	addr.sin_port=htons(PORT);

	if(bind(listen_fd,(struct sockaddr*)&addr,sizeof(addr))<0){
		perror("bind");
		return -1;
	}

	//3.listen
	if(listen(listen_fd,20)<0){
		perror("listen");
		return -1;
	}


	//4.create epoll
	int epoll_fd=epoll_create(1);
	if(epoll_fd<0){
		perror("epoll creat");
		return -1;
	}

	//
	struct epoll_event ev;
	ev.events=EPOLLIN | EPOLLET;
	// EPOLLIN: 表示“有数据读” (对于大门来说，意味着“有人来连接了”)
    	// EPOLLET: 边缘触发模式 (Edge Triggered)，这是 Epoll 高效
	ev.data.fd=listen_fd;

	if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listen_fd,&ev)<0){
		perror("epoll_ctl::listen sock");
		return -1;
	}

	std::cout<<"epoll server started on port "<<PORT<<" ..."<<std::endl;

	//开始干活 事件循环
	struct epoll_event events[MAX_EVENTS];

	while(true){
		//epoll wait,no things,no cpu
		//event sum
		int nfds=epoll_wait(epoll_fd,events,MAX_EVENTS,-1);
		
		if(nfds==-1){
			perror("epool_wait");
			break;
		}
		
		// 遍历所有发生的事件
		for(int i=0;i<nfds;i++){
			if(events[i].data.fd == listen_fd){
				//说明有新连接！
				struct sockaddr_in client_addr;
				socklen_t client_len =sizeof(client_addr);
				
				int conn_sock=accept(listen_fd,(struct sockaddr*)&client_addr,&client_len);
				if(conn_sock==-1){
					perror("accept");
					continue;
				}

				//关键:把新来的连接也设为非阻塞
				setNonBlocking(conn_sock);

				//
				struct epoll_event ev_client;
				ev_client.events=EPOLLIN | EPOLLET;
				ev_client.data.fd=conn_sock;

				if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,conn_sock,&ev_client)<0){
					perror("epoll_ctl:conn_sock");
					close(conn_sock);
				}else{
					std::cout<<"New client connected: "<<conn_sock<<std::endl;
					// 初始化他的专属缓冲区 (空 vector)
                    			g_clientBuffer[conn_sock] = std::vector<char>();				
					//给大厅直接推送卡片信息
					int roomcount=2;
					int bodySize=roomcount*sizeof(P_ROOMCARD);
					int allSize=sizeof(NetPack)+bodySize;

					std::vector<char> sendBuf(allSize,0);
					NetPack* pHead=(NetPack*)sendBuf.data();
					pHead->msgType=M_ROOMCARD;
					pHead->packSize=bodySize;
					pHead->userID=1;
					pHead->roomID=-1;
	
					// 3. 填充房间卡片信息
    					P_ROOMCARD* cards = (P_ROOMCARD*)(sendBuf.data() + sizeof(NetPack));
    					
    					cards[0].roomID = 333;
    					cards[0].picID=1;
					strcpy(cards[0].ownerName, "小紫");
    					strcpy(cards[0].roomTitle, "C++硬核直播间");

    					cards[1].roomID = 344;
					cards[1].picID=2;
    					strcpy(cards[1].ownerName, "坦坦");
    					strcpy(cards[1].roomTitle, "手撕Epoll实战");

    					// 4. 执行发送
    					send(conn_sock, sendBuf.data(), allSize, 0);
    					std::cout << ">> 已向新客户端 " << conn_sock << " 推送首屏大厅数据" << std::endl;

				}
			}else{
				//有客户端发数据
				std::cout<<"Data received from client: "<<events[i].data.fd<<std::endl;
				int client_fd=events[i].data.fd;

				//边缘触发模式，所以必须一次把缓冲区内的东西读完
				while(1){
					char buf[4096]={0};//一次最多收4K
					int len=read(client_fd,buf,sizeof(buf));
					if(len>0){
						//把收到d数据塞进这个客户端专属的vector里
						for(int j=0;j<len;j++){
							g_clientBuffer[client_fd].push_back(buf[j]);
						}
					}else if(len==0){
						//客户端断开连接
						cout<<"Clien disconnected: "<<client_fd<<std::endl;
						close(client_fd);// 关闭fd会自动从epoll里移除，不需要手动 del
						//清理内存，防止内存泄漏
						g_clientBuffer.erase(client_fd);
						break;
					}else{
						//非阻塞状态,len<0
						if(errno==EAGAIN||errno==EWOULDBLOCK){
							//开始解析包数据
							HandleData(g_clientBuffer[client_fd],client_fd);
							break;
						}else{
							perror("read error");
							close(client_fd);
							g_clientBuffer.erase(client_fd);
							break;
						}
					}
				}
			}

		}
			
	}
	close(listen_fd);
	close(epoll_fd);

	return 0;

}

