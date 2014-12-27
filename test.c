#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <net/if.h>
//#include <libexplain/bind.h>


int sock_raw1, sock_raw2;
pthread_t thread1, thread2;

void *recieveFromSocket(void*);
void bindToDevice(int, const char[]);

int getOtherSocket(int);

int main()
{
	printf("starting\n");
	//create raw sockets
	sock_raw1 = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	sock_raw2 = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	
	bindToDevice(sock_raw1, "eth0\x00");
	bindToDevice(sock_raw2, "eth1\x00");
	
	
	if(sock_raw1 > -1 && sock_raw2 > -1)
	{
		printf("Sockets mounted.\n");
	}
	else
	{
		printf("Error mounting socket, %i\n", sock_raw1);
		return 1;
	}

	pthread_create(&thread1, NULL, recieveFromSocket, (void*)sock_raw1);
	pthread_create(&thread2, NULL, recieveFromSocket, (void*)sock_raw2);

	while(1)
	{
		sleep(1000);
		printf("listening...\n");
	}
	return 0;
}

void bindToDevice(int sock, const char name[]) 
{
	//specify ethernet device
	struct sockaddr_ll device;
	struct ifreq ifr;
	
	bzero(&ifr, sizeof(ifr));

	strcpy(ifr.ifr_name,name);
	
	printf("%s\n",ifr.ifr_name);
	
	printf("%i\n",ifr.ifr_ifindex);

	if(ioctl(sock, SIOCGIFINDEX, &ifr)<0)
	{
		printf("failiure getting eth0 index\n");
	}
	device.sll_family = AF_PACKET;
	device.sll_ifindex = ifr.ifr_ifindex;
	//device.sll_protocol = htons(ETH_P_ALL);
	
	if(bind(sock, (struct sockaddr *) &device, sizeof(device)) == -1)
	{
		printf("error binding to device %i (%s)\n", device.sll_ifindex, ifr.ifr_name);
		//printf("%s\n",explain_bind(sock_raw1, (struct sockaddr*) &device, sizeof(device)));
	}
}

void *recieveFromSocket(void* socket) 
{
	int from = (int)socket;
	int to = getOtherSocket(from);
	while(1)
	{
		unsigned char* pBuffer = (unsigned char *) malloc(0xFFFF);
		struct sockaddr_ll toAddress;
		int data_size = recv(from, pBuffer, 0xFFFF, 0);
		printf("Packet recieved, %i bytes on %i\n", data_size, from);
		if(send(to, pBuffer, data_size, 0)<0)
		{
			printf("can't send");
		}

	}
}

int getOtherSocket(int socket) {
	if(socket == sock_raw1) 
		return sock_raw2;
	if(socket == sock_raw2)
		return sock_raw1;
	return -1;
}
