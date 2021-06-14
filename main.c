#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAX_BUFFER_LEN	2048


int
main(int argc, char *argv[])
{
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	int n_bytes;
	int sockfd;
	int status;

	struct addrinfo hints;
	struct addrinfo *remote_addr;
	struct addrinfo *p;

	SSL_CTX *ctx;
	SSL *ssl;
	X509 *cert;

	char *hostname;
	char *port;
	char req[MAX_BUFFER_LEN];
	char res[MAX_BUFFER_LEN];

	if ((ctx = SSL_CTX_new(TLS_client_method)) == NULL) {
		fprintf(stderr, "SSL_CTX_new() failed.\n");
		exit(EXIT_FAILURE);
	}

	if (argc < 2) {
		fprintf(stderr, "usage: %s [hostname]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	hostname = argv[1];
	port = "443";

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(hostname, port, &hints, &remote_addr))) {
		fprintf(stderr, "getaddrinfo() failed.\n",
			gai_strerror(status));
		return -1;
	}

	for (p = remote_addr; p != NULL; p = p->ai_next) {
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd == -1) {
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "connect() failed.\n");
		return -1;
	}

	freeaddrinfo(remote_addr);

	if ((ssl = SSL_new(ctx)) == NULL) {
		fprintf(stderr, "SSL_new() failed.\n");
		exit(EXIT_FAILURE);
	}

	if (SSL_set_tlsext_host_name(ssl, hostname) == 0) {
		fprintf(stderr, "SSL_set_tlsext_host_name() failed.\n");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	if (SSL_set_fd(ssl, sockfd) == 0) {
		fprintf(stderr, "SSL_set_fd() failed.\n");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	if (SSL_connect(ssl) == -1) {
		fprintf(stderr, "SSL_connect() failed.\n");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	cert = SSL_get_peer_certificate(ssl);
	if (cert == NULL) {
		fprintf(stderr, "SSL_get_peer_certificate() failed.\n");
		exit(EXIT_FAILURE);
	}
	X509_free(cert);

	sprintf(req, "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
		hostname);
	SSL_write(ssl, req, strlen(req));

	while ((n_bytes = SSL_read(ssl, res, sizeof(res))) != 0) {
		if (n_bytes == -1) {
			fprintf(stderr, "SSL_read() failed.\n");
			exit(EXIT_FAILURE);
		}

		printf("(%d) %s", n_bytes, res);
		memset(res, 0, sizeof(res));
	}

	SSL_shutdown(ssl);
	SSL_free(ssl);
	SSL_CTX_free(ctx);
	close(sockfd);

	exit(EXIT_SUCCESS);
}
