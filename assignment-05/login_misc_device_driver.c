#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

#define LOGIN "lowczarc"
#define LOGIN_SIZE 8

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
	int retval;
	size_t size_read;

	if (*offset >= LOGIN_SIZE)
		return 0;

	size_read =
	    length < LOGIN_SIZE - *offset ? length : LOGIN_SIZE - *offset;

	retval = copy_to_user(buffer, LOGIN + *offset, size_read);

	if (retval < 0)
		return retval;
	size_read -= retval;

	*offset += size_read;

	return size_read;
}

static ssize_t misc_device_write(struct file *filp, const char *buffer,
				 size_t length, loff_t * offset)
{
	char local_buffer[LOGIN_SIZE];

	if (length != LOGIN_SIZE)
		return -EINVAL;

	if (copy_from_user(local_buffer, buffer, LOGIN_SIZE))
		return -EINVAL;

	if (memcmp(local_buffer, LOGIN, LOGIN_SIZE))
		return -EINVAL;

	return LOGIN_SIZE;
}

module_init(lowczarc_init);
module_exit(lowczarc_cleanup);
