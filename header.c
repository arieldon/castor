#include "header.h"

void
parse_gemini_header(char *response, struct gemini_header *header)
{
	char *crlf = strstr(response, "\r\n");

	header->status = (uint8_t)atoi(response);

	if (crlf == NULL) {
		strncpy(header->meta, response + 3, 1021);
	} else {
		strncpy(header->meta, response + 3,
			(size_t)(crlf - response - 3));
	}
	header->meta[1024] = '\0';
}
