/*
 * regsub
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regexp.h>
#include "regmagic.h"
#include "scheme.h"

#undef BUFSIZ
#define BUFSIZ 4
/*
 - regsub - perform substitutions after a regexp match
 */
char *
regsub(rp, source)
const regexp *rp;
const char *source;
{
	register regexp * const prog = (regexp *)rp;
	register char *src = (char *)source;
	register char *dst;
	register char c;
	register int no;
	register size_t len;

	if (prog == NULL || source == NULL) {
		regerror("NULL parameter to regsub");
		return;
	}
	if ((unsigned char)*(prog->program) != MAGIC) {
		regerror("damaged regexp");
		return;
	}

	int bufsiz = BUFSIZ; /* record space alloced for destination buffer */
	char *destbuf = scheme_malloc (bufsiz);
	dst=destbuf;
	char *newptr;

	while ((c = *src++) != '\0') {
		if (c == '&')
			no = 0;
		else if (c == '\\' && isdigit(*src))
			no = *src++ - '0';
		else
			no = -1;

		if (no < 0) {	/* Ordinary character. */
			if (c == '\\' && (*src == '\\' || *src == '&'))
				c = *src++;
			*dst++ = c;
			if ((dst - destbuf) >= bufsiz) {
				/* relocate space */
				bufsiz = bufsiz + BUFSIZ;
				newptr = scheme_realloc (destbuf, bufsiz);
				assert (newptr == destbuf);
			  }
		} else if (prog->startp[no] != NULL && prog->endp[no] != NULL &&
					prog->endp[no] > prog->startp[no]) {
			len = prog->endp[no] - prog->startp[no];
			if (((dst + len) - destbuf) >= bufsiz) {
				/* relocate space */
				int nb = (int) (len / BUFSIZ);
				bufsiz = bufsiz + (nb+1) * BUFSIZ;
				newptr = scheme_realloc (destbuf, bufsiz);
				assert (newptr == destbuf);
			}

			(void) strncpy(dst, prog->startp[no], len);
			dst += len;
			if (*(dst-1) == '\0') {	/* strncpy hit NUL. */
				regerror("damaged match string");
				return NULL;
			}
		}
	}
	*dst++ = '\0';
	return destbuf;
}
