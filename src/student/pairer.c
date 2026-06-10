#include "student_api.h"


int student_pair_syscall(struct syscall_pairer *pairer,
                         const struct syscall_event *ev,
                         struct syscall_event *out)
{

    if (ev->entering == 1)  {
        pairer->entry = *ev;
        pairer->has_entry = 1;
        return 0;
                            }

    if (ev->entering == 0)  {
        if (pairer->has_entry == 0)  {
            return -1;
                                     }

        *out = pairer->entry;
        out->ret = ev->ret;
        out->entering = 0;       
        pairer->has_entry = 0;
        return 1;
                            }

    return 0;
}