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

#include "parser.h"
#include "url.h"

#define MAX_BUFFER_LEN	2048


int
main(int argc, char *argv[])
{
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	struct url url;
	struct gemini_header header;

	int n_bytes;
	int sockfd;

	struct addrinfo hints;
	struct addrinfo *remote_addr;
	struct addrinfo *p;

	SSL_CTX *ctx;
	SSL *ssl;
	X509 *cert;

	char req[MAX_BUFFER_LEN];
	char res[MAX_BUFFER_LEN];

	if (argc < 2) {
		fprintf(stderr, "usage: %s [hostname]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((ctx = SSL_CTX_new(TLS_client_method())) == NULL) {
		fprintf(stderr, "SSL_CTX_new() failed.\n");
		exit(EXIT_FAILURE);
	}

	parse_url(argv[1], &url);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(url.authority, GEMINI_PORT, &hints, &remote_addr)) {
		fprintf(stderr, "getaddrinfo() failed.\n");
		exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(remote_addr);

	if ((ssl = SSL_new(ctx)) == NULL) {
		fprintf(stderr, "SSL_new() failed.\n");
		exit(EXIT_FAILURE);
	}

	if (SSL_set_tlsext_host_name(ssl, url.authority) == 0) {
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

	sprintf(req, "gemini://%s%s\r\n",
		url.authority,
		url.path ? url.path : "");
	SSL_write(ssl, req, strlen(req));

	SSL_read(ssl, res, sizeof(res));
	parse_gemini_header(res, &header);

	switch (header.status / 10) {
	case GEMINI_INPUT:
		break;
	case GEMINI_SUCCESS:
		while ((n_bytes = SSL_read(ssl, res, sizeof(res))) != 0) {
			if (n_bytes == -1) {
				fprintf(stderr, "SSL_read() failed.\n");
				exit(EXIT_FAILURE);
			}
			printf("%s", res);
		}
		break;
	case GEMINI_REDIRECT:
		break;
	case GEMINI_TEMPORARY_FAILURE:
	case GEMINI_PERMANENT_FAILURE:
		fprintf(stderr, "%s", header.meta);
		break;
	}

	SSL_shutdown(ssl);
	SSL_free(ssl);
	SSL_CTX_free(ctx);

	free_url(&url);
	close(sockfd);

	exit(EXIT_SUCCESS);
}
