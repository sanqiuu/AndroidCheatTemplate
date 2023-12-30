//
// Created by Administrator on 2022/3/3.
//

#ifndef BAIDU_AMMAP_H
#define BAIDU_AMMAP_H

#include "log.h"
#include <sys/mman.h>
#include <linux/ashmem.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <sys/system_properties.h>

template<typename S>
class Ammap {

    int *recv_fd(string &socketname, int n) {
        int                sfd, cfd, *fds;
        struct sockaddr_un addr;
        unlink(socketname.c_str());
        if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
            LOGE("socket err: %s\n", strerror(errno));
            return NULL;
        }
        int reuse = 1;
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
            LOGE("setsockopt err: %s\n", strerror(errno));
            unlink(socketname.c_str());
            return NULL;
        }
        /*  如果UNIX_DOMAun_FILE所指向的文件存在，则删除 */
        if (access(socketname.c_str(), F_OK)) {
            unlink(socketname.c_str());//删除该文件
        }
        bzero(&addr, sizeof(addr));
        addr.sun_family = AF_UNIX;

        strncpy(addr.sun_path, socketname.c_str(), sizeof(addr.sun_path) - 1);

        if (::bind(sfd, (struct sockaddr *) &addr, sizeof(addr))) {
            LOGE("bind err: %s\n", strerror(errno));
            unlink(socketname.c_str());
            return NULL;
        }
        chmod(socketname.c_str(), 0777);
        if (listen(sfd, 5) < 0) {
            LOGE("listen err: %s\n", strerror(errno));
            unlink(socketname.c_str());
            return NULL;
        }
        if ((cfd = accept(sfd, NULL, NULL)) < 0) {
            LOGE("accept err: %s\n", strerror(errno));
            unlink(socketname.c_str());
            return NULL;
        }

        fds = (int *) malloc(n * sizeof(int));
        struct msghdr  msg = {0};
        struct cmsghdr *cmsg;
        char           buf[CMSG_SPACE(n * sizeof(int))], data;
        memset(buf, '\0', sizeof(buf));
        struct iovec io = {.iov_base = &data, .iov_len = sizeof(data)};
        msg.msg_name       = NULL;
        msg.msg_namelen    = 0;
        msg.msg_flags      = 0;
        msg.msg_iov        = &io;
        msg.msg_iovlen     = 1;
        msg.msg_control    = buf;
        msg.msg_controllen = sizeof(buf);

        if (recvmsg(cfd, &msg, 0) < 0) {
            LOGE("recvmsg errno: %s\n", strerror(errno));
            unlink(socketname.c_str());
            return NULL;
        }

        cmsg = CMSG_FIRSTHDR(&msg);

        memcpy(fds, (int *) CMSG_DATA(cmsg), n * sizeof(int));
        close(sfd);
        close(cfd);
        unlink(socketname.c_str());
        return fds;
    }

    int send_fd(string socketname, int *fds, int n) {
        int                sfd;
        struct sockaddr_un addr;
        if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
            LOGE("socket err: %s\n", strerror(errno));
            return 0;
        }
        bzero(&addr, sizeof(addr));
        addr.sun_family = AF_UNIX;
//    strcpy(addr.sun_path,socketname.c_str());
        strncpy(addr.sun_path, socketname.c_str(), sizeof(addr.sun_path) - 1);
        if (connect(sfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            LOGE("socket connect err: %s\n", strerror(errno));
            return 0;
        }
        struct msghdr  msg = {0};
        struct cmsghdr *cmsg;
        char           buf[CMSG_SPACE(n * sizeof(int))], data;
        memset(buf, '\0', sizeof(buf));
        struct iovec io = {.iov_base = &data, .iov_len = sizeof(data)};
        msg.msg_name       = NULL;
        msg.msg_namelen    = 0;
        msg.msg_iov        = &io;
        msg.msg_iovlen     = 1;
        msg.msg_flags      = 0;
        msg.msg_control    = buf;
        msg.msg_controllen = sizeof(buf);

        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type  = SCM_RIGHTS;
        cmsg->cmsg_len   = CMSG_LEN(n * sizeof(int));

        memcpy((int *) CMSG_DATA(cmsg), fds, n * sizeof(int));

        if (sendmsg(sfd, &msg, 0) < 0) {
            LOGE("sendmsg errno: %s\n", strerror(errno));
            return 0;
        }
        close(sfd);
        return 1;
    }

public:

    void saveData(void *data,int size,char * saveName){



    };

    S *recvAmemMmapMsgData(const char *PH) {
        string sss = PH;
        S      *str;
        int    *fd = recv_fd(sss, 1);
        str = (S *) mmap(NULL, sizeof(S), PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);
        close(*fd);
        if (str != MAP_FAILED) {
            return str;
        } else {
            return nullptr;
        }
    }

    S *sendAmemMmapMsgData(const char *PH) {
        int fd;
        S   *str;
        fd = open("/dev/ashmem", O_RDWR);
        ioctl(fd, ASHMEM_SET_SIZE, sizeof(S));
        string sss = PH;
        int    u   = 0;
        for (;;) {
            if (send_fd(sss, &fd, 1)) {
                break;
            }
            ++u;
            if (u > 15) {
                LOGE("无法找到主程序，退出");
                exit(10);
            }
            usleep(200000);
        }
        str = (S *) mmap(NULL, sizeof(S), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);
        if (str != MAP_FAILED) {
            return str;
        } else {
            return nullptr;
        }
    }


};
template<typename T>
class SaveData{
    T saveData;
    int mmapSize;
    FILE *saveFd = nullptr;
    int Fd = -1;
    bool isSaveIo = false;
    bool isOpen = false;
    bool isIOorMMAP = false;
    string Dir;
    string Name;
    string File;
    int OFFSET = 0;
    const char * Slash = "/";
    //设置保存路径，最后不带/  并且检查文件夹是否存在如果不存在则创建
    void setDir(char *Dir_){
        Dir = Dir_;
        if (mkdir(Dir_, S_IRWXU | S_IRWXG ) == 0){
            LOGE("文件夹创建成功:%s",Dir_);
        } else{
            LOGE("文件夹创建失败:%s",Dir_);
        }
    }
    //设置保存名字 检查文件是否存在
    void setName(char *name){
        Name = name;
        File = Dir;
        File += Slash;
        File += Name;
        if ( access(File.c_str(), F_OK) == 0){
            LOGE("文件存在:%s",File.c_str());
            isSaveIo = true;
        }else{
            LOGE("文件不存在:%s",File.c_str());
            isSaveIo = false;
        }
    }

    void InitIo(){
        if ( !isOpen ){
            if (isSaveIo){
                saveFd = fopen(File.c_str(),"wb+");
                if ( saveFd != nullptr ){
                    isOpen = true;
                }else{
                    isOpen = false;
                };
            }else{
                saveFd = fopen(File.c_str(),"rb+");
                if ( saveFd != nullptr ){
                    isOpen = true;
                }else{
                    isOpen = false;
                };
            }
        }
    }
    void InitMmap(int size){
        if ( !isOpen ){
            OFFSET = 0;
            Fd = open(File.c_str(),O_CREAT|O_RDWR|O_SYNC,00777);
            ftruncate(Fd, size);
            if ( Fd > 0 ){
                saveData = (T )mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, Fd, 0);
                LOGE("初始化Mmap成功");
                isOpen = true;
            }else{
                LOGE("初始化Mmap失败");
                isOpen = false;
            };
        }
    }
    void setOffset(int offset){
        if (isOpen){
            fseek(saveFd,offset,SEEK_SET);
        }
    }
    int writeData(T *Data,int size){
        if (isOpen){
            return fwrite(Data,size,1,saveFd);
        }
        return 0;
    }
    int readData(T *Data,int size){
        if (isOpen){
            return fread(Data,size,1,saveFd);
        }
        return 0;
    }
};


#endif //BAIDU_AMMAP_H
