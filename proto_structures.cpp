#include "proto_structures.h"
ipv4_addr::ipv4_addr(uint32_t x){
	word=htonl(x);
}
ipv4_addr::ipv4_addr(const std::string& x){
	std::istringstream ss(x);
	char _;
	for(int i=0;i<siz;i++){
		if(i)ss>>_;
		int tmp;ss>>tmp;
		addr[i]=tmp;
	}
}
ipv4_addr::ipv4_addr(const char *x){
	for(int i=0;i<siz;i++){
		int n;
		sscanf(x,"%hhd%n",addr+i,&n);
		x+=n+1;
	}
}
ipv4_addr::operator std::string(){
	std::ostringstream ss;
	for(int i=0;i<siz;i++){
		if(i)ss<<'.';
		int tmp=addr[i];
		ss<<tmp;
	}
	return ss.str();
}

mac_addr::mac_addr(const std::string& x){
	std::istringstream ss(x);
	char _;
	for(int i=0;i<siz;i++){
		if(i)ss>>_;
		int tmp;ss>>tmp;
		addr[i]=tmp;
	}
}
mac_addr::mac_addr(const char *x){
	for(int i=0;i<siz;i++){
		int n;
		sscanf(x,"%hhd%n",addr+i,&n);
		x+=n+1;
	}
}
mac_addr::operator std::string(){
	std::ostringstream ss;
	for(int i=0;i<siz;i++){
		if(i)ss<<'-';
		int tmp=addr[i];
		ss<<tmp;
	}
	return ss.str();
}
