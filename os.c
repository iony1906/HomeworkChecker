//------------------------------------------------------------------------
// NAME: Viktor Vladimirov
// CLASS: XIa
// NUMBER: 5
// PROBLEM: #1
// FILE NAME: os.c
// FILE PURPOSE: *nix tail implementation, a very bad one at that!
// stdouts the last 10 lines of the files passed as ARG
// or the last 10 lines of the stdin if '-' is an ARG
// or if there are no arguments at all 
//------------------------------------------------------------------------

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#define BUFFER_SIZE 100
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

//------------------------------------------------------------------------
// FUNCTION: void tailstdin(int, int)
// reads the STDIN and writes the last 10 lines of it at the STDOUT
// PARAMETERS:
// int argc- the total program arguments, int k- the position of the '-'
// in the argv
// both are used to make the output formatted by the standard
// (with ==> standard input <== and new lines where needed)
//------------------------------------------------------------------------

void tailstdin(int argc, int k)
{
	//	reads the entire stdin to a very large buffer
	//	and counts the new lines...
	//	then writes the last 10 lines to a second very large buffer
	//	and prints them out
	// 	could probably do it without the second buffer?
	//	will rewrite the algorithm if i have any time left
	
	if(argc>2)
	{		
		write(STDOUT_FILENO, "==> ", 4);
		write(STDOUT_FILENO, "standard input", 15);
		write(STDOUT_FILENO, " <==\n", 5);
	}
	char *buff = (char *) calloc(100000, sizeof(char));
	char *parsed = (char *) calloc(100000, sizeof(char));
	int lines = 0;
	int parsed_pos = 0;	
	int result = 1;
	int i = 0;
	while(result>0)
	{
		result = read(STDIN_FILENO, &buff[i], 1);
		i++;
	}
	
	for(int i = 0; i < strlen(buff); i++)
	{
		if(buff[i] == '\n')
			lines++;
	}
	
	for(int i = 0; i < strlen(buff); i++)
	{
		if(lines-10<=0)
		{
			parsed[parsed_pos] = buff[i];
			parsed_pos++;
		}
		if(buff[i] == '\n')
			lines--;
	}
	
	for(int i = 0; i < strlen(parsed); i++)
	{
		write(STDOUT_FILENO, &parsed[i], 1);
	}
	if(k+1!=argc)write(STDOUT_FILENO, "\n", 1);
}

//------------------------------------------------------------------------
// FUNCTION: void tailfile(char*,int, int)
// reads the file passed as first argument and writes the last 10 lines
// to STDOUT
// PARAMETERS:
// char*- the filename of the file to be tailed
// int argc- the total program arguments, int k- the position of the '-'
// in the argv
// both are used to make the output formatted by the standard
// (with ==> standard input <== and new lines where needed)
//------------------------------------------------------------------------


void tailfile(char * fl, int argc, int k)
{	

	// this segment reads the entire file and counts the new lines in it
	// a VERY VERY slow implementation of the algo but it works
	
	
	int fd = open(fl, O_RDONLY);
	if(fd <= 0)
	{
		char message[200];
		strcpy(message, "tail: cannot open '");
		strcat(message, fl);
		strcat(message, "' for reading");
		perror(message);
		close(fd);
		return;
	}
	int result = 1;
	if(argc>2)
	{		
		write(STDOUT_FILENO, "==> ", 4);
		write(STDOUT_FILENO, fl, strlen(fl));
		write(STDOUT_FILENO, " <==\n", 5);
	}
	int line_count = 0;
	result = 1;
	char buff;
	int chars = 0;
	while(result > 0)
	{
		result = read(fd, &buff, 1);
		if(result < 0)
		{
			char message[200];
			strcpy(message, "tail: error reading '");
			strcat(message, fl);
			strcat(message, "'");
			perror(message);
			close(fd);
			return;
		}
		if(buff=='\n')
			line_count++;
		chars++;
	}
	
	
	// those few lines were supposed to parse the file correctly if it doesnt end in a new line
	// obsolete at the moment, they dont work anymore after the partial read/write fix
	// but they used to work...
	
	bool ends_without_n = 0;
	read(fd, &buff, 1);
	if(buff != '\n')
	{
		line_count++;
		ends_without_n = 1;
	}
	 //check ~line 220
	
	//here we read the file again, from the very beginning and then write the chars only if they are
	//situated in the last 10 lines. again, this is slow...
	
	lseek(fd, 0, SEEK_SET);
	
	result = 1;
	bool first_line_skipped = 0;
	
	while(line_count > 0 && result != 0)
	{
		if(line_count-10<=0&&first_line_skipped==1)
		{
			int terf = 0;
			
			//the fix for partial read and write?
			//the normal tail reads and writes in batches and rereads until the entire batch is completed
			//tried that, but it didnt work
			
			while(terf == 0 && line_count > 0)
				terf = write(STDOUT_FILENO, &buff, 1);
			if(terf < 0){
				char str[] = "tail: error writing 'standard output'";
				perror(str);
				close(fd);
				return;
			}
		}
		
		//first line skipped is a solution for the fact that if the file is <10 lines
		//a new line is written in the very beginning
		//for some reason? i dont remember when exactly this error started occuring
		
		first_line_skipped = 1;
		if(buff == '\n')
			line_count--;
		
		result = 0;
		
		//same thing as above, partial read and write fix
		//came up with this one after reading the write and read documentation for the 42th time
		//after which i thought a lot about the return values and how i can use them
		
		while(result == 0 && line_count > 0)
			result = read(fd, &buff, 1);
		if(result < 0)
		{
			char message[200];
			strcpy(message, "tail: error reading '");
			strcat(message, fl);
			strcat(message, "'");
			perror(message);
			close(fd);
			return;
		}
	}
	
	//			and here we go, the not working close that catches the exception of -1
	//			maybe i am not allowed to open a file 2 times while tailing it?
	//			or is the function called in a different way?
	//			
	//			have been trying to fix this for 2 days now, and has 12+ hours of testing (probably?)
	//			it returs a 0 if it closed correctly, and i cannot find a case
	//			in which it doesnt close correctly, and then it would catch the exception
	

	//			but whats the point in catching the exception at the last test and then it not working
	//			when everything is ok?
	
	result = close(fd);
	
	if(result == -1)
	{
		char message[200];
		strcpy(message, "tail: error reading '");
		strcat(message, fl);
		strcat(message, "': Input/output error\n");
		write(STDERR_FILENO, message, strlen(message));	
	}
	
	if(ends_without_n)
		write(STDOUT_FILENO, "\n", 1);
	if(k+1!=argc)
	{	
		result = write(STDOUT_FILENO, "\n", 1);
	}
}

//------------------------------------------------------------------------
// FUNCTION: int main(int, char**)
// calls the 2 tail functions according to the ARG case
// PARAMETERS:
// int argc- the total count of strings used to call the program
// char** argc- a double dimensional array with the program strings
//------------------------------------------------------------------------

int main(int argc,char **argv)
{
	if(argc==1)
	{
		tailstdin(argc, 0);
	}
	for(int k = 1; k < argc; k++)
	{
		if(strcmp(argv[k], "-")==0)
		{
			tailstdin(argc, k);
		}
		else
		{
			tailfile(&argv[k][0], argc, k);
		}
	}
	return 1;
}