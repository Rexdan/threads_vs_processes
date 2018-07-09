/*
 * compressR-worker_LOLS.c
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
#include <math.h>

static char *filePath;
int numParts;
int currProc;

void append(char *p, char c)
{
	if(c == '\0') return;
	
	int len;
	
	if(p) len = strlen(p);

	p = realloc(p, len + 2);

	p[len] = c;
}

void cmprssHelper(char *input, char c, int count)
{
	char *temp = (char*)malloc(sizeof(char)*3);
	temp[0] = (count) + '0';
	temp[1] = c;
	temp[2] = '\0';
	count = 1;
	strcat(input, temp);
	free(temp);
}

char* cmprss(char *fileString, char *finaString, int strt, int end){
	//printf("String going into cmprss: %s\n", fileString);
	int i,j,count,max;
	i = strt;
	while(i <= end){
		if(!isalpha(fileString[i])){
			printf("WARNING: %c is not a legal character\n",fileString[i]);
			i++;
			continue;
		}
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
		i = j;
	}
	return finaString;
	
}

int main (const int argc, const char **argv)
{
	int bytesToRead, numBytes, fd, fdR;
	filePath = (char*)malloc(sizeof(char));
	filePath[strlen(filePath)] = '\0';
	strcpy(filePath, argv[0]);
	
	numParts = atoi(argv[1]);
	
	currProc = atoi(argv[3]);
	
	/*...Creating File...*/
	char *name = (char*)malloc(sizeof(char));
	char end[7] = {'_','L','O','L','S'};
	end[6] = '\0';
	name[strlen(name)] = '\0';
	int i;
	//char c;
	int max99 = (int)((ceil(log10(currProc))+1)*sizeof(char));
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
	if(numParts != 1)
	{
		//int max99 = (int)((ceil(log10(currProc))+1)*sizeof(char));
		//char cba[max99];
		sprintf(c, "%d", currProc - 1);
		strcat(end, c);

		/*c = currProc + '0';
		end[6] = c;*/
	}
	strcat(name, end);
	/*The third argument is an octal number that allows for others to read the file.*/
	fdR = open(name, O_RDWR|O_CREAT, 0666);
	if(fdR == -1)
	{
		fprintf(stderr, "Error Creating File: %s\n", strerror(errno));
	}

	int start;
	/*Opening original file.*/
	fd = open(filePath, O_RDONLY);
	numBytes = lseek(fd, 0, SEEK_END) - 1;
	//printf("Our number of bytes: %d\n", numBytes);
	if(fd == -1)
	{
		fprintf(stderr, "Error Opening File: %s\n", strerror(errno));
	}
	if(numBytes % numParts != 0)
	{
		bytesToRead = numBytes/numParts;
		/*After getting the mod, we should divide whatever was modded by the above number to get amount of bytes to be read by last thread.*/
		if(currProc == 1)
		{
			bytesToRead += (numBytes % numParts);
			//printf("BYTES TO READ: %d\n", bytesToRead);
			start = 0;
		}
		else
		{	
			start = (currProc-1) * bytesToRead + 1;
			start += (numBytes % numParts) - 1;
		}
	}
	else
	{
		bytesToRead = numBytes/numParts;
		start = (currProc-1) * bytesToRead;
	}
	char endOfstr[] = "!";
	//printf("Reading %d bytes.\n", bytesToRead);
	char *finalString = (char*)malloc(sizeof(char));
	//finalString[strlen(finalString)] = '\0';
	char *fileString = (char*)malloc(sizeof(char));
	//fileString[strlen(fileString)] = '\0';

	off_t ret;
	ret = lseek(fd, (off_t)start, SEEK_SET);
	
	if(ret == (off_t)-1)
	{
		printf("Error Opening File At Index Number: %d\n", start);
	}
	read(fd, fileString, bytesToRead);
	strcat(fileString,endOfstr);
	finalString = cmprss(fileString, finalString, 0, bytesToRead);
	//printf("Starting at index %d. Writing to file: %s\n", start,finalString);
	write(fdR, finalString, strlen(finalString));
	close(fd);
	close(fdR);
	free(filePath);
	free(name);
	/*free(fileString);
	free(finalString);*/
	return 0;
}
