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
	gemini_input = 1,
	gemini_success = 2,
	gemini_redirect = 3,
	gemini_temporary_failure = 4,
	gemini_permanent_failure = 5,
};

void parse_gemini_header(char *response, struct gemini_header *header);

#endif
