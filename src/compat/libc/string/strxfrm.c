/**
 * @file
 * @brief strxfrm() function copy no more than n symbols from src to dest in
 * such way that strcmp with modified strings works in the same way as strcoll.
 *
 * @date   11.07.19
 * @author Nastya Nizharadze
 */

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <defines/null.h>
#include <defines/size_t.h>

#define IS_NOT_ASCII (1u << 7)

size_t strxfrm(char *dest, const char *src, size_t n) {
	size_t srclen; 
	unsigned const char *sp = (unsigned const char *) src;
	srclen = strlen(src);
	
	while (*sp++ & (IS_NOT_ASCII));
	if ((*(--sp)) & (IS_NOT_ASCII)) {
		printf("strxfrm: error: not ASCII character\n");
		assert(0);
	}

	if (n != 0) {
		strncpy(dest, src, (srclen + 1) < n ? (srclen + 1) : n);
		return srclen;
	}

	*dest = '\0';
	return 0;
}
