#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0
   for ternary operator(?:)
	 GNU extension to C allows omitting the second operand, and using implicitly the first operand as the second also:

	 	a = x ? : y;
	The expression is equivalent to
		
		a = x ? x : y; 
	
	except that if x is an expression, it is evaluated only once. The difference is significant if evaluating the expression has side effects. This shorthand form is sometimes known as the Elvis operator in other languages.
#endif

int main(void)
{
	const char *str0 = 0 ? : "";
	const char *str1 = 3 ? : "";
	if(str0 == NULL)
	{
		printf("after 0 ? : \"\", str0 == NULL\n");
	}
	else
	{
		printf("after 0 ? : \"\", str0 != NULL, and the len of str0: %d\n", strlen(str0));
       	}
	if(str1 == NULL)
	{
		printf("after 1 ? : \"\", str1 == NULL\n");
	}
	else
	{
		printf("after 1 ? : \"\", str1 != NULL, %p\n", str1);
#if 0
		printf("after 1 ? : \"\", str1 != NULL, and the len of str1: %d\n", strlen(str1));
		printf("after 1 ? : \"\", str1 != NULL, and the data of str1: %s\n", str1);
#endif
	}
#if 0
	else
	{
		printf("after 1 ? : \"\", str1 != NULL, and the len of str1: %d\n", strlen(str1));
	}
#endif
	return 0;
}
