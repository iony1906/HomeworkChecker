//------------------------------------------------------------------------
// NAME: Dimitar Stoilov
// CLASS: XIa
// NUMBER: 8
// PROBLEM: #1
// FILE NAME: tail.c
// FILE PURPOSE:
// Реализация на функцията tail
// 
//------------------------------------------------------------------------
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>

void func(int argc, char **argv, char buff, int count, int fd, int result, int k);

int main(int argc, char **argv){
	char buff;
	int count = 0;
	int fd = 1;
	int result = 1;
	int f = 1;
	int rf;
	if(argc > 1){
		
		for(int k = 1; k < argc; k++)
		{	
			if(strstr("-", argv[k]) == NULL){
				fd = open(argv[k], O_APPEND);
				
				// Проверка дали файлът може да се отвори
				if(fd == -1){
					write(2, "tail: cannot open '", 19);
					write(2,argv[k], strlen(argv[k]));
					write(2, "' for reading: ", 15);
					perror("");
				}
				// Проверка дали файлът може да се прочете
				rf = read(fd, &buff, 1);
				if(rf == -1 && fd > 1){
					if(argc>2){
						write(1, "\n==> ", 4);
						write(1, argv[k], strlen(argv[k]));
						write(1, " <==\n", 5);
					}
					write(2, "tail: error reading '", 21);
					write(2,argv[k], strlen(argv[k]));
					write(2, "': ", 3);
					perror("");
				}
		
				close(fd);
			}
			
			if(argc>2 && fd>1 && rf == 1)
			{	

				if(strstr("-", argv[k]) != NULL) {
					if(f == 0){
						write(1, "\n", 1);
						//f = 1;
					}
					f = 0;

					write(1, "==> ", 4);
					write(1, "standart input", 15);
					write(1, " <==", 5);
				}else{
					if(f == 0){
						write(1, "\n", 1);
					}
					f = 0;
					write(1, "==> ", 4);
					write(1, argv[k], strlen(argv[k]));
					write(1, " <==\n", 5);
				}
				
			}
			// Проверка дали в комадния редсе срещя “-“
			if(strstr("-", argv[k]) != NULL) {
				if(argc != 2){
					write(1, "\n", 1);
				}
				int size = 100;
				// Заделям динамично памет в която ще записвам прочетеното от стандартния вход
				char *arr = malloc(size * sizeof(char));
				int d = 0;
				while(result > 0){
					result = read(0, &buff, 1);
					arr[d] = buff;
					d++;
					if(d == size){
						size*=2;
						arr = (char *) realloc(arr, size);
					}
				}
				d--;
				arr[d] = '\0';
				for(int i = 0; i < d; i++){
					char buff = arr[i];
					if(buff == '\n'){
						count++;
					}
				}

				result = 1;
				count -= 9;
				int count1 = 0;
				int s=0;
				if(arr[d-1]=='\n'){
					count--;
					s = 1;
					d--;
				}

				if(count <= 0){
					for(int i = 0; i < d; i++){
						char buff = arr[i];
						write(1, &buff, 1);
					}
					if(s==1){
						write(1, "\n", 1);
					}			
				}else{
					for(int i = 0; i < d; i++){
						char buff = arr[i];
						if(count1 >= count){
							write(1, &buff, 1);
						}
						if(buff == '\n'){
							count1++;
						}
					}
					if(s==1){
						write(1, "\n", 1);
					}	
				}
				free(arr);
			}else{
				if(fd >1 && rf == 1){
					int fc = 1;
					func(argc, argv, buff, count, fd, result, k);
					
					fc = close(fd);
					// Проверка дали файла може да се затваря успешно
					if(fc == -1){
						write(2, "tail: error reading '", 21);
						write(2, argv[k], strlen(argv[k]));
						write(2, "': ", 3);
						perror("");
					}
				}
			}
		}
	}else{
		int size = 100;
		// Заделям динамично памет в която ще записвам прочетеното от стандартния вход
		
		char *arr = malloc(size * sizeof(char));
		int d = 0;
		while(result > 0){
			result = read(0, &buff, 1);
			arr[d] = buff;
			d++;
			if(d == size){
				size *= 2;
				arr = (char *) realloc(arr, size);
			}
		}
		d--;
		arr[d] = '\0';
		for(int i = 0; i < d; i++){
			char buff = arr[i];
			if(buff == '\n'){
				count++;
			}
		}

		result = 1;
		count -= 9;
		int count1 = 0;
		if(arr[d-1]=='\n'){
			count--;
		}

		if(count <= 0){
			for(int i = 0; i < d; i++){
				char buff = arr[i];
				write(1, &buff, 1);
			}		
		}else{
			for(int i = 0; i < d; i++){
				char buff = arr[i];

				if(count1 >= count){
					write(1, &buff, 1);
				}
				if(buff == '\n'){
					count1++;
				}
			}
		}
			free(arr);
		
	}
	// Изкарване на грешка в случай че прочетените данни не могат да се запишат в стандартния изход
	int w = write(1, 0, 0);
	if(w==-1){
		perror("tail: error writing 'standard output'");
	}
	return 0;
}
void func(int argc, char **argv, char buff, int count, int fd, int result, int k){

//------------------------------------------------------------------------
// FUNCTION: func (име на функцията)
// Отваря даден текстов файл, чете записаните в него данни елемент по елемент и 
// записва последните десет реда в стандартния изход
// PARAMETERS:
// int argc, char **argv, char buff, int count, int fd, int result, int k
// 
//------------------------------------------------------------------------

	// Отваряме файла който е подаден като аргумент в командния ред
	fd = open(argv[k], O_APPEND);
	count = 0;
	result = 1;
	// Намираме броя на новите редове
	while(result > 0){
        if(buff=='\n'){
			count++;
		}
		result = read(fd, &buff, 1);
	}
	count -=9;
	close(fd);
	// Отваряме отново файла като след като стигаме първия от последните десет реда започваме
	// да ги пишем в стандартния изход. В случай че редовете са по малко от 10 изкарваме всички редове
	fd = open(argv[k], O_RDONLY);
	result = 1;
	int result1 = 1;
	int count1 = 0;
	if(count <= 0){
		while(result > 0)
		{
			result = read(fd, &buff, 1);
			write(1, &buff, result);
		}
	}else{
		while(result > 0)
		{
			if(buff=='\n'){
				count1++;
				if(count1==count){
					while(result1 > 0){
						result1 = read(fd, &buff, 1);
						if(result1 <= 0){
							break;
						}
						write(1, &buff, result);
					}
					
				}
			}
			result = read(fd, &buff, 1);		
		}
		
	}
}
