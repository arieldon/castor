#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "parser.h"
#include "url.h"

#define MAX_BUFFER_LEN	2048
#define MAX_REDIRECTS	5


int
main(int argc, char *argv[])
{
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	int sockfd;
	int n_bytes = 0;
	int n_redirects = 0;

	SSL_CTX *ctx;
	SSL *ssl;

	struct url url;
	struct gemini_header header;

	char req[MAX_BUFFER_LEN];
	char res[MAX_BUFFER_LEN];

	if (argc < 2) {
		fprintf(stderr, "usage: %s [domain]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	parse_url(argv[1], &url);
	sockfd = connect_to_domain(url.authority);
	secure_connection(sockfd, &ctx, &ssl, url.authority);

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
