#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

static ssize_t misc_device_read(struct file *filp, char *buffer,
				size_t length, loff_t * offset);
static ssize_t misc_device_write(struct file *filp, const char *buffer,
				 size_t length, loff_t * offset);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lancelot Owczarczak <contact@lancelot.life>");
MODULE_DESCRIPTION("42 login misc char device driver");

static struct file_operations fops = {
	.read = misc_device_read,
	.write = misc_device_write,
};

static struct miscdevice login_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fortytwo",
	.fops = &fops,
};

static int __init lowczarc_init(void)
{
	int error = misc_register(&login_device);

	if (error) {
		printk(KERN_ERR "misc_register returned: %d\n", error);
		return error;
	}

	printk(KERN_INFO "Login Misc Char Device Driver was loaded\n");
	return 0;
}

static void __exit lowczarc_cleanup(void)
{
	misc_deregister(&login_device);

	printk(KERN_INFO "Login Misc Char Device Driver was cleaned up.\n");
}

static ssize_t misc_device_read(struct file *filp, char *buffer,
				size_t length, loff_t * offset)
{
	int retval = length < 8 ? length : 8;

	strncpy(buffer, "lowczarc", retval);

	return retval;
}

static ssize_t misc_device_write(struct file *filp, const char *buffer,
				 size_t length, loff_t * offset)
{
	if (length != 8 || strncmp(buffer, "lowczarc", 8)) {
		return -EINVAL;
	}

	return 8;
}

module_init(lowczarc_init);
module_exit(lowczarc_cleanup);
