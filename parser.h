#ifndef PARSE_H
#define PARSE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct gemini_header {
	uint8_t status;
	char meta[1025];
};

enum gemini_status {
	GEMINI_INPUT = 1,
	GEMINI_SUCCESS = 2,
	GEMINI_REDIRECT = 3,
	GEMINI_TEMPORARY_FAILURE = 4,
	GEMINI_PERMANENT_FAILURE = 5,
};

void parse_gemini_header(char *response, struct gemini_header *header);

#endif
