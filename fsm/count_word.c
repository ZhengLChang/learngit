#include <stdio.h>
#define OUT 0
#define IN 1

int main(void)
{
	char c, state;
	int count = 0;
	state = OUT;
	while(scanf("%c", &c) != EOF && c != '\n')
	{
		switch(state)
		{
			case OUT:
				if(c == ' ' || c == '\t')
				{
					state = OUT;
				}
				else
				{
					state = IN;
					count++;
				}
			case IN:
				if(c != ' ' && c != '\t')
					state = IN;
				else
					state = OUT;
		}
	}
	printf("the num of word: %d\n", count);
	return 0;
}
