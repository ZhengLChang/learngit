#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <errno.h>
#include <fcntl.h>
#include <locale.h>

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

int split(char *split_str, const char *delim, char get_str[][200], int *num)
{
	int i = 0;
	char *tok;
	if(split_str == NULL ||
			delim == NULL ||
			get_str == NULL ||
			num == NULL)
	{
		return 0;
	}
	if((tok = strtok(split_str, delim)) != NULL)
	{
		strncpy(get_str[i], tok, sizeof(get_str[i]));		
		for(i = 1; i < *num; i++)
		{
			if((tok = strtok(NULL, delim)) != NULL)
			{
				strncpy(get_str[i], tok, sizeof(get_str[i]));		
			}
			else
			{
				break;
			}
		}
		*num = i;
	}
	return 1;
}

int hex_str_to_bin(char *hex)
{
	int j = 0, num = 0;
	for(j = 0; j < strlen(hex); j++)
	{
		if(hex[j] == '0' && hex[j] == 'x')
		{
			j += 2;
		}
		if(hex[j] >= '0' && hex[j] <= '9')
		{
			num *= 16;
			num += hex[j] - '0';
		}
		else if(hex[j] >= 'a' && hex[j] <= 'f')
		{
			num *= 16;
			num += hex[j] - 'a' + 10;
		}
	}
	return num;
}

int main(int argc, char **argv)
{
	conf_head_t standard_head;
	conf_node_t *node;
	int positiveCount = 0, negativeCount = 0;
	FILE *fp = NULL;
	wchar_t str_Contacts[10240];
	if(argc != 2)
	{
		printf("usage: %s [head file name]\n", argv[0]);
		return -1;
	}
	setlocale(LC_ALL, "zh_CN.UTF-8");
	if(!read_conf(&standard_head, argv[1]))
	{
		printf("read %s failed\n", "chinese_traditional.txt");
		return -1;
	}
	if((fp = fopen("./output.txt", "w")) == NULL)
	{
		printf("fopen error: %s\n", strerror(errno));
		return -1;
	}
	for(node = standard_head.root; node != NULL; node = node->next)
	{
		int i, n;
		char get_str[200][200];
		memset(str_Contacts, 0, sizeof(str_Contacts));
		fwprintf(fp, L"%s = ", node->name + 4);
		wprintf(L"%s\n", node->data);
		n = sizeof(get_str)/sizeof(get_str[0]);
		if(split(node->data, ", ", get_str, &n))
		{
			wprintf(L"n = %d\n", n);
			for(i = 0; i < n; i++)
			{
				str_Contacts[i] = hex_str_to_bin(get_str[i]);
		       	}
		}
		fwprintf(fp, L"%ls\n", str_Contacts);
	}
	printf("Everything is OK\n");
	return 0;
}
























