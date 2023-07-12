#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>

#define TAM 10
#define GROUP (TAM/2)
#define PERMISSIONS 0666
#define SERVER 0

struct sembuf operation;

void down(int semid, int semaphore_value);
void up(int semid, int semaphore_value);
int create_semaphore(key_t key, int num_semaphores, int type);


void down(int semid, int semaphore_value)
{
	operation.sem_flg = 0;
	operation.sem_op = -1;
	operation.sem_num = semaphore_value;
	semop(semid, &operation, 1);
}

void up(int semid, int semaphore_value)
{
	operation.sem_flg = 0;
	operation.sem_op = 1;
	operation.sem_num = semaphore_value;
	semop(semid, &operation, 1);
}

int create_semaphore(key_t key, int num_semaphores, int type)
{
	int semid, i, j;
   
	if((semid = semget(key,num_semaphores,IPC_CREAT | PERMISSIONS)) == -1)
		return -1;
	
	if(SERVER == type)
		for(i = 0, j = 1; i < num_semaphores; i++, j--)
			semctl(semid,i,SETVAL,j);
	
	return semid;
}
