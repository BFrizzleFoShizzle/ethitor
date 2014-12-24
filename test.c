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

int main()
{
	printf("starting\n");
	unsigned char *pBuffer = (unsigned char *) malloc(0xFFFF);
	//create raw sockets
	sock_raw1 = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	sock_raw2 = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	//specify ethernet device
	struct sockaddr_ll device;
	struct ifreq ifr;
	
	bzero(&ifr, sizeof(ifr));

	strcpy(ifr.ifr_name,"eth0\x00");
	
	printf("%s\n",ifr.ifr_name);
	
	printf("%i\n",ifr.ifr_ifindex);

	if(ioctl(sock_raw1, SIOCGIFINDEX, &ifr)<0)
	{
		printf("failiure getting eth0 index\n");
	}
	device.sll_family = AF_PACKET;
	device.sll_ifindex = ifr.ifr_ifindex;
	//device.sll_protocol = htons(ETH_P_ALL);
	
	if(bind(sock_raw1, (struct sockaddr *) &device, sizeof(device)) == -1)
	{
		printf("error binding to device %i (%s)\n", device.sll_ifindex, ifr.ifr_name);
		//printf("%s\n",explain_bind(sock_raw1, (struct sockaddr*) &device, sizeof(device)));
	}
	bzero(&ifr, sizeof(ifr));	
	strcpy(ifr.ifr_name,"eth1\x00");
	if(ioctl(sock_raw2, SIOCGIFINDEX, &ifr)<0)
	{
		printf("failiure getting eth1 index\n");
	}
	
	device.sll_family = AF_PACKET;
	device.sll_ifindex = ifr.ifr_ifindex;

	if(bind(sock_raw2, (struct sockaddr *) &device, sizeof(device)) < 0)
	{
		printf("error binding to device %i (%s)\n", device.sll_ifindex, ifr.ifr_name);
	}
	//setsockopt(sock_raw2, SOL_SOCKET, SO_BINDTODEVICE, "eth1", 4);
	
	if(sock_raw1 > -1 && sock_raw2 > -1)
	{
		printf("Sockets mounted.\n");
	}
	else
	{
		printf("Error mounting socket, %i\n", sock_raw1);
		return 1;
	}
	printf("test\n");
	
	pthread_create(&thread1, NULL, recieveFromSocket, (void*)sock_raw1);
	pthread_create(&thread2, NULL, recieveFromSocket, (void*)sock_raw2);	
	printf("test\n");
	while(1)
	{
		sleep(1000);
		printf("listening...\n");
	}
	/*
	while(1)
	{
		int data_size = recv(sock_raw1, pBuffer, 0xFFFF, 0);
		printf("Packet recieved, %i bytes\n", data_size);
	}
	*/

	return 0;
}

void *recieveFromSocket(void* socket) 
{
	int sock = (int)socket;
	while(1)
	{
		unsigned char* pBuffer = (unsigned char *) malloc(0xFFFF);
		int data_size = recv(sock, pBuffer, 0xFFFF, 0);
		printf("Packet recieved, %i bytes on %i\n", data_size, sock);

	}
}
