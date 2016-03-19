#include <pj/pool.h>
#include <pjlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define COUNT 1024
static unsigned sizes[COUNT];
//static char *p[COUNT];
int main(void)
{
	pj_caching_pool caching_pool;
	pj_pool_t *pool = NULL;
	pj_pool_factory *mem = &caching_pool.factory;
	char *str = NULL;
	int i = 0;
	pj_init();
	pj_caching_pool_init(&caching_pool, &pj_pool_factory_default_policy, 0);
	pool = pj_pool_create(mem, "zhj", 1024, 512, NULL);
	if(NULL == pool)
	{
		return -1;
	}
		
	if((str = (char *)pj_pool_calloc(pool, 1, 100)) == NULL)
	{
		printf("calloc error\n");
		pj_pool_release(pool);
		return -1;
	}
	str[snprintf(str, 99, "%s\n", "hello,world")] = '\0';
	printf("%s", str);
	
	for(i = 0; i < COUNT; i++)
	{
		char *p;
		if((p = (char *)pj_pool_alloc(pool, sizes[i])) == NULL)
		{
			printf("pj_pool_alloc error\n");
			pj_pool_release(pool);
			return -1;
		}
		printf("pj_pool_alloc success %d\n", i);
		*p = '\0';
	}

	pj_pool_release(pool);
	pj_caching_pool_destroy(&caching_pool);
	return 0;
}























