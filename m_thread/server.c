#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>    
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <pthread.h>


#define MAXLINE 200
#define RIO_BUFSIZE 8192
#define LISTENQ 1024

typedef struct{
    int rio_fd;
    int rio_cnt;
    char *rio_bufptr;
    char rio_buf[RIO_BUFSIZE];

}rio_t;
typedef struct sockaddr SA;

typedef struct{
int tm_sec;
int tm_min;
int tm_hour;
int tm_mday;
int tm_mon;
int tm_year;
int tm_wday;
int tm_yday;
int tm_isdst;
}tm;

void *thread(void *vargp);

int open_listenfd(int port){

    int listenfd,optval=1;
    struct sockaddr_in serveraddr;

    if((listenfd = socket(AF_INET,SOCK_STREAM,0))<0)
        return -1;

    if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,
                (const void*)&optval,sizeof(int))<0)
        return -1;

    bzero((char *)&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);
    if(bind(listenfd,(SA*)&serveraddr,sizeof(serveraddr))<0)
        return -1;

    if(listen(listenfd,LISTENQ)<0)
        return -1;

    return listenfd;
}


void sigchld_handler(int sig){

    pid_t pid;
    int stat;

    while((pid = waitpid(-1,&stat,WNOHANG))>0){
        printf("child %d terminated\n",pid);
    }
    return;
}

int get_local_ip(const char *eth_inf,char *ipBuf)
{
    int sd;
    struct sockaddr_in sin;
    struct ifreq ifr;
 
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }
 
    strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
 
    // if error: No such device  
    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)
    {
        printf("ioctl error: %s\n", strerror(errno));
        close(sd);
        return -1;
    }
    
    snprintf(ipBuf ,50,"%s" ,inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr)); 

    close(sd);
    return 0;
}


int main(int argc,char **argv){
    int listenfd,*connfdp,port,clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;
    pthread_t tid;
    char sbuf[MAXLINE];
    char rbuf[MAXLINE];
    char myhost[32] = "";
    rio_t rio;
    time_t lt;
    tm *local;
    char str1[MAXLINE]="Server IP:";
    char str2[MAXLINE]="Server Implementer:";
    char str3[MAXLINE]="Current Time:";
     
    

    if(argc != 2){
    
        fprintf(stderr,"usage:%s <port>\n",argv[0]);
        exit(0);
    }

    port = atoi(argv[1]);
    signal(SIGCHLD,sigchld_handler);
    listenfd = open_listenfd(port);
    while(1){
    
	clientlen = sizeof(clientaddr);

        connfdp =malloc(sizeof(int));
    
        *connfdp = accept(listenfd,(SA *)&clientaddr,&clientlen);	

        hp = gethostbyaddr((const char*)&clientaddr.sin_addr.s_addr,
                sizeof(clientaddr.sin_addr.s_addr),AF_INET);
        haddrp = inet_ntoa(clientaddr.sin_addr);
        printf("server connected to %s (%s)\n",hp->h_name,haddrp);
		
	pthread_create(&tid,NULL,thread,connfdp);

        pthread_join(tid,NULL);

    }
}


void *thread(void *vargp){
    
    time_t lt;
    tm *local;
    char sbuf[MAXLINE];
    char str1[MAXLINE]="Server IP:"; 
    char myhost[32] = "";
    
    int connfd = *((int*)vargp);

    free(vargp);

    pthread_detach(pthread_self());

	lt = time(NULL);
        local = localtime(&lt);
        strftime(sbuf,64,"%Y-%m-%d %H:%M:%S",local);
        get_local_ip("enp0s3",myhost);
        //send(connfd,myhost,MAXLINE,0);
        strcat(sbuf,"\n");
        strcat(sbuf,str1);
        strcat(sbuf,myhost);
        send(connfd,sbuf,MAXLINE,0);
        close(connfd);
return NULL;
}
