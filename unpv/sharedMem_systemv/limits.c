#include <unp.h>

#define MAX_NIDS 4096

int main(int argc, char **argv)
{
	int i, j, shmid[MAX_NIDS];
	void *addr[MAX_NIDS];
	//unsigned long size;
	for(i = 0; i <= MAX_NIDS; i++)
	{
		if((shmid[i] = shmget(IPC_PRIVATE, 1024, IPC_CREAT)) < 0)
		{
			printf("%d identifiers open at once\n", i);
			break;
		}
	}
	for(j = 0; j < i; j++)
	{
		shmdt(addr[j]);
		if(shmctl(shmid[j], IPC_RMID, NULL) < 0)
		{
			printf("shmctl error: %s\n", strerror(errno));
			return -1;
		}
	}

/*
	for(i = 0; i <= MAX_NIDS; i++)
	{
		if((shmid[i] = shmget(IPC_PRIVATE, 1024, IPC_CREAT)) < 0)
		{
			printf("shmget error: %s\n", strerror(errno));
			return -1;
		}
		if((addr[i] = shmat(shmid[i], NULL, 0)) == (void *)-1)
		{
			printf("%d shared memory segments at once\n", i);
			if(shmctl(shmid[i], IPC_RMID, NULL) < 0)
			{
				printf("shmctl error: %s\n", strerror(errno));
				return -1;
			}
			break;
		}
	}
	for(j = 0; j < i; j++)
	{
		shmdt(addr[j]);
		if(shmctl(shmid[i], IPC_RMID, NULL) < 0)
		{
			printf("shmctl error: %s\n", strerror(errno));
			return -1;
		}
	}
	*/
	return 0;
}
