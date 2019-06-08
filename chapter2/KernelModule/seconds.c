#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <asm/param.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

long unsigned init_jiffies = 0;

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
};

int proc_init(void)
{
    init_jiffies = jiffies;
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);

    return 0;
}


void proc_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
}

ssize_t  proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    
    if(completed){
        completed = 0;
        return 0;
    }

    completed = 1;
   
    rv = sprintf(buffer, "seconds: %lu\n", (jiffies - init_jiffies) / HZ);

    copy_to_user(usr_buf, buffer, rv);
    
    return rv;
}
module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SECONDS MODULE");
MODULE_AUTHOR("ZYY");

