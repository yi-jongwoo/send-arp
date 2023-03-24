#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <cstring>
#include <pcap.h>
#include <ctype.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "proto_structures.h"
mac_addr get_mac_addr(const char *dev){
	std::ifstream fs(std::string("/sys/class/net/")+dev+"/address");
	if(!fs.is_open()){
		printf("can\'t access %s\n",dev);
	}
	std::string s;fs>>s;
	mac_addr res(s);
	return res;
}
ipv4_addr get_ipv4_addr(const char *dev){
	int fd=socket(AF_INET,SOCK_DGRAM,0);
	struct ifreq ifr;
	ifr.ifr_addr.sa_family=AF_INET;
	strncpy(ifr.ifr_name,dev,IFNAMSIZ-1);
	ioctl(fd,SIOCGIFADDR,&ifr);
	return ipv4_addr(inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
}

mac_addr arp_request(const ipv4_addr& sip,const ipv4_addr& tip,const mac_addr& smac,const char *dev){
	arp_eth_ipv4 packet(smac,sip,tip);
	
	assert(sizeof packet==42);
	
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t* handle=pcap_open_live(dev,BUFSIZ,1,1,errbuf);
	if(handle==nullptr){
		printf("pcap error : %s\n",errbuf);
		exit(1);
	}
	pcap_sendpacket(handle,packet,42);
	pcap_close(handle);
}

void arp_spoof(const char *str_sip,const char *str_tip,const ipv4_addr& ip,const mac_addr &mac,const char *dev){
	ipv4_addr sip(str_sip);
	ipv4_addr tip(str_tip);
	mac_addr victim_mac=arp_request(ip,sip,mac,dev);
}

int main(int c,char **v){
	if(c&1){
		printf("syntex : send-arp <interface> <sender ip> <target ip> ...\n");
		return 1;
	}
	mac_addr mac=get_mac_addr(v[1]);
	ipv4_addr ip=get_ipv4_addr(v[1]);
	std::cout<<std::string(mac)<<' '<<std::string(ip)<<std::endl;
	for(int i=2;i<c;i+=2)
		arp_spoof(v[i],v[i+1],ip,mac,v[1]);
	return 0;
}
