#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <string.h>
#include "semaphoreOperations.h"

#define SERVER 0


typedef struct Asientos
{
	int NumAsiento[TAM][TAM];
	int AsientosVendidos[TAM*TAM];
	int contador_lugares;
	
}Asientos;

int main()
{
	int IdMemory, IdSemaphore, i, j, counter = 1, cont = 0;
	key_t key1, key2;
	struct Asientos *Memory = NULL;
	
	key1 = ftok("Servidor.c", 'T');
	key2 = ftok("Cliente.c", 'W');
	
	if((IdSemaphore = create_semaphore(key1, 2, SERVER)) == -1) {
		perror("Error al ejecutar semget");
		exit(-1);
	}
	
	if((IdMemory = shmget(key2, sizeof(Asientos), IPC_CREAT | PERMISSIONS)) == -1) {
		perror("Error en la ejecucion de memoria compartida");
		exit(-1);
	}
	
	if((Memory = (Asientos *)shmat(IdMemory, NULL, 0)) == (Asientos *)(-1)) {
		perror("Fallo de referencia a memoria");
		exit(-1);
	}
	
	down(IdSemaphore, 0);
	
	for(i = 0; i < TAM*TAM; i++)
		Memory->AsientosVendidos[i] = 0;
	
	for(i = 0; i < TAM; i++)
		for(j = 0; j < TAM; j++)
			Memory->NumAsiento[i][j] = counter++;
		
	Memory->contador_lugares = 0;
	
	while(Memory->contador_lugares <= (TAM*TAM)) 
	{
		up(IdSemaphore, 1);
		sleep(3);
		down(IdSemaphore, 0);
	}
	
	shmdt ((char *)Memory);
	shmctl (IdMemory, IPC_RMID, (struct shmid_ds *)NULL);
	
}
