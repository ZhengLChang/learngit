#include <pj/pool.h>
#include <pjlib.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	pj_caching_pool caching_pool;
	pj_pool_t *pool = NULL;
	pj_pool_factory *mem = &caching_pool.factory;
	char *str = NULL;
	
	pj_init();
	pj_caching_pool_init(&caching_pool, &pj_pool_factory_default_policy, 0);
	pool = pj_pool_create(mem, "zhj", 1024, 512, NULL);
	if(NULL == pool)
	{
		printf("pj_pool_create error\n");
		return -1;
	}
	if((str = (char *)pj_pool_calloc(pool, 1, 100)) == NULL)
	{
		printf("calloc error\n");
		pj_pool_release(pool);
		return -1;
	}
	snprintf(str, 100, "test pj");
	printf("address:\n");
	if(pool->block_list.buf != NULL)
	printf("str: %p\n, pool.buf: %p\n, pool.cur: %p\n", &str, 
				pool->block_list.buf,
				pool->block_list.cur);
	printf("pool name: %s data: %s\n", pool->obj_name, 
			pool->block_list.buf);
	pj_pool_release(pool);
	pj_caching_pool_destroy(&caching_pool);
	return 0;
}






















