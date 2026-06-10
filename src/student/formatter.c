#include "student_api.h"
#include "syscall_names.h"
#include <stdio.h>
#include "trace_helpers.h"

void student_debug_raw_event(const struct syscall_event *ev,char *buf,size_t bufsz)
{
  

    snprintf(buf, bufsz, "pid=%d %s %s",
             ev->pid,
             syscall_name(ev->syscall_no),
             ev->entering ? "entrada" : "saida");
}

void student_format_event(const struct syscall_event *ev,char *buf,size_t bufsz)
{
   

if (ev->syscall_no == 1)    { 
    snprintf(buf, bufsz, "write(%ld, %#lx, %ld) = %ld",
             (long)ev->args[0],
             ev->args[1],
             (long)ev->args[2],
             ev->ret);
             return;
                            }

if (ev->syscall_no == 0)    {  
    snprintf(buf, bufsz, "read(%ld, %#lx, %ld) = %ld",
             (long)ev->args[0],
             ev->args[1],
             (long)ev->args[2],
             ev->ret);
             return;
                            }
if (ev->syscall_no == 59) {
    char path[256];
    if (read_child_string(ev->pid, ev->args[0], path, sizeof(path)) < 0)
        snprintf(buf, bufsz, "execve(\"<ilegivel>\", ...) = %ld", ev->ret);
    else
        snprintf(buf, bufsz, "execve(\"%s\", ...) = %ld", path, ev->ret);
    return;
}
if (ev->syscall_no == 257) {
    char path[256];
    if (read_child_string(ev->pid, ev->args[1], path, sizeof(path)) < 0)
        snprintf(buf, bufsz, "openat(%ld, \"<ilegivel>\", %ld, %ld) = %ld",
                 (long)ev->args[0], (long)ev->args[2], (long)ev->args[3], ev->ret);
    else
        snprintf(buf, bufsz, "openat(%ld, \"%s\", %ld, %ld) = %ld",
                 (long)ev->args[0], path, (long)ev->args[2], (long)ev->args[3], ev->ret);
    return;
}
    snprintf(buf, bufsz, "%s(%#lx, %#lx, %#lx, %#lx, %#lx, %#lx) = %ld",
        syscall_name(ev->syscall_no),
             ev->args[0],
             ev->args[1],
             ev->args[2],
             ev->args[3],
             ev->args[4],
             ev->args[5],
             ev->ret);
}
