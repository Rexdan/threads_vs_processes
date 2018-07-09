/*
 * compressR_LOLS.c
 * Tested on null.cs.rutgers.edu
 *
 *  Created on: Nov 22, 2016
 *      Authors: Jesse Gatling and Andre Pereira
 *	
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

char *filePath;
int numBytes, numParts;

int main(const int argc, const char **argv)
{
	if(argc > 3)
	{
		printf("ERROR. Too many arguments.\n");
		return EXIT_FAILURE;
	}
	if(argc < 3)
	{
		printf("ERROR. Too few arguments.\n");
		return EXIT_FAILURE;
	}
	int i;
	for(i = 0; i < strlen(argv[2]); i++)
	{
		if(!isdigit(argv[2][i])) break;
	}
	/*If we went through whole string, we have a number!*/
	if(i == strlen(argv[2]))
	{
		numParts = atoi(argv[2]);
		if(numParts <= 0)
		{
			printf("ERROR. Second parameter is not a natural number.\n");
			return EXIT_FAILURE;
		}
	}
	else
	{
		printf("ERROR. Second parameter is illegal.\n");
		return EXIT_FAILURE;
	}

	i = 0;
	filePath = (char*)malloc(sizeof(char));
	strcpy(filePath, argv[1]);
	filePath[strlen(filePath)] = '\0';
	
	int fd = open(filePath, O_RDONLY);
	if(fd == -1)
	{
		fprintf(stderr, "Error Opening File: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	numBytes = lseek(fd, 0, SEEK_END) - 1;
	//printf("Our number of bytes: %d\n", numBytes);
	int status;
	pid_t pid;
	
	const char *command = "./compressR-worker_LOLS";
	char *child_args[5];
	
	/*Have file name passed as first element.*/
	child_args[0] = filePath;

	char num_parts[64], num_bytes[64], num_proc[64];
	
	sprintf(num_parts, "%d", numParts);
	/*Have number of partitions, i.e. processess passed.*/
	child_args[1] = num_parts;
	
	/*Size of file.*/
	sprintf(num_bytes, "%d", numBytes - 1);
	child_args[2] = num_bytes;
	
	/*Being safe.*/
	child_args[4] = '\0';

	for(i = 0; i < numParts; i++)
	{
		pid = fork();
		if(pid < 0)
		{
			fprintf(stderr, "Fork Failed! Here's why: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		else if(pid == 0)
		{
			//printf("hello worlds");
			/*Wanted portability. itoa is not standard.*/
			sprintf(num_proc, "%d", (i+1));
			/*Giving process its number.*/
			child_args[3] = num_proc;
			/*Have to actually code the children files now.*/
			execvp(command, child_args);
			printf("%d\n",errno);
			memset(&num_proc[0], 0, sizeof(num_proc));
		}
		else
		{
			while (wait(&status) != pid);
		}
	}

	return 0;
}

