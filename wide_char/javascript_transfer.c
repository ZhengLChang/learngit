#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

typedef enum javascript_class_status
{
	LINE_UNPROCESSED,
	LINE_ERROR,
	LINE_EMPTY,
	LINE_COMMENT_BEGIN,
	LINE_COMMENT_END, /*don't support code and comment in same line*/
	LINE_CLASS,
	LINE_CLASS_LEFT_PARENTHESIS,
	LINE_LEFT_PARENTHESIS,
	LINE_RIGHT_PARENTHESIS,
	LINE_SECTION_VALUE,
}class_status;

typedef struct javascript_class_parse_node
{
	char *class_name;
	char *section;
	char *value;
	struct javascript_class_parse_node *next;
}parser_node;

typedef struct javascript_class_parse_head
{
	struct javascript_class_parse_node head;
}parser_head;

typedef int bool_t;
#define BOOL_TRUE 1
#define BOOL_FALSE 0
bool_t list_init(parser_head *list)
{
	if(list != NULL)
	{
		memset(list, 0, sizeof(list));
		return BOOL_TRUE; //success
	}
	return BOOL_FALSE; //false
}

bool_t list_del(parser_head **list)
{
	if(list == NULL || *list == NULL)
		return BOOL_FALSE;
	parser_head *head =  *list;
	parser_node *tmp;
	for(head =  *list, tmp = head->head.next; 
			tmp != NULL; tmp = head->head.next)
	{
		head->head.next = tmp->next;
		free(tmp->section);
		free(tmp->value);
	}
	free(*list);
	*list = NULL;
	return BOOL_TRUE;
}

bool_t node_push(parser_head *list, parser_node *node)
{
	if(list == NULL || node == NULL)
	{
		return BOOL_FALSE;
	}
	node->next = list->head.next;		
	list->head.next = node;
	return BOOL_TRUE;
}
#define ASCIILINESZ 1024
/**
 *   @brief    Remove blanks at the beginning and the end of a string.
 *     @param    str  String to parse and alter.
 *       @return   unsigned New size of the string.
 *        */
/*--------------------------------------------------------------------------*/

#define ISSPACE(n) ((int)(n) == ' ' || (int)(n) == '\r' || (int)(n) == '\n' || \
		(int)(n) == '\t')
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

class_status parser_line(const char *input_line, 
		char *class_name,
		char *section,
		char *value)
{
	class_status stat;
	char *line = NULL, str_name[ASCIILINESZ], str_value[ASCIILINESZ];
	size_t len;

	line = strdup(input_line);
	len = strstrip(line);

	stat = LINE_UNPROCESSED;
	if(len < 1)
	{
		stat = LINE_EMPTY;
	}
	else if(line[0] == '}')
	{
		stat = LINE_RIGHT_PARENTHESIS;
	}
	else if((line[0] == '/' && line[1] == '*') ||
			(line[0] == '/' && line[1] == '/'))
	{
		stat = LINE_COMMENT_BEGIN;
	}
	else if(len >= 2 && line[len - 1] == '/' && line[len - 2] == '*')
	{
		stat = LINE_COMMENT_END;
	}
	else if(sscanf(line, "%[^:]: '%[^\']'", str_name, str_value) == 2)
	{
		stat = LINE_SECTION_VALUE;
		strstrip(str_name);
		strstrip(str_value);
		strcpy(section, str_name);
		strcpy(value, str_value);
	}
	else if(sscanf(line, "%[^=] = %s", str_name, str_value) == 2 ||
		sscanf(line, "%[^:]: {", str_name) == 1)
	{
		stat = LINE_CLASS;
		strstrip(str_name);
		if(class_name[0] != '\0')
			strcat(class_name, ".");
		strcat(class_name, str_name);
	}
	else
	{
		stat = LINE_RIGHT_PARENTHESIS;
	}
	printf("%d\n", stat);
	free(line);
	return stat;
}

parser_head* file_parser_load(char *filename)
{
	FILE *fp;

	char line[ASCIILINESZ + 1];
	char class_name[ASCIILINESZ + 1];
	char section[ASCIILINESZ + 1];
	char value[ASCIILINESZ + 1];

	parser_head *head;
	parser_node *node;

	int last, len;
	if(filename == NULL)
		return BOOL_FALSE;
	
	if((head = (parser_head *)(calloc(1, sizeof(*head)))) == NULL)
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
	memset(class_name, 0, sizeof(class_name));
	memset(section, 0, sizeof(section));
	memset(value, 0, sizeof(section));
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
		switch(parser_line(line, class_name, section, value))
		{
			case LINE_EMPTY:
			case LINE_COMMENT_BEGIN:
			case LINE_COMMENT_END:
				break;
			case LINE_CLASS:
				break;
			case LINE_SECTION_VALUE:
			{
				if(class_name[0] == '\0')
				{
					printf("get section and value but class is empty\n");
					break;
				}
				if((node = (parser_node *)calloc((size_t)1, sizeof(*node))) != NULL)
				{
					node->class_name = strdup(class_name);
					node->section = strdup(section);
					node->value = strdup(value);
					printf("%s.%s=%s\n", node->class_name,
						node->section, 
						node->value);		
					node_push(head, node);
				}
				else
				{
					printf("calloc error: %s\n", strerror(errno));
				}
				break;
			}
			case LINE_RIGHT_PARENTHESIS:
			{
				memset(class_name, 0, sizeof(class_name));
				memset(section, 0, sizeof(section));
				memset(value, 0, sizeof(section));
				break;
			}
			default:
			{
				printf("don't found define\n");
				memset(class_name, 0, sizeof(class_name));
				memset(section, 0, sizeof(section));
				memset(value, 0, sizeof(section));
				break;
			}
		}
		memset(line, 0, ASCIILINESZ);
		last = 0;
	}
	fclose(fp);
	return head;
}

void parser_dump(parser_head *head)
{
	parser_node *tmp;
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
		//fprintf(fp, "%s.%s=%s\n", tmp->class_name,
		//		tmp->section, 
		//		tmp->value);		
		if(strlen(tmp->class_name) > strlen("tips") && strcmp(tmp->class_name + strlen(tmp->class_name) - 4, "tips") == 0)
		{
			fprintf(fp, "tips.%s=%s\n", tmp->section, 
					tmp->value);		
		}
		else
		{
			fprintf(fp, "%s=%s\n", tmp->section, 
				tmp->value);		
		}
	}
	fclose(fp);
	return ;
}

int main(int argc, char **argv)
{
	parser_head *head;
	if(argc != 2)
	{
		printf("usage: %s filename\n", argv[0]);
		return -1;
	}
	head = file_parser_load(argv[1]);
	if(head != NULL)
	{
		parser_dump(head);
		list_del(&head);
	}
	printf("Everything is OK\n");
	return 0;
}










