#include <unistd.h>     // pipe, fork, dup2, execvp, close
#include <stdlib.h>     // exit
#include <sys/types.h>  // pid_t

int ft_popen(const char *file, char *const av[], char type)
{
    int fd[2];
    pid_t pid;
    
    if (!file || !av || (type != 'r' && type != 'w') || pipe(fd) < 0)
        return -1;
    if ((pid = fork()) < 0)
    {
        close(fd[0]);
        close(fd[1]);
        return -1;
    }
    if (pid == 0)
    {
        // close(fd[type == 'r' ? 0 : 1]); // to avoid double close
        dup2(fd[type == 'r' ? 1 : 0], type == 'r' ? 1 : 0);
        close(fd[0]);
        close(fd[1]);
        execvp(file, av);
        exit(1);
    }
    close(fd[type == 'r' ? 1 : 0]);
    return fd[type == 'r' ? 0 : 1];
}

#pragma region Test code

// Variant from subject without get_next_line and ft_putstr
int main(void)
{
    write(1, "Test ft_popen_short\n", 20);
    int  fd;
    char c;

    fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');
    if (fd < 0)
        return (1);
    while (read(fd, &c, 1) > 0)
        write(1, &c, 1);
    close(fd);
    return (0);
}
#pragma endregion
