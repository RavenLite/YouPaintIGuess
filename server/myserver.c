/*
 * webserv.c - a minimal web server (version 0.2) usage: ws portnumber
 * features: supports the GET command only runs in the current directory
 * forks a new child to handle each request
 * has MAJOR security holes, for demo purposes only has many other weaknesses, but is a good start
 * 
 * @author: Raven Xu
*/ 
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <stdlib.h>
#include <bsd/string.h>
#define HOSTLEN 256
#define BACKLOG 1
void return_file(char *f, int fd, char *content);
void operation(char *f, int fd);
typedef struct player P;

struct player {
    char *name; // 玩家姓名
    int score; // 玩家积分
};

struct game {
    P players[6]; // 玩家数组
    int state; // 游戏状态(0-未开始, 1-进行中)
    int number; // 人数
    int time; // 剩余时间
    char message[500];// 消息
    char *curname; // 绘图玩家
    char *key; // 关键语句
    char *img; // 当前图像
} mygame;

void read_til_crnl(FILE *fp) {
    char buf[BUFSIZ];
    while (fgets(buf,BUFSIZ,fp) != NULL && strcmp(buf,"\r\n") != 0); 
}

void header(FILE *fp, char *content_type) {
    fprintf(fp, "HTTP/1.0 200 OK\r\n"); 
    if (content_type)
        fprintf(fp, "Content-type: %s\r\n", content_type);
}

char *file_type(char *f) {
    /* returns 'extension' of file */ 
    char *cp;
    if ((cp = strrchr(f, '.')) != NULL)
    return cp+1; 
    return "";
}

void cannot_do(int fd) {
    FILE *fp = fdopen(fd, "w");
    fprintf(fp, "HTTP/1.0 501 Not Implemented\r\n"); 
    fprintf(fp, "Content-type: text/plain\r\n"); 
    fprintf(fp, "\r\n");
    fprintf(fp, "That command is not yet implemented\r\n");
    fclose(fp); 
}

int not_exist(char *f) {
    struct stat info;
    return(stat(f, &info) == -1); 
}

/* 字符串分割
 *返回一个 char *arr[], size为返回数组的长度
 */
char **explode(char sep, const char *str, int *size) {
        int count = 0, i;
        for(i = 0; i < strlen(str); i++)
        {       
                if (str[i] == sep)
                {       
                        count ++;
                }
        }
        char **ret = calloc(++count, sizeof(char *));
        int lastindex = -1;
        int j = 0;
        for(i = 0; i < strlen(str); i++)
        {       
                if (str[i] == sep)
                {       
                        ret[j] = calloc(i - lastindex, sizeof(char)); //分配子串长度+1的内存空间
                        memcpy(ret[j], str + lastindex + 1, i - lastindex - 1);
                        j++;
                        lastindex = i;
                }
        }
        //处理最后一个子串
        if (lastindex <= strlen(str) - 1)
        {
                ret[j] = calloc(strlen(str) - lastindex, sizeof(char));
                memcpy(ret[j], str + lastindex + 1, strlen(str) - 1 - lastindex);
                j++;
        }
        *size = j;
        return ret;
}

/* ------------------------------------------------------ *
 * do_cat(filename,fd)
 * sends back contents after a header 
 * ------------------------------------------------------ */
void do_404(char *item, int fd) {
    FILE *fp = fdopen(fd, "w");
    fprintf(fp, "HTTP/1.0 404 Not Found\r\n"); 
    fprintf(fp, "Content-type: text/plain\r\n"); 
    fprintf(fp, "\r\n");
    fprintf(fp, "The item you requested: %s\r\nis not found\r\n", item);
    fclose(fp); 
}

/* ------------------------------------------------------ *
 * do_cat(filename,fd)
 * sends back contents after a header 
 * ------------------------------------------------------ */
void do_cat(char *f, int fd) {
    // 请求处理
    if (strstr(f, "data") != NULL) {
        operation(f, fd);
    } else { 
    // 文件返回
        char *extension = file_type(f); 
        char *content = "text/plain";
        if (strcmp(extension,"html") == 0) {
            content = "text/html";
            return_file(f, fd, content);
        }
        else if (strcmp(extension, "gif") == 0) {
            content = "image/gif";
            return_file(f, fd, content);
        }  
        else if (strcmp(extension, "jpg") == 0) {
            content = "image/jpeg";
            return_file(f, fd, content);
        }
        else if (strcmp(extension, "jpeg") == 0) {
            content = "image/jpeg";
            return_file(f, fd, content);
        }
        else if (strcmp(extension, "ico") == 0) {
            content = "image/x-icon";
            return_file(f, fd, content);
        }
        else if (strcmp(extension, "js") == 0) {
            content = "application/x-javascript";
            return_file(f, fd, content);
        }
        else if (strcmp(extension, "css") == 0) {
            content = "text/css";
            return_file(f, fd, content);
        }
    }
    //exit(0); 
}

void return_file(char *f, int fd, char *content) {
    FILE *fpsock, *fpfile;
    int c;
    fpsock = fdopen(fd, "w");
    fpfile = fopen( f , "r");
    if (fpsock != NULL && fpfile != NULL) {
        header(fpsock, content); 
        fprintf(fpsock, "\r\n");
        while ((c = getc(fpfile)) != EOF )
            putc(c, fpsock); 
            fclose(fpfile);
            fclose(fpsock); 
    }
}

void login(char *name, int fd) {
    mygame.number++; // 游戏人数加一
    mygame.players[mygame.number-1].name = name; // 存入玩家姓名
    mygame.players[mygame.number-1].score = 0; // 初始化玩家积分

    char notification[50] = "欢迎玩家 [";
    strncat(notification, name, 50);
    strncat(notification, "] 加入游戏！<br>", 50);
    strncat(mygame.message, notification, 50);

    // 发送返回值
    FILE *fp = fdopen(fd, "w");
    fprintf(fp, "HTTP/1.0 200 OK\r\n"); 
    fprintf(fp, "Content-type: text/plain\r\n"); 
    fprintf(fp, "\r\n");
    fprintf(fp, "Login successfully\r\n");
    fclose(fp); 
}

void getInfo(int fd){
    FILE *fp = fdopen(fd, "w");
    char data[6000000] = "{";

    char mystate[5]; // 游戏状态(0-未开始, 1-进行中)
    char mynumber[5]; // 人数
    char mytime[5]; // 剩余时间
    sprintf(mystate, "%d" , mygame.state);
    sprintf(mytime, "%d" , mygame.time);
    sprintf(mynumber, "%d" , mygame.number);
    // char *message;// 消息数组
    // char *curname; // 绘图玩家
    // char *key; // 关键语句
    strncat(data, "\"state\": \"", 15);
    strncat(data, mystate, 10);
    strncat(data, "\",", 10);

    strncat(data, "\"number\": \"", 15);
    strncat(data, mynumber, 10);
    strncat(data, "\",", 10);

    strncat(data, "\"time\": \"", 15);
    strncat(data, mytime, 10);
    strncat(data, "\",", 10);

    strncat(data, "\"curname\": \"", 15);
    strncat(data, mygame.curname, 10);
    strncat(data, "\",", 10);

    strncat(data, "\"key\": \"", 15);
    strncat(data, mygame.key, 10);
    strncat(data, "\",", 10);

    strncat(data, "\"img\": \"", 15);
    strncat(data, mygame.img, 50000);
    strncat(data, "\",", 10);

    strncat(data, "\"message\": \"", 15);
    strncat(data, mygame.message, 500);
    strncat(data, "\"", 10);

    strncat(data, "}", 10);
    printf("%s\n", data);

    fprintf(fp, "HTTP/1.0 200 OK\r\n"); 
    fprintf(fp, "Content-type: text/plain\r\n"); 
    fprintf(fp, "\r\n");
    fprintf(fp, data);
    // fprintf(fp, "{\"name\": \"raven\"}");
    fprintf(fp, "\r\n");
    fclose(fp); 
}

void start(int fd) {
    // 修改游戏状态
    mygame.state = 1;
    mygame.curname = mygame.players[0].name;
    mygame.key = "sun";

    // 增加消息记录
    char notification[50] = "游戏开始，请玩家 ";
    strncat(notification, mygame.curname, 50);
    strncat(notification, " 开始作图！<br>", 50);
    strncat(mygame.message, notification, 50);

    // 发送返回值
    FILE *fp = fdopen(fd, "w");
    fprintf(fp, "HTTP/1.0 200 OK\r\n"); 
    fprintf(fp, "Content-type: text/plain\r\n"); 
    fprintf(fp, "\r\n");
    fprintf(fp, "Game started\r\n");
    fclose(fp); 
}


void chat(char *name, char *message, int fd){
    // 是否是正确答案
    if(strcmp(mygame.key, message) == 0){
        char notification[100] = "游戏结束，恭喜玩家 ";
        strncat(notification, name, 10);
        strncat(notification, " 回答正确！<br>正确答案：", 50);
        strncat(notification, mygame.key, 10);
        strncat(mygame.message, notification, 100);

        mygame.state = 0;
        mygame.curname = "未开始";
        mygame.key = "未开始";
    } else {
        // 增加消息记录
        char notification[50] = "[";
        strncat(notification, name, 10);
        strncat(notification, "]", 5);
        strncat(notification, message, 30);
        strncat(notification, "<br>", 10);
        strncat(mygame.message, notification, 50);
    }

    // 发送返回值
    FILE *fp = fdopen(fd, "w");
    fprintf(fp, "HTTP/1.0 200 OK\r\n"); 
    fprintf(fp, "Content-type: text/plain\r\n"); 
    fprintf(fp, "\r\n");
    fprintf(fp, "Chat well\r\n");
    fclose(fp); 
}

void upload(char *img, int fd) {
    mygame.img = img;

    // 发送返回值
    FILE *fp = fdopen(fd, "w");
    fprintf(fp, "HTTP/1.0 200 OK\r\n"); 
    fprintf(fp, "Content-type: text/plain\r\n"); 
    fprintf(fp, "\r\n");
    fprintf(fp, "Upload successfully\r\n");
    fclose(fp); 
}

void getimg(int fd){
    printf("%s", mygame.img);
    FILE *fp = fdopen(fd, "w");
    fprintf(fp, "HTTP/1.0 200 OK\r\n"); 
    fprintf(fp, "Content-type: text/plain\r\n"); 
    fprintf(fp, "\r\n");
    fprintf(fp, mygame.img);
    fprintf(fp, "\r\n");
    fclose(fp); 
}

void operation(char *f, int fd) {
    int size;
    char *action = "";
    char *data = "";
    char **ret = explode('&', f, &size);
    action = ret[0];
    data = ret[1];
    ret = explode('=', action, &size);
    action = ret[1];
    ret = explode('=', data, &size);
    data = ret[1];
    printf("[INFO]Handle the request: \naction: %s \ndata: %s\n", action, data);
    if(strcmp(action, "login") == 0) {
        login(data, fd);
    } else if (strcmp(action, "getInfo") == 0) {
        getInfo(fd);
    } else if (strcmp(action, "start") == 0) {
        start(fd);
    } else if (strcmp(action, "upload") == 0) {
        upload(data, fd);
    } else if (strcmp(action, "getimg") == 0) {
        getimg(fd);
    } else if (strcmp(action, "leave") == 0) {
        // leave();
    } else {
        chat(action, data, fd);
    } 
}

void process_post(int fd) {
    printf("Get a post request\n");
}
/* ------------------------------------------------------ *
 * process_rq( char *rq, int fd )
 * do what the request asks for and write reply to fd handles request in a new process
 * rq is HTTP command: GET /foo/bar.html HTTP/1.0 
 * ------------------------------------------------------ */
void process_rq(char *rq, int fd ) {
    char cmd[BUFSIZ], arg[BUFSIZ];
    /* create a new process and return if not the child */ 
    // if (fork() != 0)
    //     return;
    strcpy(arg, "./"); 
    /* precede args with ./ */ 
    if (sscanf(rq, "%s%s", cmd, arg+2) != 2)
        return;
    if (strcmp(cmd, "GET") != 0 ) {
        process_post(fd);
    } else {
        do_cat(arg, fd);
    } 
}
void write_log(char message[500]){
    FILE *fp;
	char c = '\n';
    lockf(fp,1,0);
    fp = fopen("record.txt","a");
    fprintf(fp,"%s",message);
    lockf(fp,0,0);
    fclose(fp);
}
int main(int ac, char *av[]){
    /* 初始化游戏状态 */
    mygame.state = 0;
    mygame.number = 0;
    mygame.time = 0;
    strncat(mygame.message, "YPIG(You-Paint-I-Guess)<br>", 50);
    mygame.curname = "未开始";
    mygame.key = "未开始";
    mygame.img = "";
	/* 根据所指定的端口号创建socket用于监听 */
	int tcp_socket;
	struct sockaddr_in addr;
	int n;
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(av[1]));
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(tcp_socket, (const struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("cannot bind");
		exit(1);
	}

	/* 开启监听 */
	listen(tcp_socket, 1);
    system("clear");
    printf("YPIG(You-Paint-I-Guess) Server started successfully.\n");
    printf("YPIG is a web game finished on 'NEU 2018 Linux OS course' by Raven Xu.\n");
    printf("Copyright - 2018 All Rights Reserved.\n");
    printf("===========================================\n\n");
    printf("Listening on Port: %s\n", av[1]);
    
	int fd;
    FILE *fpin;
	char request[1024];
	/* 死循环以便处理各种请求 */
	while(1) {
        fd = accept(tcp_socket, NULL, NULL);// 若无请求则一直处于阻塞态
        fpin = fdopen(fd, "r");
        /* read request */ 
        fgets(request, BUFSIZ, fpin);// 
        printf("[INFO]Receive a request: %s", request);// 打印收到的请求内容
        //写入日志文件，异步IO/多线程
        if (fork() == 0){
            write_log(mygame.message);
            exit(0);
        }
        read_til_crnl(fpin);
        /* do what client asks */
        process_rq(request, fd);// 处理请求
        fclose(fpin); 
	}

    return 0;
}