#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    for (int i = 0; i < 5; i++) // loop will run n times (n=5)
    {
        if (fork() == 0)
        {
            printf("[son] pid %d from [parent] pid %d\n", getpid(), getppid());
            exit(0);
        }
    }
    for (int i = 0; i < 5; i++) // loop will run n times (n=5)
        wait(NULL);
}
