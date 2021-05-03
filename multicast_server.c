/* Send Multicast Datagram code example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE 1024
#define SLEEP_MS 100

struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;

int main(int argc, char *argv[])
{
	/* Create a datagram socket on which to send. */
	char buffer[SIZE];
	char *file_name = argv[1];
	size_t f_size;
	sd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sd < 0)
	{
		perror("Opening datagram socket error");
		exit(1);
	}
	else
		printf("Opening the datagram socket...OK.\n");

	/* Initialize the group sockaddr structure with a */
	/* group address of 226.1.1.1 and port 4321. */
	memset((char *)&groupSock, 0, sizeof(groupSock));
	groupSock.sin_family = AF_INET;
	groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
	groupSock.sin_port = htons(4321);

	/* Set local interface for outbound multicast datagrams. */
	/* The IP address specified must be associated with a local, */
	/* multicast capable interface. */
	localInterface.s_addr = inet_addr("127.0.0.1");

	/* IP_MULTICAST_IF:  Sets the interface over which outgoing multicast datagrams are sent. */
	if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
	{
		perror("Setting local interface error");
		exit(1);
	}
	else
		printf("Setting the local interface...OK\n");

	int lb = 0;
	if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (const char *)&lb, sizeof(lb)) < 0)
	{
		perror("setting IP_MULTICAST_LOOP error");
		exit(1);
	}
	FILE *fp = fopen(file_name, "rb");

	fseek(fp, 0, SEEK_END);
	f_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	int chunk = f_size / SIZE;
	int rest = f_size - chunk * SIZE;
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
	
	/* Send a message to the multicast group specified by the*/
	/* groupSock sockaddr structure. */
	/*int datalen = 1024;*/
	int file_name_len = strlen(file_name)+1;
	if (sendto(sd, &file_name_len, sizeof(file_name_len), 0, (struct sockaddr *)&groupSock, sizeof(groupSock)) < 0)
        {
                perror("Sending datagram message error");
        }

	printf("file name len %d\n",file_name_len);


        if (sendto(sd, file_name, file_name_len, 0, (struct sockaddr *)&groupSock, sizeof(groupSock)) < 0)
        {
                perror("Sending datagram message error");
        }
        else{
                printf("Sending datagram message...OK.\n");
                printf("File name: %s\n", file_name);
		usleep(SLEEP_MS);
        }


	if (sendto(sd, &f_size, sizeof(f_size), 0, (struct sockaddr *)&groupSock, sizeof(groupSock)) < 0)
	{
		perror("Sending datagram message error");
	}
	else{
		printf("File size: %f%s\n", send_size,unit);
		usleep(SLEEP_MS);
	}

	while (chunk--)
	{
		fread(buffer, SIZE, 1, fp);
		sendto(sd, buffer, SIZE, 0, (struct sockaddr *)&groupSock, sizeof(groupSock));
		usleep(SLEEP_MS);
	}
	fread(buffer, rest, 1, fp);
	sendto(sd, buffer, rest, 0, (struct sockaddr *)&groupSock, sizeof(groupSock));
	usleep(SLEEP_MS);
	close(sd);
	/* Try the re-read from the socket if the loopback is not disable
	if(read(sd, databuf, datalen) < 0)
	{
	perror("Reading datagram message error\n");
	close(sd);
	exit(1);
	}
	else
	{
	printf("Reading datagram message from client...OK\n");
	printf("The message is: %s\n", databuf);
	}
	*/
	return 0;
}
