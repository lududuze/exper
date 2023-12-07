#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>    
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>

#define RIO_BUFSIZE 8192

typedef struct sockaddr SA;
typedef struct{
    int rio_fd;
    int rio_cnt;
    char *rio_bufptr;
    char rio_buf[RIO_BUFSIZE];

}rio_t;

#define MAXLINE 200

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


int open_clientfd(char *hostname,int port){

    int clientfd;
    struct hostent *hp;
    struct sockaddr_in serveraddr;

    if((clientfd = socket(AF_INET,SOCK_STREAM,0))<0)
        return -1;

    if((hp = gethostbyname(hostname))==NULL)
        return -2;

    bzero((char *)&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)hp->h_addr_list[0],
            (char *)&serveraddr.sin_addr.s_addr,hp->h_length);
    serveraddr.sin_port = htons(port);

    if(connect(clientfd,(SA *)&serveraddr,sizeof(serveraddr))<0)
        return -1;
    
    return clientfd;
}



int main(int argc,char **argv){

    int clientfd,port;
    char *host,buf[MAXLINE];
    char myhost[32] = "";
    char sbuf[MAXLINE];
    char rbuf[MAXLINE];
    rio_t rio;
    char str1[MAXLINE]="Client IP:";
    char str2[MAXLINE]="Server Implementer:20191331";
    
    char str3[MAXLINE]="Current Time:";

    if(argc!=3){
    
        fprintf(stderr,"usage:%s <host> <port>\n",argv[0]);
        exit(0);
    }
    get_local_ip("enp0s3",myhost);
    host = argv[1];
    port = atoi(argv[2]);

    clientfd = open_clientfd(host,port);
  
    while(1){
        

        recv(clientfd,rbuf,MAXLINE,0);

        printf("%s",str1);
        puts(myhost);

        printf("%s",str2);
        putchar('\n');

        printf("%s",str3);
      
        puts(rbuf);
       
        close(clientfd);
       
        exit(0);
    }

}
