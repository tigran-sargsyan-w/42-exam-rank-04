#include <stdbool.h>    // bool, true, false
#include <stdio.h>      // printf
#include <signal.h>     // sigaction, sig_atomic_t, SIGALRM, SA_RESTART, kill
#include <sys/wait.h>   // waitpid, WIFEXITED, WEXITSTATUS, WIFSIGNALED, WTERMSIG
#include <unistd.h>     // fork, alarm
#include <string.h>     // strsignal
#include <stdlib.h>     // exit, abort

// Global flags used by the signal handler and the parent
static volatile sig_atomic_t g_timed_out = 0;  // set to 1 when timeout occurs
static volatile sig_atomic_t g_child_pid = -1; // pid of the child to kill on timeout

// Signal handler for SIGALRM: mark timeout and kill the child process
static void alarm_handler(int sig)
{
    (void)sig;                 // unused parameter
    g_timed_out = 1;           // remember that timeout has been reached
    if (g_child_pid > 0)       // if we have a valid child pid
        kill((pid_t)g_child_pid, SIGKILL); // forcefully kill the child
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    struct sigaction sa;
    pid_t pid;
    int status;

    // basic validation
    if (!f)
        return -1;

    g_timed_out = 0;
    g_child_pid = -1;

    pid = fork();
    if (pid < 0) // fork failed
        return -1;

    if (pid == 0) // child process
    {
        f();
        exit(0);
    }

    // parent process
    g_child_pid = pid;

    // set up SIGALRM handler
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = alarm_handler;
    if (sigaction(SIGALRM, &sa, NULL) == -1)
    {
        // if sigaction fails, clean up the child and report sandbox error
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
        g_child_pid = -1;
        return -1;
    }

    // start the timeout timer
    alarm(timeout);

    if (waitpid(pid, &status, 0) == -1)// wait for the child
    {
        alarm(0); // cancel timer just in case
        g_child_pid = -1;
        return -1;
    }

    // child is done, we no longer need the timer or the pid
    alarm(0); // cancel timer just in case
    g_child_pid = -1;

    if (g_timed_out) // if timeout occurred
    {
        if (verbose)
            printf("Bad function: timed out after %u seconds\n", timeout);
        return 0;
    }

    if (WIFEXITED(status)) // no timeout: inspect the way the child terminated
    {
        int code = WEXITSTATUS(status);
        if (code == 0)
        {
            // exited normally with status 0 → nice function
            if (verbose)
                printf("Nice function!\n");
            return 1;
        }
        else
        {
            // exited normally but with non-zero status → bad function
            if (verbose)
                printf("Bad function: exited with code %d\n", code);
            return 0;
        }
    }

    if (WIFSIGNALED(status)) // child was terminated by a signal (segfault, abort, etc.)
    {
        int sig = WTERMSIG(status);
        if (verbose)
            printf("Bad function: %s\n", strsignal(sig));
        return 0;
    }
    return -1;
}

#pragma region Test code

void ok_f(void)
{
	printf("noice. ");
}

void inf_f(void)
{
	while (1)
		;
}

void bad_exit(void)
{
	exit(41);
}

void suicide_f(void)
{
    kill(getpid(), SIGKILL);
    sleep(10);
}

void fast_print_f(void)
{
    printf("fast_print_f executed\n");
}

void abort_f(void)
{
    abort();
}

void stop_f(void)
{
    raise(SIGSTOP);
    sleep(10);
}

void segfault_f(void)
{
    int *p = NULL;
    *p = 42;
}

void cancel_alarm(void)
{
	struct sigaction ca;
	ca.sa_handler = SIG_IGN;
	sigaction(SIGALRM, &ca, NULL);

	sleep(5);
	printf("f waited 5 seconds, should be terminated before by alarm set in parent process if timeout < 5\n");
}

void leak_f(void)
{
	int *p = NULL;
	*p = 4;
}

void test_func(void (*f)(void), unsigned int timeout, const char* name)
{
    printf("==== Test : %s (timeout %us) ====\n", name, timeout);
    int res = sandbox(f, timeout, true);
    printf("Result = %d\n\n", res);
}

int main(void)
{
    test_func(ok_f, 2, "ok_f (nice function, immediate return)");

    test_func(inf_f, 2, "inf_f (infinite loop, should timeout)");

    test_func(bad_exit, 2, "bad_exit (exit with code 41)");

    test_func(cancel_alarm, 2, "cancel_alarm (ignore SIGALRM, sleep 5s)");

    test_func(segfault_f, 2, "segfault_f (segmentation fault)");

    test_func(abort_f, 2, "abort_f (abort signal SIGABRT)");

    test_func(stop_f, 2, "stop_f (stops self with SIGSTOP, never continues)");

    test_func(suicide_f, 2, "suicide_f (kills self with SIGKILL)");

    test_func(fast_print_f, 2, "fast_print_f (quick print + return 0)");

    return 0;
}
#pragma endregion