#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ASCIILINESZ 1024

#define ISSPACE(n) ((int)(n) == ' ' || (int)(n) == '\r' || (int)(n) == '\n' || \
		(int)(n) == '\t')
typedef struct listNode_
{
	char *name;
	char *value;
	struct listNode_ *next;
}listNode;

typedef struct listHead_
{
	listNode head;
}listHead;

typedef int bool_t;
#define BOOL_TRUE 1
#define BOOL_FALSE 0

char *toHumpName(char *str)
{
	return str;
}

bool_t list_init(listHead *list)
{
	if(list != NULL)
	{
		memset(list, 0, sizeof(*list));
		return BOOL_TRUE; //success
	}
	return BOOL_FALSE; //false
}


bool_t list_del(listHead **list)
{
	if(list == NULL || *list == NULL)
		return BOOL_FALSE;
	listHead *head =  *list;
	listNode *tmp;
	for(head =  *list, tmp = head->head.next; 
			tmp != NULL; tmp = head->head.next)
	{
		head->head.next = tmp->next;
		free(tmp->name);
		free(tmp->value);
	}
	free(*list);
	*list = NULL;
	return BOOL_TRUE;
}

bool_t node_push(listHead *list, listNode *node)
{
	if(list == NULL || node == NULL)
	{
		return BOOL_FALSE;
	}
	node->next = list->head.next;		
	list->head.next = node;
	return BOOL_TRUE;
}
static unsigned strstrip(char * s)
{
	char *last = NULL ;
	char *dest = s;

	if (s==NULL) return 0;
	last = s + strlen(s);
	while (ISSPACE((int)*s) && *s) 
		s++;
	while (last > s) {
		if (!ISSPACE((int)*(last-1)))
			break ;
		last -- ;
	}
	*last = (char)0;
	memmove(dest,s,last - s + 1);
	return last - s;
}

bool_t parser_line(const char *input_line, 
		char *name,
		char *value)
{
	char *line = NULL, str_name[ASCIILINESZ], str_value[ASCIILINESZ];
	size_t len;

	line = strdup(input_line);
	len = strstrip(line);

	if(sscanf(line, "\"%[^\"]\":\"%[^\"]\"", str_name, str_value) == 2)
	{
		strstrip(str_name);
		strstrip(str_value);
		strcpy(name, str_name);
		strcpy(value, str_value);
		return BOOL_TRUE;
	}
//	printf("name: %s\n", str_name);
//	printf("error: %s\n", line);
	return BOOL_FALSE;
}


listHead* file_parser_load(char *filename)
{
	FILE *fp;

	char line[ASCIILINESZ + 1];
	char name[ASCIILINESZ + 1];
	char value[ASCIILINESZ + 1];

	listHead *head;
	listNode *node;

	int last, len;
	if(filename == NULL)
		return NULL;
	
	if((head = (listHead *)(calloc(1, sizeof(*head)))) == NULL)
	{
		printf("calloc error: %s\n", strerror(errno));
		return NULL;
	}

	if((fp = fopen(filename, "r")) == NULL)	
	{
		printf("read file %s error: %s\n", filename, strerror(errno));
		return NULL;
	}
	
	memset(line, 0, sizeof(line));	
	memset(name, 0, sizeof(name));
	memset(value, 0, sizeof(value));
	last = 0;
	while(fgets(line+last, ASCIILINESZ - last, fp) != NULL)
	{
		len = (int)strlen(line) - 1;
		if(len <= 0)
		{
			continue;
		}
		if(line[len] != '\n' && !feof(fp))
		{
			printf("input line too long\n");
			list_del(&head);
			fclose(fp);
			return NULL;
		}
		while((len >= 0) &&
				((line[len] == '\n' || line[len] == ' ')))
		{
			line[len] = '\0';
			len--;
		}
		if(len < 0)
		{
			len = 0;
		}
		if(line[len] == '\\')
		{
			last = len;
			continue;
		}
		else
		{
			last = 0;
		}
		if(parser_line(line, name, value))
		{
			if(name[0] == '\0')
			{
				printf("get section and value but class is empty\n");
				break;
			}
			if((node = (listNode *)calloc((size_t)1, sizeof(*node))) != NULL)
			{
				node->name = strdup(name);
				node->value = strdup(value);
				node_push(head, node);
			}
			else
			{
				printf("calloc error: %s\n", strerror(errno));
			}
		}
		memset(line, 0, ASCIILINESZ);
		last = 0;
	}
	fclose(fp);
	return head;
}
void parser_dump(listHead *head)
{
	listNode *tmp;
	FILE *fp;
	if(head == NULL)
	{
		return ;
	}

	if((fp = fopen("./output", "w")) == NULL)
	{
		printf("fopen error: %s\n", strerror(errno));
		return ;
	}
	for(tmp = head->head.next; 
		tmp != NULL; tmp = tmp->next)
	{
		fprintf(fp, "%s: \'%s\',\n", tmp->name, 
				tmp->value);		
	}
	fclose(fp);
	return ;
}

void parser_dump_given_name(listHead *head, const int argc, char **argv)
{
	listNode *tmp;
	FILE *fp;
	int i = 0;
	if(head == NULL || argc <= 0 || argv == NULL || *argv == NULL)
	{
		return ;
	}

	if((fp = fopen("./output", "w")) == NULL)
	{
		printf("fopen error: %s\n", strerror(errno));
		return ;
	}
	for(i = 1; i < argc; i++)
		for(tmp = head->head.next; 
			tmp != NULL; tmp = tmp->next)
	{
		if(strcmp(argv[i], tmp->name) == 0)
		fprintf(fp, "%s: \'%s\',\n", tmp->name, 
					tmp->value);		
	}
	fclose(fp);
	return ;
}

int main(int argc, char **argv)
{
	listHead *head;
	if(argc < 2)
	{
		printf("usage: %s filename\n", argv[0]);
		return -1;
	}
	head = file_parser_load(argv[1]);
	if(head != NULL)
	{
		//parser_dump_given_name(head, argc, argv);
		parser_dump(head);
		list_del(&head);
	}
	printf("Everything is OK\n");
	return 0;
}
