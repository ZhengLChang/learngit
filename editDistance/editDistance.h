#ifndef EDITDISTANCE_H_
#define EDITDISTANCE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
int EditDistance(const char *s, const int s_size, 
			const char *n, const int n_size);
void EditPath(const char *s, const int s_size, 
			const char *n, const int n_size);


#endif
