
#include <linux/module.h>

static int __init dummy_init(void) /*Module Entry Point*/
{
	printk(KERN_ALERT "Inside the Init function\n");
	return 0;
}

void __exit dummy_exit(void)  /*Module Exit Point*/
{
	printk(KERN_ALERT "Inside the exit function\n");
}

module_init(dummy_init);
module_exit(dummy_exit);
