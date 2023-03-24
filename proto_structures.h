#include <cstdint>
#include <iostream>
#include <sstream>
#include <cstring>
#include <netinet/in.h>

union ipv4_addr{
	static constexpr int siz=4;
	uint8_t addr[4];
	uint32_t word;
	ipv4_addr(){}
	ipv4_addr(uint32_t x);
	ipv4_addr(const std::string& x);
	ipv4_addr(const char *x);
	operator std::string();
};
