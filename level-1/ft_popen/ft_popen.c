#include <unistd.h>     // pipe, fork, dup2, execvp, close
#include <stdlib.h>     // exit
#include <sys/types.h>  // pid_t

int ft_popen(const char *file, char *const argv[], char type)
{
    int     fd[2];
    pid_t   pid;

    // basic validation of parameters
    if (!file || !argv || !argv[0] || (type != 'r' && type != 'w'))
        return (-1);
    if (pipe(fd) == -1)
        return (-1);

    pid = fork();
    if (pid < 0) // fork failed
    {
        close(fd[0]);
        close(fd[1]);
        return (-1);
    }
    if (pid == 0) // child process
    {
        if (type == 'r')
        {
            // connect child's stdout to pipe write end 
            if (dup2(fd[1], STDOUT_FILENO) == -1)
            {
                close(fd[0]);
                close(fd[1]);
                exit(1);
            }
        }
        else // type == 'w'
        {
            // connect child's stdin to pipe read end 
            if (dup2(fd[0], STDIN_FILENO) == -1)
            {
                close(fd[0]);
                close(fd[1]);
                exit(1);
            }
        }
        // we don't need pipe fds anymore in the child 
        close(fd[0]);
        close(fd[1]);

        execvp(file, argv);
        exit(1); // if execvp failed
    }

    // parent process 
    if (type == 'r')
    {
        // parent will read from fd[0], so close write end 
        close(fd[1]);
        return (fd[0]);
    }
    else // type == 'w'
    {
        // parent will write to fd[1], so close read end 
        close(fd[0]);
        return (fd[1]);
    }
}

#pragma region Test code

// Variant from subject without get_next_line and ft_putstr
int main(void)
{
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
