
#include <stdio.h>
#include <stdlib.h>
//#include <sys/ipc.h>
#include <ctype.h>
#include <sys/types.h>
#include <string.h>
#include <stdio_ext.h>
#include "semaphoreOperations.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define CLIENT 1


int validate(char cad1[]);

typedef struct Asientos
{
	int NumAsiento[TAM][TAM];
	int AsientosVendidos[TAM*TAM];
	int contador_lugares;
	
}Asientos;

int search(Asientos *NumAsiento, int elemento)
{
	int i = 0, coinciencia = 0;
	
	while((!coinciencia) && (i < TAM*TAM)) {
		if(NumAsiento->AsientosVendidos[i] == elemento)
			return 1;
		i++;
	}

	return 0;
}

int MostrarAsientos(struct Asientos *NumAsiento)
{
	int i, j, aux;
	
	for(i = 0; i < TAM; i++) {
		for(j = 0; j < TAM; j++) {
			aux = NumAsiento->NumAsiento[i][j];
			if((search(NumAsiento,aux)) == 0)
				((aux % GROUP) != 0) ? printf(ANSI_COLOR_GREEN " %2d  " ANSI_COLOR_RESET, aux) : printf(ANSI_COLOR_GREEN "%2d	   " ANSI_COLOR_RESET, aux);
			else
				((aux % GROUP) != 0) ? printf(ANSI_COLOR_RED " %2d  " ANSI_COLOR_RESET, aux) : printf(ANSI_COLOR_RED "%2d	   " ANSI_COLOR_RESET, aux);
		}
		
		puts("\n");
	}
	
	puts(ANSI_COLOR_GREEN "\n\tVERDE" ANSI_COLOR_RESET ": Asientos Disponibles.");
	puts(ANSI_COLOR_RED "\tROJO" ANSI_COLOR_RESET ": Asientos Ocupados");
}

int main() 
{
	int IdMemory, IdSemaphore, option, flag1, i, j, z;
	char cad1[10];
	int TotalAsientos, NumeroAsiento, num; 
	key_t key1, key2;
	Asientos *Memory = NULL;
	
	key1 = ftok("Servidor.c", 'T');
	key2 = ftok("Cliente.c", 'W');
	
	if((IdSemaphore = create_semaphore(key1, 2, CLIENT)) == -1) {
		perror("Error al ejecutar semget");
		exit(-1);
	}
	
	if((IdMemory = shmget(key2, sizeof(Asientos), IPC_CREAT | 0600)) == -1) {
		perror("Error en la ejecucion de memoria compartida");
		exit(-1);
	}
	
	if((Memory = (Asientos*)shmat(IdMemory, NULL, 0)) == (Asientos *)(-1)) {
		perror("Fallo de referencia a memoria");
		exit(-1);
	}
	
	down(IdSemaphore, 1);
	
	do {
		option = 0;
		printf("\n\t**------Bienvenido a AEROLINEAS DaVInci------**\n\n");
		
		if(Memory->contador_lugares == (TAM*TAM)) {
			puts("Lo sentimos, se han agotado los todos los lugares");
			break;
		}
		
		MostrarAsientos(Memory);
		do {
			printf("\nPor favor digite el numero de asientos que desea comprar: ");
			scanf("%s",cad1);
			flag1 = validate(cad1);
			
			if(flag1) {
				NumeroAsiento = atoi(cad1);
				if((NumeroAsiento < 1) || (NumeroAsiento > ((TAM*TAM) - Memory->contador_lugares))) {
					puts("Verifique el rango seleccionado");
					flag1 = 0;
				}		
			}	
		}while(!flag1);
		
		printf("Por favor elija el lugar segun se muestra en la pantalla\n");
		TotalAsientos = NumeroAsiento;
		j = 1;
		
		do{
			printf("Asiento %d:  Numero: ", j);
			scanf("%s",cad1);
			if(validate(cad1)) {
				num = atoi(cad1);
				flag1 = 1;
				if((num < 1) || (num > (TAM*TAM))) {
					puts("Verifique que su lugar se encuentra en la pantalla");
					flag1 = 0;
				}
				else if(search(Memory,num)) {
					printf("El lugar "ANSI_COLOR_RED "%d" ANSI_COLOR_RESET " ya esta ocupado, por favor elija otro lugar", num);
					flag1 = 0;
				}
				else {
					Memory->AsientosVendidos[Memory->contador_lugares++] = num;
					--NumeroAsiento;
					j++;
				}
			}
				
			puts("Pulse ENTER para continuar");
			__fpurge(stdin);
			getchar();
			system("clear");
			MostrarAsientos(Memory);
			
		}while(!flag1 || (NumeroAsiento != 0));
		
		printf("Asientos comprados: ");
		for(i = (Memory->contador_lugares - TotalAsientos);i < Memory->contador_lugares;i++)
			printf(ANSI_COLOR_GREEN "%d " ANSI_COLOR_RESET,Memory->AsientosVendidos[i]);
		
		printf("\nDesea confirmar su compra? [0]SI / [1]NO: ");
		scanf("%d",&option);
		if(option) {
			for(i = (Memory->contador_lugares - TotalAsientos);i < Memory->contador_lugares;i++)
				Memory->AsientosVendidos[i] = 0;
			Memory->contador_lugares -= TotalAsientos;
			printf("Por favor repita su proceso de compra de nuevo.");
			printf("\nPulse ENTER para continuar");
			__fpurge(stdin);
			getchar();
		}
		
	} while(option);
	
	up(IdSemaphore, 0);
	
	//shmdt(Memory);
	
}

int validate(char cad1[])
{
	int i;
	
	for(i = 0;i < strlen(cad1);i++)
		if(!(isdigit(cad1[i]))) {
			puts("Dato Invalido, Por favor verifque");
			return 0;
		}
	
	return 1;
}
