#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/io.h>		/* needed for mmio handling */
#include <linux/cdev.h>
#include <linux/moduleparam.h>  /* needed for module parameters */
#include <linux/slab.h>        /* needed for dynamic memory management */

#define MAX_SIZE 10000

static dev_t exe_dev;
static struct cdev exe_cdev;

static char **buffer;

static int number_of_instances = 3;
module_param(number_of_instances, int, 0);

static int exe_open(struct inode* i, struct file* f) {
    pr_info("exe : open operation... major:%d, minor:%d\n",
            imajor(i),
            iminor(i));

    if (iminor(i) >= 3) {
        pr_err("Too much instances");
        return -EINVAL;
    }

    f->private_data = buffer[iminor(i)];
    pr_info("skeleton: private_data=%p\n", f->private_data);
    return 0;
}

static int exe_release(struct inode* i, struct file* f)
{
    pr_info("skeleton: release operation...\n");
    return 0;
}

static ssize_t exe_read(struct file* f, char __user* buf, size_t count, loff_t* off) {
    char* ptr_buffer;
    char* offset_ptr;

    if ((*off + count) > MAX_SIZE) {
        pr_err("Failed bad size : off: %lld, count: %lu", *off, count);
        return -1;
    }
    
    ptr_buffer = (char*)f->private_data;
    offset_ptr = ptr_buffer + *off;

    if (copy_to_user(buf, offset_ptr, count) != 0) {
        pr_err("Failed to read");
        return -1;
    }

    return count;
}

static ssize_t exe_write(struct file* f, const char __user* buf, size_t count, loff_t* off) {
    char* ptr_buffer;
    char* offset_ptr;

    if ((*off + count) > MAX_SIZE)
        return -1;

    if (count <= 0) return count;

    ptr_buffer = (char*)f->private_data;
    offset_ptr = ptr_buffer + *off;

    offset_ptr[count] = 0; // end with \0
    if (copy_from_user(offset_ptr, buf, count) != 0) {
        pr_err("Failed to read");
        return -1;
    }

    return count;
}

static struct file_operations exe_fops = {
    .owner   = THIS_MODULE,
    .open    = exe_open,
    .read    = exe_read,
    .write   = exe_write,
    .release = exe_release,
};

static int __init skeleton_init(void)
{
    int i;
    if (alloc_chrdev_region(&exe_dev, 0, number_of_instances, "mymodule") != 0) {
        pr_err("Cannot allocate char dev region");
        return -1;
    }

    cdev_init (&exe_cdev, &exe_fops);
    exe_cdev.owner = THIS_MODULE;

    if (cdev_add(&exe_cdev, exe_dev, number_of_instances)) {
        pr_err("Cannot create cdev");
        return -1;
    }
    pr_info("%d %d", MAJOR(exe_dev), MINOR(exe_dev));

    buffer = kzalloc(sizeof(char *) * number_of_instances, GFP_KERNEL);
    if (buffer == NULL) {
        pr_err("Cannot allocates instances");
        return -1;
    }
    for (i = 0; i < number_of_instances; i++) {
        buffer[i] = (char *)kzalloc(MAX_SIZE, GFP_KERNEL);
        if (buffer[i] == NULL) {
            pr_err("Cannot allocates instance %d", i);
            return -1;
        }
    }
    
	pr_info ("Linux module 02 skeleton loaded with %d instances\n", number_of_instances);
	return 0;
}

static void __exit skeleton_exit(void)
{
    int i;
    cdev_del(&exe_cdev);
    unregister_chrdev_region(exe_dev, number_of_instances);

    for (i = 0; i < number_of_instances; i++) {
        kfree(buffer[i]);
    }
    kfree(buffer);
	pr_info ("Linux module skeleton unloaded\n");
}


module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("SSY");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");