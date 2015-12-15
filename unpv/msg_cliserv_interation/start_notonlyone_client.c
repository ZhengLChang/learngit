#include <unp.h>
struct stoCliMes
{
	FILE *fp;
	pid_t pid;
	struct stoCliMes *next;
};

void insertTolist(struct stoCliMes **head, FILE *fp, pid_t pid);
struct stoCliMes * findNode(struct stoCliMes *head, pid_t pid);
void _insertToList(struct stoCliMes **head, struct stoCliMes *node);
int  delNode(struct stoCliMes **head, struct stoCliMes *node);
void sighandle(int);

struct stoCliMes *head = NULL;

int main(int argc, char *argv[])
{
	int num = 0;
	FILE *fp = 0;
	if(argc == 1)
	{
		printf("usage: %s [filename] [filename] ...\n", argv[0]);
		return -1;
	}
	signal(SIGCHLD, sighandle);
	for(num = argc; num > 1; num--)
	{
		if((fp = popen("./client_main &", "w")) == NULL)	
		{
			printf("popne error: %s\n", strerror(errno));	
			continue;
		}
		fwrite(argv[argc - 1], strlen(argv[argc - 1]), 1,
					fp);
	}
	sleep(10);
	return 0;
}

void sighandle(int signo)
{
	pid_t pid;
	struct stoCliMes *tem;
	pid = wait(NULL);
	tem = findNode(head, pid);
	if(tem == NULL)
		return ;
	fclose(tem->fp);
	tem->fp = NULL;
	delNode(&head, tem);
		
	return ;
}


void _insertToList(struct stoCliMes **head, struct stoCliMes *node)
{
	node->next = *head;
	*head = node;
	return ;
}

void insertTolist(struct stoCliMes **head, FILE *fp, pid_t pid)
{
	struct stoCliMes *node = malloc(sizeof(struct stoCliMes));
	node->fp = fp;
	node->pid = pid;
	node->next = NULL;
	_insertToList(head, node);
}

struct stoCliMes* findNode(struct stoCliMes *head, pid_t pid)
{
	struct stoCliMes *temNode = head;
	for( ; temNode != NULL; temNode = temNode->next)
	{
		if(temNode->pid == pid)
			return temNode;
	}
	return NULL;
}

int  delNode(struct stoCliMes **head, struct stoCliMes *node)
{
	struct stoCliMes *pre, *cur;
	if(head == NULL || *head == NULL)
	{
		return -1;
	}

	if(*head == node)	
	{
		pre = *head;
		*head = (*head)->next;
		free(pre);
		pre = NULL;
		return 0;
	}
	for(pre = *head, cur = pre->next; ; pre = cur, cur = cur->next)
	{
		if(cur == NULL)
		{
			return -1;
		}
		else if(cur == node)
		{
			pre->next = cur->next;	
			free(cur);
			cur = NULL;
			return 0;
		}
	}
	return 0;
}









