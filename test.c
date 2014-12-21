#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <arpa/inet.h>

int sock_raw1;
int sock_raw2;

int main()
{
	printf("starting\n");
	unsigned char *pBuffer = (unsigned char *) malloc(0xFFFF);
	sock_raw1 = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	//sock_raw2 = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	setsockopt(sock_raw1, SOL_SOCKET, SO_BINDTODEVICE, "eth0", strlen("eth0")+1);
	//setsockopt(sock_raw2, SOL_SOCKET, SO_BINDDEVICE, "eth1", strlen("eth1")+1);
	
	if(sock_raw1 >= 0)
	{
		printf("Socket mounted.\n");
	}
	else
	{
		printf("Error mounting socket, %i\n", sock_raw1);
		return 1;
	}
	
	while(1)
	{
		int data_size = recv(sock_raw1, pBuffer, 0xFFFF, 0);
		printf("Packet recieved, %i bytes\n", data_size);
	}

	return 0;
}
