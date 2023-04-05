#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "wish_utils.h"

#define MAX_SIZE 100

char *mypath[] = {"bli", "bla", "/bin/", ""};

int main(int argc, char *argv[])
{

	char str[MAX_SIZE];
	char *command_string;
	char *s;
	int fd;

	do
	{
		printf("whish> ");

		// Lee lo que el usuario escribe en la consola
		/*
		str es donde se almacenan lo que el usuario escribe
		MAX_SIZE es el numero maximo de caracteres que se leerán
		stdin le dice que leerá la entrada del usuario desde el teclado
		*/
		fgets(str, MAX_SIZE, stdin);
		s = str;
		while (*s != '\n')
		{
			++s;
		}
		//Se cambia el valor de \n por el de \0 
		/*porque esto le dice a las funciones que usen esa cadena
		en donde termina*/
		*s = '\0';
		s = str;
		
		//Contiene el primer comando
		command_string = strtok_r(s, " ", &s);

		//strcmp compara dos cadenas de texto
		/* si son iguales retorna 0,
		si la primera es menor, retorna un valor negativo, creo que -1
		si la primera es mayor, retorna un valor positivo, creo que +1
		*/
		if (strcmp(command_string, "exit") == 0)
		{
			execute_exit(0);
		}
		else if (strcmp(command_string, "cd") == 0)
		{
			execute_cd(s);
		}
		else if (strcmp(command_string, "path") == 0)
		{
			execute_path();
		}
		else
		{
			fd = -1;
			char **mp = mypath;
			char specificpath[MAX_SIZE];

			//Si *mp no está vacio y fd es diferente de 0, entra al while
			while ((strcmp(*mp, "") != 0) && fd != 0)
			{
				//Copia el contenido de *mp +1 en specificpath
				//mp apunta a al vector que contiene las ubicaciones
				strcpy(specificpath, *mp++);

				//char *strncat(char *dest, const char *src, size_t n);
				/*
				concatena n caracteres de src en dest
				concatena toda la cadena command_string en specificpath
				*/
				strncat(specificpath, command_string, strlen(command_string));

				//access verifica si specificpath tiene permisos de ejecucion o si es ejecutable
				//devuelve 0 si es permitido y -1 y es denegado
				fd = access(specificpath, X_OK);
			}
			if (fd == 0)
			{
				//Crea un proceso hijo y el identificar de cada proceso lo guarda en subprocess
				int subprocess = fork();

				//El proceso hijo vale 0 y en el padre es el identificador del proceso hijo
				//Es -1 si es error
				if (subprocess < 0)
				{
					printf("Error launching the subprocess");
				}
				else if (subprocess == 0)
				{
					//Este es el hijo
					char *myargs[3];

					//Crea una copia y devuelve la ubicacion de la copia
					myargs[0] = strdup(specificpath);
					myargs[1] = strdup(".");
					myargs[2] = NULL;

					//Ejecuta un programa
					/*
					execvp (const char *file, char *const argv[]);
					file es un puntero al nombre del archivo que se quiere ejcutar
					argv[] es un puntero a una matriz de argumentos de linea de comandos
						que se pasan al programa como argumentos
	
					*/
					execvp(myargs[0], myargs);
				}
				else
				{
					//El proceso padre espera a que el hijo termine
					wait(NULL);
				}
			}
			else
			{
				printf("Command not found: %s\n", str);
			}
		}
	} while (1);
}
