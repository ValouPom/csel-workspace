#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/io.h>		/* needed for mmio handling */
#include <linux/cdev.h>

#include <linux/device.h>          /* needed for sysfs handling */
#include <linux/platform_device.h> /* needed for sysfs handling */
#include <linux/miscdevice.h>

#define MAX_SIZE 10000


struct config_test {
    int id;
    char name[20];
};

//static dev_t exe_dev;
//static struct cdev exe_cdev;

static char buffer[MAX_SIZE];
static struct config_test cfg;

ssize_t sysfs_show_attr(struct device* dev,
                        struct device_attribute* attr,
                        char* buf)
{
    sprintf(buf, "%d;%s", cfg.id, cfg.name);
    return strlen(buf);
}
ssize_t sysfs_store_attr(struct device* dev,
                         struct device_attribute* attr,
                         const char* buf,
                         size_t count)
{
    sscanf(buf, "%d;%s", &cfg.id, cfg.name);

    return count;
}

DEVICE_ATTR(cfg, 0664, sysfs_show_attr, sysfs_store_attr);

static int exe_open(struct inode* i, struct file* f) {
    pr_info("exe : open operation... major:%d, minor:%d\n",
            imajor(i),
            iminor(i));
    return 0;
}

static int exe_release(struct inode* i, struct file* f)
{
    pr_info("skeleton: release operation...\n");
    return 0;
}

static ssize_t exe_read(struct file* f, char __user* buf, size_t count, loff_t* off) {
    char* offset_ptr;

    if ((*off + count) > MAX_SIZE) {
        pr_err("Failed bad size : off: %lld, count: %lu", *off, count);
        return -1;
    }

    offset_ptr = buffer + *off;

    if (copy_to_user(buf, offset_ptr, count) != 0) {
        pr_err("Failed to read");
        return -1;
    }

    return count;
}

static ssize_t exe_write(struct file* f, const char __user* buf, size_t count, loff_t* off) {
    char* offset_ptr;
    if ((*off + count) > MAX_SIZE)
        return -1;

    if (count <= 0) return count;

    offset_ptr = buffer + *off;

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

static struct miscdevice misc_device = {
    .fops = &exe_fops,
    .minor = MISC_DYNAMIC_MINOR,
    .mode  = 0,
    .name  = "mymodule",
};

static int __init skeleton_init(void)
{
    /*
    if (alloc_chrdev_region(&exe_dev, 0, 1, "exe_cdev") != 0)
    {
        pr_err("Cannot allocate char dev region");
        return -1;
    }

    cdev_init (&exe_cdev, &exe_fops);
    exe_cdev.owner = THIS_MODULE;

    cdev_add(&exe_cdev, exe_dev, 1);
    pr_info("%d %d", MAJOR(exe_dev), MINOR(exe_dev));
    */

    if (misc_register(&misc_device) != 0) {
        return -1;
    }

    if (device_create_file(misc_device.this_device, &dev_attr_cfg) != 0) {
        return -1;
    }
    
	pr_info ("Linux module 05 skeleton loaded\n");
	return 0;
}

static void __exit skeleton_exit(void)
{
    /*
    cdev_del(&exe_cdev);
    unregister_chrdev_region(exe_dev, 1);
    */
   misc_deregister(&misc_device);
	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("SSY");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");