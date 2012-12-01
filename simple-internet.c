#include "simple-internet.h"

const char* ipv4_ntoa(struct in_addr addr){
	return inet_ntoa(addr);
}
//copy from cygwin
int ipv4_aton(const char* addr, struct in_addr* result){
#if defined(_MSWINDOWS_)
	uint32_t val;
	int base, n;
	char c;
	uint8_t parts[4];
	uint8_t *pp = parts;
	int digit;

	c = *addr;
	for (;;) {
		/*
		 * Collect number up to ``.''.
		 * Values are specified as for C:
		 * 0x=hex, 0=octal, isdigit=decimal.
		 */
		if (!isdigit((unsigned char)c))
			return (0);
		val = 0; base = 10; digit = 0;
		if (c == '0') {
			c = *++addr;
			if (c == 'x' || c == 'X')
				base = 16, c = *++addr;
			else {
				base = 8;
				digit = 1 ;
			}
		}
		for (;;) {
			if (isascii(c) && isdigit((unsigned char)c)) {
				if (base == 8 && (c == '8' || c == '9'))
					return (0);
				val = (val * base) + (c - '0');
				c = *++addr;
				digit = 1;
			} else if (base == 16 && isascii(c) &&
				   isxdigit((unsigned char)c)) {
				val = (val << 4) |
					(c + 10 - (islower((unsigned char)c) ? 'a' : 'A'));
				c = *++addr;
				digit = 1;
			} else
				break;
		}
		if (c == '.') {
			/*
			 * Internet format:
			 *	a.b.c.d
			 *	a.b.c	(with c treated as 16 bits)
			 *	a.b	(with b treated as 24 bits)
			 */
			if (pp >= parts + 3 || val > 0xffU)
				return (0);
			*pp++ = val;
			c = *++addr;
		} else
			break;
	}
	/*
	 * Check for trailing characters.
	 */
	if (c != '\0' && (!isascii(c) || !isspace((unsigned char)c)))
		return (0);
	/*
	 * Did we get a valid digit?
	 */
	if (!digit)
		return (0);
	/*
	 * Concoct the address according to
	 * the number of parts specified.
	 */
	n = pp - parts + 1;
	switch (n) {
	case 1:				/* a -- 32 bits */
		break;

	case 2:				/* a.b -- 8.24 bits */
		if (val > 0xffffffU)
			return (0);
		val |= parts[0] << 24;
		break;

	case 3:				/* a.b.c -- 8.8.16 bits */
		if (val > 0xffffU)
			return (0);
		val |= (parts[0] << 24) | (parts[1] << 16);
		break;

	case 4:				/* a.b.c.d -- 8.8.8.8 bits */
		if (val > 0xffU)
			return (0);
		val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
		break;
	}
	if (addr != NULL)
		result->S_un.S_addr = htonl(val);
	return (1);
#else
	return inet_aton(addr, result);
#endif
}

in_addr_t ipv4_addr(const char* addr){
	return inet_addr(addr);
}

const char* ipv4_ntop(const void* addr, char* result, int resultlen){
	return inet_ntop(AF_INET,  (void*)addr, result, resultlen);
}

int ipv4_pton(const char* addr, void* result){
	return inet_pton(AF_INET, addr, result);
}

const char* ipv6_ntop(const void* addr, char* result, int resultlen){
	return inet_ntop(AF_INET6, (void*)addr, result, resultlen);
}

int ipv6_pton(const char* addr, void* result){
	return inet_pton(AF_INET6, addr, result);
}

const struct hostent* ipv4_gethostbyname(const char* hostname){
	return gethostbyname(hostname);
}

const struct hostent* ipv4_gethostbyaddr(const char* addr){
	return gethostbyaddr(addr, 4, AF_INET);
}

int ip_getaddrinfo(const char* hostname, const char* service, 
				   const struct addrinfo* hints, struct addrinfo** results){
	return getaddrinfo(hostname, service, hints, results);
}

void ip_freeaddrinfo(struct addrinfo* addrinfos){
	freeaddrinfo(addrinfos);
}

const char* ip_gai_strerror(int error){
#if defined(_MSWINDOWS_)
	return gai_strerrorA(error);
#else
	return gai_strerror(error):
#endif
}

const struct servent* ip_getservbyname(const char* servicename, const char* protocolname){
	return getservbyname(servicename, protocolname);
}

const struct servent* ip_getservbyport(int port, const char* protocolname){
	return getservbyport(port, protocolname);
}
