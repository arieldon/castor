#include "client.h"

int
connect_to_domain(char *domain)
{
	int client;
	struct addrinfo hints = {0};
	struct addrinfo *remote_addr;
	struct addrinfo *p;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(domain, GEMINI_PORT, &hints, &remote_addr)) {
		fprintf(stderr, "getaddrinfo() failed.\n");
		exit(EXIT_FAILURE);
	}

	for (p = remote_addr; p != NULL; p = p->ai_next) {
		client = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (client == -1) {
			continue;
		}

		if (connect(client, p->ai_addr, p->ai_addrlen) == -1) {
			close(client);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "connect() failed.\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(remote_addr);
	return client;
}
