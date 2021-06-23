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

void
secure_connection(int client, SSL_CTX **ctx, SSL **ssl, char *domain)
{
	X509 *cert;

	*ctx = SSL_CTX_new(TLS_client_method());
	if (ctx == NULL) {
		fprintf(stderr, "SSL_CTX_new() failed.\n");
		exit(EXIT_FAILURE);
	}

	*ssl = SSL_new(*ctx);
	if (ssl == NULL) {
		fprintf(stderr, "SSL_new() failed.\n");
		exit(EXIT_FAILURE);
	}

	if (SSL_set_tlsext_host_name(*ssl, domain) == 0) {
		fprintf(stderr, "SSL_set_tlsext_host_name() failed.\n");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	if (SSL_set_fd(*ssl, client) == 0) {
		fprintf(stderr, "SSL_set_fd() failed.\n");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	if (SSL_connect(*ssl) == -1) {
		fprintf(stderr, "SSL_connect() failed.\n");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	cert = SSL_get_peer_certificate(*ssl);
	if (cert == NULL) {
		fprintf(stderr, "SSL_get_peer_certificate() failed.\n");
		exit(EXIT_FAILURE);
	}

	X509_free(cert);
}
