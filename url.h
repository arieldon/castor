#ifndef URL_H
#define URL_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* <SCHEME>://<AUTHORITY>/<PATH>?<QUERY> */
struct url {
	char *scheme;
	char *authority;
	char *path;
	char *query;
};

struct url *create_url();
void parse_url(const char *, struct url *);
void free_url(struct url *);

void append_query(char *, struct url *);

#endif
