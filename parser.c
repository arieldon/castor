#include "parser.h"

void
parse_gemini_header(char *response, struct gemini_header *header)
{
	header->status = (uint8_t)atoi(response);

	strncpy(header->meta, response + 3, 1024);
	header->meta[1024] = '\0';
}
