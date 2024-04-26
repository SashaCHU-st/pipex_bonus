#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define PROCESS_NUM 10

int main(int argc, char **argv)
{
	//int process_num = argc - 3
	int pid[argc - 3];
	int pipes[PROCESS_NUM +1][2];
	int i = -1;
	while (++i < PROCESS_NUM + 1)
	{
		if(pipe(pipes[i]) == -1)
		{
			printf("eroor");
			return(1);
		}
	}
	i = -1;
	while(++i < PROCESS_NUM)
	{
		pid[i] = fork();
		if(pid[i] == -1)
		{
			printf("error");
			return(2);
		}
		if(pid[i] == 0)
		{
			///child
			int j = -1;
			while (++j < PROCESS_NUM + 1)
			{
				if(i != j)
					close(pipes[j][0]);
				if(i + 1 != j)
					close(pipes[j][1]);
			}
	int x;
	if(read(pipes[i][0], &x,sizeof(int)) == -1)
		printf("errr3");
	printf("%d, got %d\n", i , x);
	x++;

	if(write(pipes[i + 1][1], &x, sizeof(int)) == -1)
		printf("erooor4");
	printf("%d, sent %d\n", i , x);
	close(pipes[i][0]);
	close(pipes[i + 1][1]);
	return(0);
	}
	}
	int j = -1;
	while (++j < PROCESS_NUM + 1)
	{
		if(j != PROCESS_NUM)
			close(pipes[j][0]);
		if(j != 0)
			close(pipes[j][1]);
	}
	int y = 5;
	printf("final result %d\n", y);
	if(write(pipes[0][1], &y, sizeof(int)) == -1)
		printf("erooror 5\n");
	if(read(pipes[PROCESS_NUM][0], &y, sizeof(int)) ==  -1)
		printf("erooror 6\n");
	printf("final result %d\n", y);

	close(pipes[0][1]);
	close (pipes[PROCESS_NUM][0]);
	while(++i < PROCESS_NUM)
	{
		
		wait(NULL);
	}
	return(0);
}
