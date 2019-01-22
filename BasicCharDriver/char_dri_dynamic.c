
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/kernel.h>

dev_t mydevno; /*To get device number*/
struct cdev *dummycdev; /* To represent internally as a charater driver  */

int dummy_open(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "Device is open\n");
	return 0;
}

int dummy_close(struct inode *inode,struct file *filep)
{
	printk(KERN_ALERT "Device Closed\n");
	return 0;
}

ssize_t dummy_read(struct file *filp,char __user *buff,size_t count , 
		loff_t *offp)
{
	printk(KERN_ALERT "Inside dummy_oread\n");
	return count;
}

ssize_t dummy_write(struct file *filp,const char __user *buff, size_t count,
		loff_t *offp )
{
	printk(KERN_ALERT "Inside the dummy_write\n");
	return count;
}

/* Writing operation to communicate with the user space */
struct file_operations dummy_fops={
	.open = dummy_open,
	.release = dummy_close,
	.read = dummy_read,
	.write = dummy_write,
};

static int __init char_dummy_init(void) /* Module Entry Point */
{
	int ret,major,minor;
	printk(KERN_ALERT "Inside the init function\n");
	
	/* To Allocate Device Number Dynamically,i.e Kernel will
	assign device number  */
	ret = alloc_chrdev_region(&mydevno, 0, 1, "Dummy_chardev");
	if(ret < 0)
	{
		printk(KERN_ALERT "Unable to allocate device number\n");
		return -1;
	}
		
	minor = MINOR(mydevno);	/* Macro to get the minor number*/
	major = MAJOR(mydevno); /* Macro to get the major number*/

	/* Set up cdev structure which represents driver as a 
		charater driver internally  */
	dummycdev = cdev_alloc();
	dummycdev->ops = &dummy_fops;
	
	/* After setting up cdev structure time to notify the kernel */
	ret = cdev_add(dummycdev, mydevno, 1);
	if(ret < 0)
	{
		printk(KERN_ALERT "Unable to add cdev structure/n");
		goto err1;
	}

	return 0;

err1:	
	unregister_chrdev_region(mydevno, 1); /*Unregistering the device number*/
	return -1;
}


void __exit char_dummy_exit(void) /* Module Exit Point */
{
	printk(KERN_ALERT "Inside the exit function\n");
	
	cdev_del(dummycdev); /* Deleting cdev structure */
	unregister_chrdev_region(mydevno, 1); /*Unregistering the device number*/
}

MODULE_LICENSE("GPL"); /* To access the other kernel symbols*/
MODULE_AUTHOR("Hiren Gohel"); /* Author name*/


module_init(char_dummy_init);
module_exit(char_dummy_exit);
