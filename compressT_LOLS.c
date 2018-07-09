/*
 * compressT_LOLS.c
 * Tested on null.cs.rutgers.edu
 *
 *  Created on: Nov 22, 2016
 *      Authors: Jesse Gatling and Andre Pereira
 *	
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

static int numParts;
char *filePath;
pthread_t *threads;
static int numBytes;
static int **threadNums;

void append(char *p, char c)
{
	int len;
	
	if(p) len = strlen(p);

	p = realloc(p, len + 2);

	p[len] = c;

	p[len + 1] = '\0';
}


char* cmprssHelper(char *fileString, char *finaString, int strt, int end){
	//char *finaString = (char*)malloc(sizeof(char)*strlen(filString));
	//printf("fileString is: %s\n",fileString);
	//finaString[strlen(finaString)] = '\0';
	int i,j,count,max;
	i = strt;
	// loops while there are char to read
	while(i <= end){
		// if current char is not alpha i is incremented and continues
		if(!isalpha(fileString[i])){
			i++;
			printf("WARNING: %c is not a legal character\n",fileString[i]);
			continue;
		}
		// checks to see how many letters if any are the same
		// count is incremented for each char found to be the same
		// non-alph chars are ignored
		for(j = i+1,count = 1; j<end; j++,count++){
			if(!isalpha(fileString[j])){
				printf("WARNING: %c is not a legal character\n",fileString[j]);
				count--;
			}
			else if(fileString[i] != fileString[j]) break; 
		}
		char c1[2] = {fileString[i],'\0'};
		if(count == 1){
			strcat(finaString,c1);
		}
		
		else if(count == 2){
			strcat(finaString,c1);
			strcat(finaString,c1);
		}
		
		else{
			max = (int)((ceil(log10(count))+1)*sizeof(char));
			char c2[max];
			sprintf(c2, "%d", count);
			//char c2[3] = {(count) + '0','\0'};
			strcat(finaString,c2);
			strcat(finaString,c1);
		}
		
		// i is set to j because j is the current index of the next char that needs to be analyzed
		i = j;
	}
	return finaString;
	
}

void *cmprss(void *ptr)
{
	int *threadNum = (int*)ptr;

	/*We should round up for the default number of reads if odd division.*/
	int bytesToRead, fd, fdR, start;
	char *finalString = (char*)malloc(sizeof(char));
	finalString[strlen(finalString)] = '\0';
	
	fd = open(filePath, O_RDONLY);
	
	if(fd == -1)
	{
		fprintf(stderr, "Error Opening File: %s\n", strerror(errno));
	}

	char *name = (char*)malloc(sizeof(char));
	char end[7] = {'_','L','O','L','S'};
	end[6] = '\0';
	name[strlen(name)] = '\0';
	int i;
	/*char c;
	for(i = 0; i < strlen(filePath); i++)
	{
		c = filePath[i];
		if(c == '.')
		{
			append(name, '_');
		}
		else append(name, c);
	}*/
	int max99 = (int)((ceil(log10(*threadNum))+1)*sizeof(char));
	char c[max99];
	for(i = 0; i < strlen(filePath); i++)
	{
		c[0] = filePath[i];
		if(c[0] == '.')
		{
			append(name, '_');
		}
		else append(name, c[0]);
	}
	if(numParts > 1)
	{
		sprintf(c, "%d", *threadNum - 1);
		strcat(end, c);
		/*int max99 = (int)((ceil(log10(*threadNum))+1)*sizeof(char));
		char cba[max99];
		sprintf(cba, "%d", *threadNum - 1);
		strcat(end, cba);*/
	}
	strcat(name, end);
	/*The third argument is an octal number that allows for others to read the file.*/
	fdR = open(name, O_RDWR|O_CREAT, 0666);
	if(fdR == -1)
	{
		fprintf(stderr, "Error Creating File: %s\n", strerror(errno));
	}
	
	if(numBytes % numParts != 0)
	{
		bytesToRead = (numBytes/numParts);
		/*After getting the mod, we should divide whatever was modded by the above number to get amount of bytes to be read by last thread.*/
		if(*threadNum == 1)
		{
			bytesToRead += numBytes%numParts;
			start = 0;
		}
		else
		{
			start = (*threadNum-1) * bytesToRead + 1;
			start += (numBytes % numParts) - 1;
		}
	}
	else
	{
		bytesToRead = (numBytes/numParts);
		start = (*threadNum-1) * bytesToRead;
	}

	off_t ret;
	ret = lseek(fd, (off_t)start, SEEK_SET);
	
	if(ret == (off_t)-1)
	{
		printf("Error Opening File At Index Number: %d\n", start);
	}
	
	char endOfstr[] = "!";
	char *fileString = (char*)malloc(sizeof(char));
	read(fd, fileString, bytesToRead);
	//printf("READING: %s\n", fileString);
	strcat(fileString,endOfstr);
	finalString = cmprssHelper(fileString,finalString, 0, strlen(fileString)-1);
	//printf("WRITING: %s\n", finalString);
	write(fdR, finalString, strlen(finalString));
	//free(finalString);
	close(fd);
	close(fdR);
	pthread_exit(NULL);
}

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
	threads = (pthread_t*)malloc(sizeof(pthread_t)*numParts);
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
	threadNums = (int**)malloc(sizeof(int*)*numParts);

	while(i < numParts)
	{
		/*Need to allocate memory for each thread number because the address of i needs to be different for each thread...*/
		int *threadNum = (int*)malloc(sizeof(int));
		*threadNum = i + 1;
		threadNums[i] = threadNum;
		int err = pthread_create(&threads[i], NULL, cmprss, (void*)(threadNum));
		if(err != 0)
		{
			fprintf(stderr, "Error Creating Thread: %s\n", strerror(errno));
			/*We try again.*/
			continue;
		}
		i++;
	}
	/*This will destroy all active threads.*/
	for(i = 0; i < numParts; i++)
	{
		pthread_join(threads[i], NULL);
	}
	for(i = 0; i < numParts; i++) free(threadNums[i]);
	free(threadNums);
	free(filePath);
	close(fd);
	return 0;
}
