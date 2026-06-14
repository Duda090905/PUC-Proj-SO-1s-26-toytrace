#include "trace_runtime.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#if !defined(__x86_64__)
#error "Este runtime didatico suporta apenas Linux x86_64."
#endif

static void fill_event_from_regs(pid_t pid,
                                 int entering,
                                 const struct user_regs_struct *regs,
                                 struct syscall_event *ev)
{

    memset(ev, 0, sizeof(*ev));
    ev->pid = pid;
    ev->entering = entering;

    ev->syscall_no = regs->orig_rax;
    ev->args[0] = regs->rdi;
    ev->args[1] = regs->rsi;
    ev->args[2] = regs->rdx;
    ev->args[3] = regs->r10;
    ev->args[4] = regs->r8;
    ev->args[5] = regs->r9;
    ev->ret = regs->rax;

}

static pid_t launch_tracee(char *const argv[])
{ 

pid_t pid = fork ();

    if (pid<0)  {
        perror("launch_trace: fork falhou");
        return -1;
                }

    if (pid == 0)   {

        ptrace(PTRACE_TRACEME, 0,NULL,NULL);
        raise (SIGSTOP);
        execvp (argv[0],argv);
        perror ("launch_tracee: execvp falhou");
        return -1;

                    }

    return pid;

}

static int wait_for_initial_stop(pid_t child)
{

    int status;

    if (waitpid (child, &status, 0) < 0 )   {

        perror ("wait_for_initial_stop: waitpid falhou");
        return -1;
                                             }

    return 0;

}

static int configure_trace_options(pid_t child)
{

if (ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD) < 0)
    {
    perror("configure_trace_options: ptrace falhou");
    return -1;
    }

    return 0;
}

static int resume_until_next_syscall(pid_t child, int signal_to_deliver)
{

    if (ptrace(PTRACE_SYSCALL, child, 0, signal_to_deliver) < 0)

    {
        perror("resume_until_next_syscall: ptrace falhou");
        return -1;
    }

    return 0;

}

static int wait_for_syscall_stop(pid_t child, int *status)
{
    if (waitpid(child, status, 0) < 0)
        {
        perror("wait_for_syscall_stop: waitpid falhou");
        return -1;
        }
    if (WIFEXITED(*status) || WIFSIGNALED(*status))
            {
    return 0;
            }

    if (WIFSTOPPED(*status) && (WSTOPSIG(*status) & 0x80))
                {
    return 1;
                }

    return 2;  /*Quando a parada nao e de syscall, ignora*/
}

int trace_program(char *const argv[],
                  trace_observer_fn observer,
                  void *userdata)
{
    pid_t child;
    int status = 0;
    int entering = 1;

    if (argv == NULL || argv[0] == NULL)    {
        fprintf(stderr, "erro: programa alvo ausente\n");
        return -1;  
                                            }

    child = launch_tracee(argv);
    if (child < 0)  {
        return -1;
                    }

    if (wait_for_initial_stop(child) < 0)   {
        return -1;
                                            }

    if (configure_trace_options(child) < 0)  {
        return -1;
                                             }

    if (resume_until_next_syscall(child, 0) < 0)    {
        return -1;
                                                    }

    while (1) {
        struct user_regs_struct regs;
        struct syscall_event ev;
        int stop_kind;

        stop_kind = wait_for_syscall_stop(child, &status);
        if (stop_kind < 0)  {
            return -1;
                            }
if (stop_kind == 0) {
            if (WIFEXITED(status))  {
                return WEXITSTATUS(status);
                                    }
            if (WIFSIGNALED(status))    {
                return 128 + WTERMSIG(status);
                                        }
            return 0;
        }

       if (stop_kind == 2)     {
            int sig = WSTOPSIG(status);
            if (sig == SIGTRAP) sig = 0;
            resume_until_next_syscall(child, sig);
            continue;
                                }

        if (ptrace(PTRACE_GETREGS, child, 0, &regs) < 0)
            {
                 perror("trace_program: ptrace getregs falhou");
                 return -1;
            }

    fill_event_from_regs(child, entering, &regs, &ev);
        if (observer != NULL)           {
            observer(&ev, userdata);
                                        }

        entering = !entering;

        if (resume_until_next_syscall(child, 0) < 0)    {
            return -1;
                                                        }
                }
}
