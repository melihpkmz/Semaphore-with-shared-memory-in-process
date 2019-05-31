
#include <stdio.h>	 /* standard I/O routines.               */
#include <sys/types.h>   /* various type definitions.            */
#include <sys/ipc.h>     /* general SysV IPC structures          */
#include <sys/shm.h>	 /* semaphore functions and structs.     */
#include <sys/sem.h>	 /* shared memory functions and structs. */
#include <unistd.h>	 /* fork(), etc.                         */
#include <wait.h>	 /* wait(), etc.                         */
#include <stdlib.h>	 /* rand(), etc.                         */
#include <string.h>

void
sem_lock(int sem_set_id)
{
    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    semop(sem_set_id, &sem_op, 1);
}
void
sem_unlock(int sem_set_id)
{
    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = 1;   
    sem_op.sem_flg = 0;
    semop(sem_set_id, &sem_op, 1);
}


void makeColor(FILE* fp,char* colorNow,int i,int sem_set_id,char boxColor,char boxs[],int box_num,int* counterColor)
{
    sem_lock(sem_set_id);
    fprintf(fp, "%d %c\n",getpid(),boxColor);
    /********COLOR CHANGES*********/
    int j = 0;
    int index = 0;
    for(j=0;j<box_num;j++)
    {
    	if(*colorNow == boxs[j]) index = j;
    }
    int flag = 1;
    if(i==index)
    {
        for(int k=0;k<box_num;k++)
        {
           if(*colorNow == boxs[k] && k+1 != box_num)
           {
		for(int a=k-1;a>=0;a--)
	   	{
			if(boxs[k+1] == boxs[a] || boxs[k+1] == *colorNow){ flag = 0; break;}
			else flag = 1;
                }
                if(boxs[k+1] != *colorNow && flag) 
                {
		     sleep(1);
                     *colorNow = boxs[k+1];
		     (*counterColor)++;
                     break;
                }
           }
        }
    }
    /*********END*************/
    sem_unlock(sem_set_id);	
}

void makePriority(FILE* fp,char* colorNow,int i,int sem_set_id,int local_pid[],int box_num,char boxs[],int* counterColor)
{
    while(*colorNow != boxs[i]) /*Wait until changing colors*/
    {
    }
    makeColor(fp,colorNow,i,sem_set_id,boxs[i],boxs,box_num,counterColor);
}



int main(int argc, char* argv[])
{ 
   FILE * fp;
   fp = fopen ("output.txt", "w+");


    int box_num;

    char* boxs;
    int shmid = 0;
    int shmid2 = 0;
    int shmid3 = 0;
    int SEM_ID = 100;
    int SEM_ID2 = 101;
    int SEM_ID3 = 102;

    int* local_pid;
    char* colorNow;
    int* counterColor;
    /*********************Create Semaphore******************************/
    union semun {            
                int val;
                struct semid_ds *buf;
                ushort * array;
    } sem_val; 
    int sem_set_id = semget(SEM_ID, 1, IPC_CREAT|0666);
    if (sem_set_id == -1) {
	perror("main: semget");
	exit(1);
    }

    sem_val.val = 2; /*Two box may enter*/
    int rc = semctl(sem_set_id, 0, SETVAL, sem_val);
    if (rc == -1) {
	perror("main: semctl");
	exit(1);
    }
    /******************************END**********************************/	

    /***************************READ FILE*******************************/
    FILE* ptr = fopen("input.txt","r"); 
    if (ptr==NULL) 
    { 
        printf("no such file."); 
        return 0; 
    }  
    fscanf(ptr,"%d\n",&box_num); 
    /* allocate a shared memory segment*/
    shmid = shmget(SEM_ID, box_num*sizeof(char), IPC_CREAT|0666);
    shmid2 =shmget(SEM_ID2, sizeof(char), IPC_CREAT|0666);
    shmid3 = shmget(SEM_ID3,sizeof(int), IPC_CREAT|0666);

    boxs = (char *) shmat(shmid, 0 ,0);
    colorNow = (char *) shmat(shmid2, 0 ,0);
    counterColor = (int *) shmat(shmid3, 0 ,0);
    (*counterColor) = 0;

    int i =0;
    char c;
    for(i=0;i<box_num;i++)
    {
    fscanf(ptr,"%c\n",&boxs[i]); 
    }
    fclose(ptr);
 
    /******************************END**********************************/
	
    local_pid = (int*) malloc(box_num * sizeof(int));
    *colorNow = boxs[0];

    /*****************************Create Children**********************/
    for(int i=0;i<box_num;i++) 
    { 
    	if(fork() == 0) 
    	{
                local_pid[i] = getpid();
		/*printf("%d. [son] pid %d from [parent] pid %d\n",i,local_pid[i],getppid());*/
                makePriority(fp,colorNow,i,sem_set_id,local_pid,box_num,boxs,counterColor); 	
       		exit(0); 
    	} 
    }

    for(int i=0;i<box_num;i++) 
    wait(NULL); 
    /******************************END**********************************/	
    fprintf(fp, "%d\n",(*counterColor));
	
    /***********Deallocate Shared Memory and Close Output File*********/ 
    shmctl(shmid, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);
    shmctl(shmid3, IPC_RMID, NULL);
    fclose(fp);
    /******************************END**********************************/	
  

    return 0;
}

