#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>

typedef struct conf_node_t_
{
	struct conf_node_t_ *next;
	char name[10240];
	char data[10240];
}conf_node_t;

typedef struct conf_head_t_
{
	conf_node_t *root;	
}conf_head_t;

int init_conf_head(conf_head_t *head)
{
	if(head == NULL)
		return -1;
	memset((void *)head, 0, sizeof(conf_head_t));
	return 0;
}
int destory_conf(conf_head_t *head)
{
	conf_node_t *node;
	if(head == NULL)
		return -1;
	while((node = head->root) != NULL)
	{
		head->root = node->next;
		free(node);
	}
}
conf_head_t * read_conf(conf_head_t *head, const char *file)
{
	FILE *fp;
	char name[10240], data[10240], buf[10240];
	if(head == NULL && file == NULL)
	{
		return NULL;
	}

	init_conf_head(head);
	fp = fopen(file, "r");
	if(fp == NULL)
	{
		return NULL;
	}
	while(fgets(buf, sizeof(buf), fp))
	{
		conf_node_t *node;
		int i = 0;
		if(buf[0] == '\n' || buf[0] == '\r' || strncmp(buf, "static const wchar_t ", strlen("static const wchar_t ")) != 0)
			continue;
		node = (conf_node_t *)malloc(sizeof(conf_node_t));
		if(node == NULL)
		{
			destory_conf(head);
			return NULL;
		}
		strncpy(node->name, buf + strlen("static const wchar_t "), sizeof(node->name) - 1);
		for(i = 0; i < strlen(node->name); i++)
		{
			if(node->name[i] == '[')
			{
				node->name[i] = '\0';
				break;
			}
		}
		strncpy(node->data, buf + strlen("static const wchar_t ") + strlen(node->name) + 6, sizeof(node->data) - 1);
		for(i = strlen(node->data) - 1; i > 0; i--)
		{
			if(node->data[i] == '}')
			{
				node->data[i] = '\0';
				break;
			}
		}
		node->next = head->root;
		head->root = node;
	}
/*	while(fscanf(fp, "static const wchar_t %s\[ = \{%s};", name, data))	
	{
		conf_node_t *node = (conf_node_t *)malloc(sizeof(conf_node_t));
		if(node == NULL)
		{
			destory_conf(head);
			return NULL;
		}
		strncpy(node->name, name, sizeof(node->name));
		strncpy(node->data, data, sizeof(node->data));
		node->next = head->root;
		head->root = node;
	}
	*/
	return head;
}

conf_node_t *find_name(conf_head_t *head, char *finded_str)
{
	conf_node_t *node;
	if(head == NULL || finded_str == NULL)
	{
		return NULL;
	}
	for(node = head->root; node != NULL; node = node->next)
	{
		if(strcmp(node->name, finded_str) == 0)
		{
			return node;
		}
	}
	return NULL;
}

int main()
{
	conf_head_t standard_head, checked_head;
	conf_node_t *node;
	int positiveCount = 0, negativeCount = 0;
#if 0
	if(!read_conf(&standard_head, "chinese_traditional.txt"))
	{
		printf("read %s failed\n", "chinese_traditional.txt");
	}
#endif
	if(!read_conf(&standard_head, "/disk4/AX/A2x/src/mutilanguge/chinese_string.c"))
	{
		printf("read %s failed\n", "chinese_traditional.txt");
	}
	if(!read_conf(&checked_head, "tmp.txt"))
	{
		printf("read %s failed\n", "tmp.txt");
	}
	/*positive check*/
	for(node = standard_head.root; node != NULL; node = node->next)
	{
/*		printf("I will check %s\t", node->name);*/
		if(find_name(&checked_head, node->name) == NULL)
		{
			printf("\n%s cannot found, data is %s, please check!!!\n", node->name, node->data);
			return -1;
			break;
			//continue;
		}
		else
		{
//			printf("OK\n");
			/*printf(" data : %s\n", node->data);*/
			positiveCount++;
		}
	}
	printf("positive check over, and everything is OK\n");
	printf("positiveCount: %d\n", positiveCount);
	/*negative check*/
	for(node = checked_head.root; node != NULL; node = node->next)
	{
/*		printf("I will check %s\t", node->name);*/
		if(find_name(&standard_head, node->name) == NULL)
		{
			printf("\n%s cannot found, data is %s, please check!!!\n", node->name, node->data);
		//	return -1;
		//	break;
			//continue;
		}
		else
		{
//			printf("OK\n");
			/*printf(" data : %s\n", node->data);*/
			negativeCount++;
		}
	}
	printf("negative check over, and everything is OK\n");
	printf("negativeCount: %d\n", negativeCount);
	return 0;
}
























