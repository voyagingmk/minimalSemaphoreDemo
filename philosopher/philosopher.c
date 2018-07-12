
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#define N 5
#define LEFT (i + N - 1) % N
#define RIGHT (i + 1) % N
#define THINKING 0
#define HUNGRY 1
#define EATING 2

sem_t *mutex = NULL;
sem_t *s[N];
int *state;

const char *s_mutex = "mutex";
const char *s_i[5] = {"s_0", "s_1", "s_2", "s_3", "s_4"};

void try_eat(int i)
{
  if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
  {
    state[i] = EATING;
    printf("Child %d eat, %d, %d, %d, %d, %d\n", i, state[0], state[1], state[2], state[3], state[4]);
    sem_post(s[i]);
  }
}

void take_forks(int i)
{
  sem_wait(mutex);
  state[i] = HUNGRY;
  try_eat(i);
  sem_post(mutex);
  sem_wait(s[i]);
}

void put_forks(int i)
{
  sem_wait(mutex);
  state[i] = THINKING;
  try_eat(LEFT);
  try_eat(RIGHT);
  sem_post(mutex);
}

void philosopher(int i)
{
  while (1)
  {
    take_forks(i);
    put_forks(i);
  }
}

int main()
{
  sem_unlink(s_mutex);
  for (int i = 0; i < 5; i++)
  {
    sem_unlink(s_i[i]);
  }
  // prevent buffering of the output
  setbuf(stdout, NULL);
  int shm_id;
  if ((shm_id = shmget(IPC_PRIVATE, sizeof(int) * N, IPC_CREAT | 0666)) == -1)
  {
    // handle error
    perror("shmget");
    exit(-1);
  }
  if ((state = shmat(shm_id, NULL, 0)) == NULL)
  {
    // handle error
    perror("shmat");
    exit(-1);
  }
  for (int i = 0; i < N; i++)
  {
    state[i] = THINKING;
  }

  // create two locked semaphores
  if ((mutex = sem_open(s_mutex, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
  {
    // handle error
    perror("sem_open");
    exit(-1);
  }
  for (int i = 0; i < 5; i++)
  {
    if ((s[i] = sem_open(s_i[i], O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
      // handle error
      perror("sem_open");
      exit(-1);
    }
  }

  /* Start children. */
  for (int i = 0; i < N; ++i)
  {
    int pid = 0;
    if ((pid = fork()) < 0)
    {
      perror("fork");
      exit(-1);
    }
    else if (pid == 0)
    {
      printf("philosopher %d begin\n", i);
      philosopher(i);
      exit(0);
    }
    else
    {
      // parent
    }
  }
  printf("Parent wait\n");
  while (waitpid(-1, NULL, 0))
  {
    if (errno == ECHILD)
    {
      break;
    }
  }
  printf("Parent ended\n");
  sem_close(mutex);
  for (int i = 0; i < 5; i++)
  {
    sem_close(s[i]);
  }
  sem_unlink(s_mutex);
  for (int i = 0; i < 5; i++)
  {
    sem_unlink(s_i[i]);
  }
  shmdt(state);
  shmctl(shm_id, IPC_RMID, NULL);
  return EXIT_SUCCESS;
}
