#include "pico/cyw43_arch.h"
#include <string>
#include <string.h>

class IPAddress {
	private:
		uint8_t octets[4];
	public:
		IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
		IPAddress(const ip4_addr_t* lwip_ip);

		std::string toString() const;

		operator std::string() const {
			return toString();
		}
};

IPAddress::IPAddress(uint8_t first_octet,
					 uint8_t second_octet,
					 uint8_t third_octet,
					 uint8_t fourth_octet)
{
	octets[0] = first_octet;
	octets[1] = second_octet;
	octets[2] = third_octet;
	octets[3] = fourth_octet;
}

IPAddress::IPAddress(const ip4_addr_t* lwip_ip){
	uint32_t ip = lwip_ip->addr;
	octets[0] = (ip & 0xff);
	octets[1] = ((ip >> 8) & 0xff);
	octets[2] = ((ip >> 16) & 0xff);
	octets[3] = (ip >> 24);
}

std::string IPAddress::toString() const {
	char buf[16 /* XXX.XXX.XXX.XXX0 */];
	sprintf(buf, "%d.%d.%d.%d",
		octets[0],
		octets[1],
		octets[2],
		octets[3]
	);
	return std::string(buf);
}