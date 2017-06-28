#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "phone_common.h"


#define _MAX_DRIVE 1024
#define _MAX_DIR 1024
#define _MAX_FNAME 1024
#define _MAX_EXT 1024
char* phone_strcpy(char *dest, const char *src, int dest_size)
{
	memset(dest, 0, dest_size);
	return strncpy(dest, src, dest_size - 1);
}

int	  phone_strcmp(const char *str1, const char *str2)
{
	return strcmp(str1, str2);
}

void* phone_malloc (int size)
{
	void *ptr = malloc (size);

	if (ptr != NULL)
		memset (ptr, 0, size);
	return ptr;
}

wchar_t* pj_ansi_to_unicode(const char *utf8, size_t length, wchar_t* outbuf, size_t buf_count)
{
	const char* pc = (const char*)utf8;
	const char* last = pc + length;

	unsigned int b;
	unsigned int num_errors = 0;
	int i = 0;
	int wlen = buf_count/sizeof(wchar_t);

	memset(outbuf, 0, buf_count);

	if(!utf8 || length == 0)
		return NULL;

	while (pc < last && i < wlen)
	{
		b = *pc++;

		if( !b ) break; // 0 - is eos in all utf encodings

		if ((b & 0x80) == 0)
		{
			// 1-byte sequence: 000000000xxxxxxx = 0xxxxxxx
			;
		} 
		else if ((b & 0xe0) == 0xc0) 
		{
			// 2-byte sequence: 00000yyyyyxxxxxx = 110yyyyy 10xxxxxx
			if(pc == last) { outbuf[i++]='?'; ++num_errors; break; }
			b = (b & 0x1f) << 6;
			b |= (*pc++ & 0x3f);
		} 
		else if ((b & 0xf0) == 0xe0) 
		{
			// 3-byte sequence: zzzzyyyyyyxxxxxx = 1110zzzz 10yyyyyy 10xxxxxx
			if(pc >= last - 1) { outbuf[i++]='?'; ++num_errors; break; }

			b = (b & 0x0f) << 12;
			b |= (*pc++ & 0x3f) << 6;
			b |= (*pc++ & 0x3f);
			if(b == 0xFEFF &&
				i == 0) // bom at start
				continue; // skip it
		} 
		else if ((b & 0xf8) == 0xf0) 
		{
			// 4-byte sequence: 11101110wwwwzzzzyy + 110111yyyyxxxxxx = 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
			if(pc >= last - 2) { outbuf[i++]='?'; break; }

			b = (b & 0x07) << 18;
			b |= (*pc++ & 0x3f) << 12;
			b |= (*pc++ & 0x3f) << 6;
			b |= (*pc++ & 0x3f);
			// b shall contain now full 21-bit unicode code point.
			assert((b & 0x1fffff) == b);
			if((b & 0x1fffff) != b)
			{
				outbuf[i++]='?';
				++num_errors;
				continue;
			}
			if( sizeof(wchar_t) == 16 ) // Seems like Windows, wchar_t is utf16 code units sequence there.
			{
				outbuf[i++] = (wchar_t)(0xd7c0 + (b >> 10));
				outbuf[i++] = (wchar_t)(0xdc00 | (b & 0x3ff));
			}
			else if( sizeof(wchar_t) >= 21 ) // wchar_t is full ucs-4 
			{
				outbuf[i++] = (wchar_t)(b);
			}
			else
			{
				assert(0); // what? wchar_t is single byte here?
			}
		} 
		else 
		{
			return NULL;
			assert(0); //bad start for UTF-8 multi-byte sequence"
			++num_errors;
			b = '?';
		}
		outbuf[i++] = (wchar_t)(b);
	}

	return outbuf;
}

int pj_ansi_to_unicode_len(const char *utf8, size_t length)
{
	const char* pc = (const char*)utf8;
	const char* last = pc + length;

	unsigned int b;
	int i = 0;

	if(!utf8 || length == 0)
		return -1;

	while (pc < last)
	{
		b = *pc++;

		if( !b ) break; // 0 - is eos in all utf encodings

		if ((b & 0x80) == 0)
		{
			// 1-byte sequence: 000000000xxxxxxx = 0xxxxxxx
			;
		} 
		else if ((b & 0xe0) == 0xc0) 
		{
			// 2-byte sequence: 00000yyyyyxxxxxx = 110yyyyy 10xxxxxx
			if(pc == last)
			{
				i ++;
				break;
			}

			b = (b & 0x1f) << 6;
			b |= (*pc++ & 0x3f);
		} 
		else if ((b & 0xf0) == 0xe0) 
		{
			// 3-byte sequence: zzzzyyyyyyxxxxxx = 1110zzzz 10yyyyyy 10xxxxxx
			if(pc >= last - 1)
			{
				i ++;
				break;
			}

			b = (b & 0x0f) << 12;
			b |= (*pc++ & 0x3f) << 6;
			b |= (*pc++ & 0x3f);
			if(b == 0xFEFF && i == 0) // bom at start
				continue; // skip it
		} 
		else if ((b & 0xf8) == 0xf0) 
		{
			// 4-byte sequence: 11101110wwwwzzzzyy + 110111yyyyxxxxxx = 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
			if(pc >= last - 2)
			{
				i ++;
				break;
			}

			b = (b & 0x07) << 18;
			b |= (*pc++ & 0x3f) << 12;
			b |= (*pc++ & 0x3f) << 6;
			b |= (*pc++ & 0x3f);
			// b shall contain now full 21-bit unicode code point.
			assert((b & 0x1fffff) == b);
			if((b & 0x1fffff) != b)
			{
				i ++;
				continue;
			}
			if( sizeof(wchar_t) == 16 ) // Seems like Windows, wchar_t is utf16 code units sequence there.
			{
				i ++;
				i ++;
			}
			else if( sizeof(wchar_t) >= 21 ) // wchar_t is full ucs-4 
			{
				i ++;
			}
			else
			{
				assert(0); // what? wchar_t is single byte here?
			}
		} 
		else 
		{
			assert(0); //bad start for UTF-8 multi-byte sequence"
			b = '?';
		}
		i ++;
	}

	return i;
}


void get_current_dir(char *work_dir)
{
	char path_buffer[PJ_MAXPATH] = {0};
	char drive[_MAX_DRIVE];
	char directory[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	int i;

	GetModuleFileNameA(NULL, path_buffer, PJ_MAXPATH);
	_splitpath( path_buffer, drive, directory, fname, ext );
	_makepath( path_buffer, drive, directory, NULL, NULL);

	for(i = 0; i < strlen(path_buffer); i++){
		if(path_buffer[i] == '\\')
			path_buffer[i] = '/';
	}

	if(strchr(path_buffer, '/') == NULL){
		path_buffer[i++] = '/';
		path_buffer[i] = 0;
	}

	phone_strcpy(work_dir, path_buffer, sizeof(path_buffer));
}


void pj_array_erase( void *array,
							unsigned elem_size,
							unsigned count,
							unsigned pos)
{
	assert(count != 0);
	if (pos < count-1) {
		memmove( (char*)array + pos*elem_size,
			(char*)array + (pos+1)*elem_size,
			(count-pos-1)*elem_size);
	}
}

char* pj_unicode_to_ansi( const wchar_t *wstr, size_t len,
								 char *buf, size_t buf_size)
{
	int i = 0;
	int wlen;
	int num_errors = 0;
	unsigned int c;
	const wchar_t *pc = wstr;
	const wchar_t *end;
	PJ_ASSERT_RETURN(wstr && buf, NULL);

	wlen = wcslen(wstr);
	end = (wlen > len/sizeof(wchar_t)) ? (pc + len/sizeof(wchar_t)) : (pc + wlen);
	memset(buf, 0, buf_size);

	for (c = *pc; pc < end; c = *(++pc)) {
		if (c < (1 << 7)) {
			if (i >= buf_size) {
				break;
			}
			buf[i++] = (char)(c);
		} else if (c < (1 << 11)) {
			if (i >= buf_size-1) {
				break;
			}
			buf[i++] = (char)((c >> 6) | 0xc0);
			buf[i++] = (char)((c & 0x3f) | 0x80);
		} else if (c < (1 << 16)) {
			if (i >= buf_size-2) {
				break;
			}
			buf[i++] = (char)((c >> 12) | 0xe0);
			buf[i++] = (char)(((c >> 6) & 0x3f) | 0x80);
			buf[i++] = (char)((c & 0x3f) | 0x80);
		} else if (c < (1 << 21)) {
			if (i >= buf_size-3) {
				break;
			}
			buf[i++] = (char)((c >> 18) | 0xf0);
			buf[i++] = (char)(((c >> 12) & 0x3f) | 0x80);
			buf[i++] = (char)(((c >> 6) & 0x3f) | 0x80);
			buf[i++] = (char)((c & 0x3f) | 0x80);
		} else {
			num_errors = 1;
			break;
		}
	}
	if(num_errors) {
		return NULL;
	} else {
		return buf;
	}
}
