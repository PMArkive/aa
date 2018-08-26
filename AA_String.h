#pragma once

#include <stdlib.h>
#include <string.h>

typedef struct {
	char * buf;
	unsigned len;
	unsigned total;
} AA_String;

int AA_String_init(AA_String * as, unsigned initial_size)
{
	AA_String tmp;
	tmp.buf = (char *)malloc(initial_size);
	if (!tmp.buf)
		return 0;
	tmp.len = 0;
	tmp.total = initial_size;
	*tmp.buf = 0;
	*as = tmp;
	return 1;
}

void AA_String_free(AA_String * as)
{
	free(as->buf);
}

int AA_String_move(AA_String * as, AA_String * out)
{
	char * x = (char *)malloc(as->len + sizeof(char));
	if (!x) return 0;
	memcpy(x, as->buf, as->len);
	x[as->len] = 0;
	AA_String_free(as);
	out->buf = x;
	out->len = as->len;
	out->total = as->len + sizeof(char);
	return 1;
}

int AA_String_resize(AA_String * as, unsigned add)
{
	add += sizeof(char); // handle NUL
	char * ret = (char *)realloc(as->buf, add + as->total);
	if (!ret) return 0;
	as->buf = ret;
	as->total += add;
	return 1;
}

int AA_String_concat(AA_String * as, const char * addee, unsigned len)
{
	if (!len) len = (unsigned)strlen(addee);
	if ((as->total - as->len) < len)
	{
		if (!AA_String_resize(as, len + 256))
			return 0;
	}
	memcpy(as->buf + as->len, addee, len);
	as->len = as->len + len;
	as->buf[as->len] = 0;
	return 1;
}
