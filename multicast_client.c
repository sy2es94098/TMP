/* Receiver/client multicast Datagram example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define SIZE 1024

struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;

int main(int argc, char *argv[])
{
	srand(time(NULL));
	char buffer[SIZE];
	size_t f_size;
	double l_rate;
	char file_name[100];
	/* Create a datagram socket on which to receive. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd < 0)
	{
		perror("Opening datagram socket error");
		exit(1);
	}
	else
		printf("Opening datagram socket....OK.\n");

	/* Enable SO_REUSEADDR to allow multiple instances of this */
	/* application to receive copies of the multicast datagrams. */

	int reuse = 1;
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
	{
		perror("Setting SO_REUSEADDR error");
		close(sd);
		exit(1);
	}
	else
		printf("Setting SO_REUSEADDR...OK.\n");

	/* Bind to the proper port number with the IP address */
	/* specified as INADDR_ANY. */
	memset((char *)&localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(4321);
	localSock.sin_addr.s_addr = INADDR_ANY;
	if (bind(sd, (struct sockaddr *)&localSock, sizeof(localSock)))
	{
		perror("Binding datagram socket error");
		close(sd);
		exit(1);
	}
	else
		printf("Binding datagram socket...OK.\n");

	/* Join the multicast group 226.1.1.1 on the local address*/
	/* interface. Note that this IP_ADD_MEMBERSHIP option must be */
	/* called for each local interface over which the multicast */
	/* datagrams are to be received. */
	group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
	/* your ip address */
	group.imr_interface.s_addr = inet_addr("127.0.0.1");
	/* IP_ADD_MEMBERSHIP:  Joins the multicast group specified */
	if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
	{
		perror("Adding multicast group error");
		close(sd);
		exit(1);
	}
	else
		printf("Adding multicast group...OK.\n");

	/* Read from the socket. */
	int ls_len = sizeof(localSock);
	int file_name_len;
        if (recvfrom(sd, &file_name_len, sizeof(file_name_len), 0, (struct sockaddr *)&localSock, &ls_len) < 0)
        {
                perror("Reading datagram message error");
                close(sd);
                exit(1);
        }
	printf("file name len: %d\n",file_name_len);

        if (recvfrom(sd, file_name, file_name_len, 0, (struct sockaddr *)&localSock, &ls_len) < 0)
        {
                perror("Reading datagram message error");
                close(sd);
                exit(1);
        }
        else
        {
                printf("Reading datagram message...OK.\n");
                printf("Receive file name: %s\n", file_name);
        }


	if (recvfrom(sd, &f_size, sizeof(f_size), 0, (struct sockaddr *)&localSock, &ls_len) < 0)
	{
		perror("Reading datagram message error");
		close(sd);
		exit(1);
	}
	else
	{
		float send_size;
		char unit[3];
		if(f_size/1000000000 > 1)
		{
			send_size = f_size/1000000000;
			strcpy(unit,"GB");
		}else if(f_size/1000000 > 1){
			send_size = f_size/1000000;
			strcpy(unit,"MB");
		}else if(f_size/1000 > 1){
			send_size = f_size/1000;
			strcpy(unit,"KB");
		}else{
			send_size = f_size;
			strcpy(unit,"B");
		}
		printf("Receive file size: %f%s\n", send_size,unit);
	}

	int chunk = f_size / SIZE;
	int rest = f_size - chunk * SIZE;
        int r_num = (rand()%100000);
        char r_prefix[110];
        sprintf(r_prefix,"%d",r_num);
        strcat(r_prefix,"_");
        strcat(r_prefix, file_name);
        printf("Saved as %s\n",r_prefix);
	
	FILE *fp;
	fp = fopen(r_prefix,"wb");

	while (chunk--)
	{
		recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr *)&localSock, &ls_len);
		fwrite(buffer,SIZE,1,fp);
	}
	recvfrom(sd, buffer, rest, 0, (struct sockaddr *)&localSock, &ls_len);
	fwrite(buffer,rest,1,fp);
	
	close(sd);
	return 0;
}
