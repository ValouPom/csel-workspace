#include <linux/init.h>   /* needed for macros */
#include <linux/kernel.h> /* needed for debugging */
#include <linux/module.h> /* needed by all modules */

#include <linux/cdev.h>    /* needed for char device driver */
#include <linux/fs.h>      /* needed for device drivers */
#include <linux/uaccess.h> /* needed to copy data to/from user */

#include <linux/device.h> /* needed for sysfs handling */
#include <linux/miscdevice.h>
#include <linux/platform_device.h> /* needed for sysfs handling */
#include <linux/thermal.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/gpio.h>

#define AUTO_MODE 1
#define MANU_MODE 0
#define BASE_FREQ 2
#define LED_PIN 10
#define SECOND_TIME 1000
#define TEMP_TO_DEG(temp) (temp / 1000)

struct thermal_zone_device* thermal_zone_cpu;
static struct timer_list timer_temp;
static struct timer_list timer_freq;
static struct class* sysfs_class;
static struct device* sysfs_device;

static int freq = BASE_FREQ;
static int temp = 0;
static int mode = AUTO_MODE;
static int led_val = 0;

ssize_t sysfs_show_freq(struct device* dev,
                       struct device_attribute* attr,
                       char* buf)
{
    sprintf(buf, "%d\n", freq);
    return strlen(buf);
}
ssize_t sysfs_store_freq(struct device* dev,
                        struct device_attribute* attr,
                        const char* buf,
                        size_t count)
{
    sscanf(buf, "%d", &freq);
    return count;
}
DEVICE_ATTR(freq, 0664, sysfs_show_freq, sysfs_store_freq);

ssize_t sysfs_show_mode(struct device* dev,
                       struct device_attribute* attr,
                       char* buf)
{
    sprintf(buf, "%d\n", mode);
    return strlen(buf);
}
ssize_t sysfs_store_mode(struct device* dev,
                        struct device_attribute* attr,
                        const char* buf,
                        size_t count)
{
    sscanf(buf, "%d", &mode);
    return count;
}

DEVICE_ATTR(mode, 0664, sysfs_show_mode, sysfs_store_mode);

ssize_t sysfs_show_temp(struct device* dev,
                       struct device_attribute* attr,
                       char* buf)
{
    sprintf(buf, "%d\n", temp);
    return strlen(buf);
}
ssize_t sysfs_store_temp(struct device* dev,
                        struct device_attribute* attr,
                        const char* buf,
                        size_t count)
{
    sscanf(buf, "%d", &temp);
    return count;
}

DEVICE_ATTR(temp, 0664, sysfs_show_temp, sysfs_store_temp);

void timer_temp_callback(struct timer_list *timer)
{
    if (thermal_zone_get_temp(thermal_zone_cpu, &temp) != 0) {
        pr_err("Cannot get cpu temperature");
        return;
    }
    temp = TEMP_TO_DEG(temp);

    mod_timer(timer, jiffies + msecs_to_jiffies(SECOND_TIME));
}

int get_period(int freqency) {
    return SECOND_TIME / freqency;
}

int get_freq_auto(void) {
    if (temp < 35) return 2;
    if (temp < 40) return 5;
    if (temp < 45) return 10;
    if (temp >= 45) return 20;
    return 0;
}

void timer_freq_callback(struct timer_list *timer)
{
    int freq_auto = 0;
    if (mode == AUTO_MODE) {
        freq_auto = get_freq_auto();
    }

    led_val = !led_val;
    gpio_set_value(LED_PIN, led_val);

    if (mode == AUTO_MODE) {
        mod_timer(timer, jiffies + msecs_to_jiffies(get_period(freq_auto)));
    } else {
        mod_timer(timer, jiffies + msecs_to_jiffies(get_period(freq)));
    }
}

static int __init miniproj_module_init(void)
{
    int status = 0;

    sysfs_class = class_create(THIS_MODULE, "miniproj_class");
    sysfs_device = device_create(sysfs_class, NULL, 0, NULL, "miniproj_device");
    if (status == 0) status = device_create_file(sysfs_device, &dev_attr_freq);
    if (status == 0) status = device_create_file(sysfs_device, &dev_attr_temp);
    if (status == 0) status = device_create_file(sysfs_device, &dev_attr_mode);

    if (status != 0) {
        pr_err("Cannot create sysfs");
        return -1;
    }

    thermal_zone_cpu = thermal_zone_get_zone_by_name("cpu-thermal");

    timer_setup(&timer_temp, timer_temp_callback, 0);
    mod_timer(&timer_temp, jiffies + msecs_to_jiffies(SECOND_TIME));

    if (gpio_request(LED_PIN, "led") != 0) {
        pr_err("Cannot init gpio");
    }
    if (gpio_direction_output(LED_PIN, 0) != 0) {
        pr_err("Cannot set ouptut direction to GPIO");
    } 

    timer_setup(&timer_freq, timer_freq_callback, 0);
    mod_timer(&timer_freq, jiffies + msecs_to_jiffies(get_period(freq)));

    pr_info("Linux module miniproj loaded\n");
    return 0;
}

static void __exit miniproj_module_exit(void)
{
    del_timer(&timer_freq);

    gpio_set_value(LED_PIN, 0);
    gpio_free(LED_PIN);

    del_timer(&timer_temp);

    device_remove_file(sysfs_device, &dev_attr_freq);
    device_remove_file(sysfs_device, &dev_attr_temp);
    device_remove_file(sysfs_device, &dev_attr_mode);
    device_destroy(sysfs_class, 0);
    class_destroy(sysfs_class);

    pr_info("Linux module miniproj unloaded\n");
}

module_init (miniproj_module_init);
module_exit (miniproj_module_exit);

MODULE_AUTHOR ("SSY");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");