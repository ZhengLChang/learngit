#include <stdio.h>

int main(void)
{
	char c, state = 0;
	while((c = getchar()) != EOF)	
	{
		switch(state)
		{
			case 0:
			{
				if(c == '/')
				{
					state = 1;
				}
				else
				{
					putchar(c);
				}
				break;
			}
			case 1:
			{
				if(c == '/')
				{
					state = 2;
				}
				else if(c == '*')
				{
					state = 3;
				}
				else
				{
					state = 0;
					putchar('/');
					putchar(c);
				}
				break;
			}
			case 2:
			{
				if(c == '\n')
				{
					state = 0;
					putchar(c);
				}
				break;
			}
			case 3:
			{
				if(c == '*')
				{
					state = 4;
				}
				break;
			}
			case 4:
			{
				if(c == '/')
				{
					state = 0;
				}
				else
				{
					state = 3;
				}
				break;
			}
			default:
			{
				printf("state error!\n");
				break;
			}
		}
	}
	return 0;
}
