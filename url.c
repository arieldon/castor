#include "url.h"

static void
_parse_url_scheme(const char *link, char **scheme)
{
	const char *p;

	p = strstr(link, "://");
	if (p == NULL) {
		fprintf(stderr, "Invalid URL.\n");
		exit(EXIT_FAILURE);
	} else {
		*scheme = (char *)calloc(sizeof(char), p - link + 1);
		if (*scheme == NULL) {
			fprintf(stderr, "Unable to allocate scheme for URL.\n");
			exit(EXIT_FAILURE);
		}

		memcpy(*scheme, link, p - link);
		scheme[p - link] = '\0';

		if (strncmp(*scheme, "gemini", p - link) != 0) {
			fprintf(stderr, "Invalid URL.\n");
			exit(EXIT_FAILURE);
		}
	}
}

static void
_parse_url_authority(const char *link, char **authority)
{
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

	*authority = (char *)calloc(sizeof(char), q - p + 1);
	if (*authority == NULL) {
		fprintf(stderr, "Unable to allocate authority for URL.\n");
		exit(EXIT_FAILURE);
	}

	memcpy(*authority, p, q - p);
	authority[q - p] = '\0';
}

static void
_parse_url_path(const char *link, char **path)
{
	const char *p;
	const char *q;

	p = strchr(strstr(link, "://") + 3, '/');
	if (p == NULL) {
		/* No path provided after authority. */
		*path = NULL;
		return;
	}

	q = strchr(link, '?');
	if (q == NULL) {
		/* Remainder of string is path. */
		q = &link[strlen(link)];
	}

	*path = (char *)calloc(sizeof(char), q - p + 1);
	if (*path == NULL) {
		fprintf(stderr, "Unable to allocate path for URL.\n");
		exit(EXIT_FAILURE);
	}

	memcpy(*path, p, q - p);
	path[q - p] = '\0';
}

static void
_parse_url_query(const char *link, char **query)
{
	const char *p;

	p = strchr(link, '?');
	if (p == NULL) {
		*query = NULL;
		return;
	}

	*query = (char *)calloc(sizeof(char), (strlen(p) + 1));
	if (*query == NULL) {
		fprintf(stderr, "Unable to allocate query for URL.\n");
		exit(EXIT_FAILURE);
	}

	memcpy(*query, p, strlen(p));
	query[strlen(p)] = '\0';
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
free_url(struct url *link)
{
	free(link->scheme);
	free(link->authority);
	free(link->path);
	free(link->query);
}
