#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/atomic.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/random.h>
#include "constants.h"
#include "proc.h"

static struct proc_dir_entry *fprint_dir_entry;
static struct proc_dir_entry *signature_entry;
static struct proc_dir_entry *signal_entry;

static atomic_t reader_available = ATOMIC_INIT(1);
static DECLARE_WAIT_QUEUE_HEAD(reader_queue);
static DECLARE_WAIT_QUEUE_HEAD(poll_wait_queue);

static const void* signature_data = NULL;
static size_t signature_data_size = 0;
static bool signature_data_available = false;

static int seqfile_iterator = 0;

void* start(struct seq_file *m, loff_t *pos);
void stop(struct seq_file *m, void *v);
void* next(struct seq_file *m, void *v, loff_t *pos);
int show(struct seq_file *m, void *v);

int open(struct inode* inode, struct file* file);
static unsigned int poll(struct file *file, poll_table *wait);

int sgn_open(struct inode* inode, struct file* file);
ssize_t sgn_write(struct file* file, const char __user* buf, size_t count, loff_t* pos);

static const struct seq_operations fprint_seq_ops = {
    .start = start,
    .next = next,
    .stop = stop,
    .show = show
};

static const struct proc_ops fprint_proc_ops = {
    .proc_open    = open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = seq_release,
    .proc_poll    = poll,
};

static const struct proc_ops fprint_signal_ops = {
    .proc_open  = sgn_open,
    .proc_write = sgn_write
};

int register_fprint_proc_file(void)
{
    fprint_dir_entry = proc_mkdir("fprint", NULL);
    if (fprint_dir_entry == NULL) {
        printk(KERN_ERR LOG_PREFIX "failed to create proc entry 'fprint'\n");
        return -1;
    }

    signature_entry = proc_create("signature", 0, fprint_dir_entry, &fprint_proc_ops);
    if (signature_entry == NULL) {
        printk(KERN_ERR LOG_PREFIX "failed to create proc entry 'signature'\n");
        proc_remove(fprint_dir_entry);
        fprint_dir_entry = NULL;
        return -1;
    }

    signal_entry = proc_create("signal", 0, fprint_dir_entry, &fprint_signal_ops);
    if (signal_entry == NULL) {
        printk(KERN_ERR LOG_PREFIX "failed to create proc entry 'signal'\n");
        proc_remove(signature_entry);
        signature_entry = NULL;
        proc_remove(fprint_dir_entry);
        fprint_dir_entry = NULL;
        return -1;
    }

    printk(KERN_INFO LOG_PREFIX "registered proc files\n");
    return 0;
}

void unregister_fprint_proc_file(void)
{
    proc_remove(signal_entry);
    signal_entry = NULL;
    proc_remove(signature_entry);
    signature_entry = NULL;
    proc_remove(fprint_dir_entry);
    fprint_dir_entry = NULL;

    printk(KERN_INFO LOG_PREFIX "unregistered proc files\n");
}

void post_signature_data(const void* data, size_t size)
{
    signature_data = data;
    signature_data_size = size;
    signature_data_available = true;
    wake_up_interruptible(&poll_wait_queue);
    wake_up_interruptible(&reader_queue);
}

void* start(struct seq_file *m, loff_t *pos)
{
    printk(KERN_INFO LOG_PREFIX "start(m, pos=%lld)\n", *pos);

    seqfile_iterator = 0;
    return &seqfile_iterator;
}

void stop(struct seq_file *m, void *v)
{
    printk(KERN_INFO LOG_PREFIX "stop(m)\n");

    atomic_set(&reader_available, 1);
}

void* next(struct seq_file *m, void *v, loff_t *pos)
{
    printk(KERN_INFO LOG_PREFIX "next(m, pos=%lld)\n", *pos);

    wait_event_interruptible(reader_queue, signature_data_available);
    signature_data_available = false;

    *pos += signature_data_size;
    return NULL;
}

int show(struct seq_file *m, void *v)
{
    printk(KERN_INFO LOG_PREFIX "show(m)\n");

    if (signature_data == NULL) {
        printk(KERN_WARNING LOG_PREFIX "signature data is NULL\n");
        return 0;
    }

    seq_write(m, signature_data, signature_data_size);
    printk(KERN_INFO LOG_PREFIX "signature data size=%zu\n", signature_data_size);
    return 0;
}

int open(struct inode* inode, struct file* file)
{
    signature_data_available = false;
    return seq_open(file, &fprint_seq_ops);
}

static unsigned int poll(struct file *file, poll_table *wait)
{
    poll_wait(file, &poll_wait_queue, wait);
    if (signature_data_available)
        return POLLIN | POLLRDNORM;
    return 0;
}

int sgn_open(struct inode* inode, struct file* file)
{
    return 0;
}

static char buffer[504*480];

ssize_t sgn_write(struct file* file, const char __user* buf, size_t count, loff_t* pos)
{
    get_random_bytes(buffer, sizeof(buffer));

    printk(KERN_INFO LOG_PREFIX "sgn_write(file=%p, buf=%p, count=%zu, pos=%lld): '%s'\n", file, buf, count, *pos, buffer);
    post_signature_data(buffer, sizeof(buffer));
    return count;
}
