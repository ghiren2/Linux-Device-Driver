
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

dev_t mydevno; /*To get device number*/
struct cdev *dummycdev; /* To represent internally as a charater driver  */
static struct class *dummy_class; /* To create class structure */
char proc_array[20]="Hello World/n";
int len;
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

int proc_open(struct inode *inode, struct file *filp)
{
        printk(KERN_ALERT "PROC_FS Device is open\n");
        return 0;
}

int proc_close(struct inode *inode,struct file *filep)
{
        printk(KERN_ALERT "PROC_FS Device Closed\n");
        return 0;
}

ssize_t proc_read(struct file *filp,char __user *buff,size_t count ,
                loff_t *offp)
{
        printk(KERN_ALERT "PROC_FS Inside dummy_oread\n");

	if(len)
		len=0;
	else
	{	
		len=1;
		return 0;
	}
		
	copy_to_user(buff,proc_array,sizeof(proc_array));

        return count;
}

ssize_t proc_write(struct file *filp,const char __user *buff, size_t count,
                loff_t *offp )
{
        printk(KERN_ALERT "PROC_FS Inside the dummy_write\n");
	
	copy_from_user(proc_array,buff,count);
	
        return count;
}


/* Writing operation to communicate with the user space */
static struct file_operations dummy_fops={
	.open = dummy_open,
	.release = dummy_close,
	.read = dummy_read,
	.write = dummy_write,
};

static struct file_operations proc_fops={
	.open = proc_open,
	.release = proc_close,
	.read = proc_read,
	.write = proc_write,
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

	/* To create fill the class structure, creating entry in /sys/class */
	dummy_class = class_create(THIS_MODULE, "DummyClass");
	if(dummy_class == NULL)
	{
		printk(KERN_ALERT "Unable to create class structure\n");
		goto err2;
	}

	/*Creating device file*/
	if(device_create(dummy_class, NULL, mydevno,NULL ,"DummyDev") == NULL)
	{
		printk(KERN_ALERT "Unable to create device file\n");
		goto err3;
	}
	
	/*It will create a variable dummy_proc in /proc directory 
	cat /proc/dummy_proc to display the array 
	and to write something into the variable use echo*/
	proc_create("dummy_proc", 0666, NULL, &proc_fops);

	return 0;


err3:
	class_destroy(dummy_class);
err2:
	cdev_del(dummycdev); /* Deleting cdev structure */
err1:	
	unregister_chrdev_region(mydevno, 1); /*Unregistering the device number*/
	return -1;
}


void __exit char_dummy_exit(void) /* Module Exit Point */
{
	printk(KERN_ALERT "Inside the exit function\n");
	remove_proc_entry("dummy_proc",NULL);
	device_destroy(dummy_class, mydevno); /*Destroy Device File*/
	class_destroy(dummy_class); /*Destroy Clas structure*/
	cdev_del(dummycdev); /* Deleting cdev structure */
	unregister_chrdev_region(mydevno, 1); /*Unregistering the device number*/
}

MODULE_LICENSE("GPL"); /* To access the other kernel symbols*/
MODULE_AUTHOR("Hiren Gohel"); /* Author name*/


module_init(char_dummy_init);
module_exit(char_dummy_exit);
