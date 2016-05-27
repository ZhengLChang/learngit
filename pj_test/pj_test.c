#include <pj/pool.h>
#include <pjlib.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
typedef struct
{
	pj_pool_t *pool;
	int a;
}Test;
int main(int argc, char **argv)
{
	pj_caching_pool caching_pool;
	pj_pool_t *pool = NULL;
	pj_pool_factory *mem = &caching_pool.factory;
	Test *mytest;
	
	pj_init();
	pj_caching_pool_init(&caching_pool, &pj_pool_factory_default_policy, 0);
	pool = pj_pool_create(mem, "zhj", 1024, 512, NULL);
	if(NULL == pool)
	{
		printf("pj_pool_create error\n");
		return -1;
	}
	if((mytest = (Test *)pj_pool_calloc(pool, 1, sizeof(Test))) == NULL)
	{
		printf("calloc error\n");
		pj_pool_release(pool);
		return -1;
	}
	mytest->pool = pool;
	mytest->a = 1;
	pj_pool_release(mytest->pool);
	pj_caching_pool_destroy(&caching_pool);
	return 0;
}






















