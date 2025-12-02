#include <unistd.h>     // fork, execvp, pipe, dup2, close
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // wait, WIFEXITED, WEXITSTATUS
#include <stdlib.h>     // exit

int picoshell(char **cmds[])
{
    int     prev_fd = -1;   // read end of the previous pipe
    int     pipefd[2];
    int     i = 0;
    int     ret = 0;        // 0 = success, 1 = error
    int     status;

    // validation of parameters
    if (cmds == NULL || cmds[0] == NULL)
        return (0);

    while (cmds[i] != NULL)
    {
        // if there is a next command, we need a pipe
        if (cmds[i + 1] != NULL)
        {
            if (pipe(pipefd) == -1)
            {
                ret = 1;
                break;
            }
        }
        else // last command: no pipe is created
        {
            pipefd[0] = -1;
            pipefd[1] = -1;
        }

        pid_t pid = fork();
        if (pid < 0) // fork failed
        {
            if (pipefd[0] != -1)
                close(pipefd[0]);
            if (pipefd[1] != -1)
                close(pipefd[1]);
            ret = 1;
            break;
        }
        if (pid == 0) // child process
        {
            // connect stdin to previous pipe if it exists
            if (prev_fd != -1)
            {
                if (dup2(prev_fd, STDIN_FILENO) == -1)
                    exit(1);
            }

            // connect stdout to current pipe if there is a next command
            if (cmds[i + 1] != NULL)
            {
                if (dup2(pipefd[1], STDOUT_FILENO) == -1)
                    exit(1);
            }

            // close unused file descriptors in the child
            if (prev_fd != -1)
                close(prev_fd);
            if (cmds[i + 1] != NULL)
            {
                close(pipefd[0]);
                close(pipefd[1]);
            }

            execvp(cmds[i][0], cmds[i]);
            exit(1); // if execvp failed
        }
        // parent process

        // previous read end is no longer needed by the parent
        if (prev_fd != -1)
            close(prev_fd);

        if (cmds[i + 1] != NULL)
        {
            // parent keeps read end for the next command
            close(pipefd[1]);      // write end is only needed in the child
            prev_fd = pipefd[0];   // read end becomes prev_fd
        }
        else
        {
            prev_fd = -1;
        }
        i++;
    }

    // close last prev_fd if still open
    if (prev_fd != -1)
        close(prev_fd);

    while (wait(&status) > 0) // wait for all children
    {
        if (WIFEXITED(status) == 0 || WEXITSTATUS(status) != 0)
            ret = 1;
    }
    return ret;
}

#pragma region Test code

#include <stdio.h>
int main(void)
{
    char *cmd1[] = {"/bin/ls", "level-1", NULL};
    // char *cmd1[] = {"/bin/ls", NULL};
    char *cmd2[] = {"/usr/bin/grep", "picoshell", NULL};
    char **cmds[] = {cmd1, cmd2, NULL};

    int result = picoshell(cmds);
    printf("picoshell returned %d\n", result);

    return 0;
}
#pragma endregion
