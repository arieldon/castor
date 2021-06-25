#include "url.h"

static void
_parse_url_scheme(const char *link, char **scheme)
{
	size_t schemelen;
	const char *p;

	p = strstr(link, "://");
	if (p == NULL) {
		fprintf(stderr, "Invalid URL.\n");
		exit(EXIT_FAILURE);
	} else {
		schemelen = p - link;
		*scheme = (char *)realloc(*scheme, sizeof(char) * (schemelen + 1));
		if (*scheme == NULL) {
			fprintf(stderr, "Unable to allocate scheme for URL.\n");
			exit(EXIT_FAILURE);
		}

		memcpy(*scheme, link, schemelen);
		(*scheme)[schemelen] = '\0';

		if (strncmp(*scheme, "gemini", schemelen) != 0) {
			fprintf(stderr, "Invalid URL.\n");
			exit(EXIT_FAILURE);
		}
	}
}

static void
_parse_url_authority(const char *link, char **authority)
{
	size_t authoritylen;
	const char *p;
	const char *q;

	p = strstr(link, "://");
	if (p == NULL) {
		fprintf(stderr, "Invalid URL.\n");
		exit(EXIT_FAILURE);
	} else {
		/* Skip past "://" to beginning of authority. */
		p += 3;
	}

	q = strchr(p, '/');
	if (q == NULL) {
		q = &link[strlen(link)];
	}

	authoritylen = q - p;
	*authority = (char *)realloc(*authority, sizeof(char) * (authoritylen + 1));
	if (*authority == NULL) {
		fprintf(stderr, "Unable to allocate authority for URL.\n");
		exit(EXIT_FAILURE);
	}

	memcpy(*authority, p, authoritylen);
	(*authority)[authoritylen] = '\0';
}

static void
_parse_url_path(const char *link, char **path)
{
	size_t pathlen;
	const char *p;
	const char *q;

	p = strchr(strstr(link, "://") + 3, '/');
	if (p == NULL) {
		/* No path provided after authority. */
		*path = strdup("/");
		return;
	}

	q = strchr(link, '?');
	if (q == NULL) {
		/* Remainder of string is path. */
		q = &link[strlen(link)];
	}

	pathlen = q - p;
	*path = (char *)realloc(*path, sizeof(char) * (pathlen + 1));
	if (*path == NULL) {
		fprintf(stderr, "Unable to allocate path for URL.\n");
		exit(EXIT_FAILURE);
	}

	memcpy(*path, p, pathlen);
	(*path)[pathlen] = '\0';
}

static void
_parse_url_query(const char *link, char **query)
{
	size_t querylen;
	const char *p;

	p = strchr(link, '?');
	if (p == NULL) {
		*query = NULL;
		return;
	}

	querylen = strlen(p);
	*query = (char *)realloc(*query, sizeof(char *) * (querylen + 1));
	if (*query == NULL) {
		fprintf(stderr, "Unable to allocate query for URL.\n");
		exit(EXIT_FAILURE);
	}

	memcpy(*query, p, querylen);
	(*query)[querylen] = '\0';
}

static char *
_percent_encode_string(char *s)
{
	char *encoded;
	size_t encodedlen;
	size_t n_spaces = 0;

	for (size_t i = 0; i < strlen(s); ++i) {
		if (s[i] == ' ') {
			++n_spaces;
		}
	}
	encodedlen = strlen(s) + (n_spaces * 3);

	encoded = (char *)calloc(encodedlen, sizeof(char));
	if (encoded == NULL) {
		fprintf(stderr, "Unable to allocate query.\n");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0, j = 0; i < encodedlen; ++i, ++j) {
		if (s[j] == ' ') {
			i += 2;
			strcat(encoded, "%20");
		} else {
			encoded[i] = s[j];
		}
	}

	return encoded;
}

struct url *
create_url()
{
	struct url *url = malloc(sizeof(struct url));
	if (url == NULL) {
		fprintf(stderr, "Unable to allocate URL.\n");
		exit(EXIT_FAILURE);
	}

	url->scheme = NULL;
	url->authority = NULL;
	url->path = NULL;
	url->query = NULL;

	return url;
}

void
parse_url(const char *link, struct url *url)
{
	_parse_url_scheme(link, &url->scheme);
	_parse_url_authority(link, &url->authority);
	_parse_url_path(link, &url->path);
	_parse_url_query(link, &url->query);
}

void
free_url(struct url *url)
{
	free(url->scheme);
	free(url->authority);
	free(url->path);
	free(url->query);
	free(url);
}

void
append_query(char *s, struct url *url)
{
	assert(url->query == NULL);

	char *encoded = _percent_encode_string(s);
	size_t encodedlen = strlen(encoded);

	url->query = (char *)calloc(encodedlen + 1, sizeof(char));
	if (url->query == NULL) {
		fprintf(stderr, "Unable to allocate query.\n");
		exit(EXIT_FAILURE);
	}

	url->query[0] = '?';
	strncat(url->query, encoded, encodedlen - 1);

	free(encoded);
}
