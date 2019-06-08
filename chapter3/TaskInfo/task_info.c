#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>

#define BUFFER_SIZE 128
#define PROC_NAME "pid"

static long pid_num;

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);
ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
    .write = proc_write
};

ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos)
{
    char *k_mem;

    k_mem = kmalloc(count, GFP_KERNEL);

    if(copy_from_user(k_mem, usr_buf, count)){
        printk(KERN_INFO "Error copying from user\n");
        return -1;
    }

    k_mem[count-1] = '\0';
    kstrtol(k_mem, 10, &pid_num);

    kfree(k_mem);

    return count;
}

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv = 0;
    struct task_struct *tsk = NULL;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    
    if(completed){
        completed = 0;
        return 0;
    }

    tsk = pid_task(find_vpid(pid_num), PIDTYPE_PID);
    if(tsk){
        volatile long state = tsk -> state;
        pid_t pid = tsk -> pid;
        char *command = tsk -> comm;

        rv = sprintf(buffer, "command = [%s] pid = [%u] state = [%u]\n", command, pid, state);
    }else{
        rv = sprintf(buffer, "pid %u not found\n", pid_num);
    }

    completed = 1;

    copy_to_user(usr_buf, buffer, rv);
    
    return rv;
}


int task_info_init(void)
{
    printk(KERN_INFO "/proc/%s created", PROC_NAME);
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);

    return 0;
}

void task_info_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed", PROC_NAME);
}

module_init(task_info_init);
module_exit(task_info_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PID Task Information Module");
MODULE_AUTHOR("ZYY");
