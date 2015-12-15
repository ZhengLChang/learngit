#include <unp.h>
struct readId
{
	char key[100], owner[100], perms[100], bytes[100], nattch[5], status[20];
	int shmid;
};

int main(int argc, char *argv[])
{
	struct stat stat;
	char *ptr;
	struct readId Id_p;
	int Num = 0;
	int fd;

	if(argc != 2)
	{
		printf("usage: shmwrite <name>\n");
		return -1;
	}
	else
	{
		printf("argv[0] = %s, argv[1] = %s\n", argv[0], argv[1]);
	}
	if((fd = open(argv[1], O_RDWR)) < 0)
	{
		printf("%s open error: %s\n", argv[1], strerror(errno));
		return -1;
	}
	if(fstat(fd, &stat) < 0)
	{
		printf("fstat error: %s\n", strerror(errno));
		return -1;
	}
	printf("size is %d\n", (int)stat.st_size);
	if((ptr = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) ==
			MAP_FAILED)
	{
		printf("mmap error: %s\n", strerror(errno));
		return -1;
	}
	close(fd);
	/*
	for(i = 0; i < stat.st_size; i++)
	{
		printf("%c", ptr[i]);
	}
	*/
	for(; ptr != NULL && *ptr != '\0'; )
	{
		Num = sscanf(ptr, "%s %d %s %s %s %s\n", Id_p.key, &Id_p.shmid, Id_p.owner, Id_p.perms, Id_p.bytes, Id_p.nattch);
		if(Num < 0)
		{
			int  i = 0;
			printf("%x %d %s\n", (unsigned int)ptr, *ptr, ptr);
			for(i = 0; ptr[i] != '\0'; i++)
			{
				printf("%x\t", ptr[i]);	
			}
			printf("\n");
			return -1;
		}
//		size = strlen(Id_p.key) + strlen(Id_p.shmid) + strlen(Id_p.owner) + strlen(Id_p.perms) + strlen(Id_p.bytes) + strlen(Id_p.nattch) + 6;
//
		if(shmctl(Id_p.shmid, IPC_RMID, NULL) < 0)
		{
			printf("shmctl error: %s\n", strerror(errno));
			return -1;
		}
		else
		{
			printf("rm %d success\n", Id_p.shmid);
		}
		ptr = strstr(ptr, "\n");
		if(ptr != NULL)
			ptr++;
	}
	return 0;
	
}
















