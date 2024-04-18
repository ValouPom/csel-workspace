#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/io.h>		/* needed for mmio handling */

//#define PAGE_SIZE 0x1000
struct data_t {
	int* chipReg;
	int chipsId[4];
};

static int __init skeleton_init(void)
{
	pr_info ("Linux module 02 skeleton loaded\n");
	unsigned int chipid[4]={[0]=0,};
	unsigned char* reg = ioremap(0x01c14000, PAGE_SIZE);
	unsigned char* regTemp = ioremap(0x01c25000, PAGE_SIZE);

	chipid[0] = ioread32(reg + 0x200);
	chipid[1] = ioread32(reg + 0x204);
	chipid[2] = ioread32(reg + 0x208);
	chipid[3] = ioread32(reg + 0x20c);

	pr_info("Chip 1 %08x\n", chipid[0]);
	pr_info("Chip 2 %08x\n", chipid[1]);
	pr_info("Chip 3 %08x\n", chipid[2]);
	pr_info("Chip 4 %08x\n", chipid[3]);

	int tempRegVal = ioread32(regTemp + 0x80);
	long temp = (-1191 * tempRegVal / 10) + 223000;

	pr_info("temp: %ld\n", temp);

	iounmap(reg);
	iounmap(regTemp);
	return 0;
}

static void __exit skeleton_exit(void)
{
	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("SSY");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
