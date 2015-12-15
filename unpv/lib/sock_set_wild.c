#include	"unp.h"

void
sock_set_wild(struct sockaddr *sa, socklen_t salen)
{
	const void	*wildptr;

	switch (sa->sa_family) {
	case AF_INET: {
		static struct in_addr	in4addr_any;

		in4addr_any.s_addr = htonl(INADDR_ANY);
		wildptr = &in4addr_any;
		break;
	}

#ifdef	IPV6
	case AF_INET6: {
		static struct in6_addr in6addr_any;
		in6addr_any = htonl(INADDR_ANY);
		wildptr = &in6addr_any;
		break;
	}
#endif

	default:
		return;
	}
	sock_set_addr(sa, salen, wildptr);
    return;
}
