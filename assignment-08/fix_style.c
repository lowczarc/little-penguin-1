#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

// Dont have a license, LOL
MODULE_LICENSE("LICENSE");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp, char __user * user, size_t size,
			 loff_t * offs);
static ssize_t myfd_write(struct file *fp, const char __user * user,
			  size_t size, loff_t * offs);

static struct file_operations myfd_fops = {
	// TODO: Verify this line is useful
	.owner = THIS_MODULE,
	.read = &myfd_read,
	.write = &myfd_write
};

static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops
};

char str[PAGE_SIZE];

static int __init myfd_init(void)
{
	int retval;

	// TODO: Use the result for error handling
	retval = misc_register(&myfd_device);
	return 0;
}

static void __exit myfd_cleanup(void)
{
}

static ssize_t myfd_read(struct file *fp, char __user * user, size_t size,
			 loff_t * offs)
{
	size_t t, i;
	char *rev_str;

	// TODO: Verify this malloc size
	// TODO: Free or remove this malloc
	rev_str = kmalloc(sizeof(char) * PAGE_SIZE * 2, GFP_KERNEL);
	// TODO: Verify if the \0 isn't copied at the start
	for (t = strlen(str) - 1, i = 0; t >= 0; t--, i++) {
		rev_str[i] = str[t];
	}
	// TODO: Verify if the \0 is set in the right place
	rev_str[i] = 0;
	// TODO: Verify simple_read_from_buffer call access_ok
	return simple_read_from_buffer(user, size, offs, rev_str, i);
}

static ssize_t myfd_write(struct file *fp, const char __user * user,
			  size_t size, loff_t * offs)
{
	ssize_t res;

	// TODO: Add a size verification
	// TODO: Verify simple_read_from_buffer call access_ok
	res = simple_write_to_buffer(str, size, offs, user, size) + 1;
	// TODO: Verify if res can't be less than size
	str[size + 1] = 0;

	return res;
}

module_init(myfd_init);
module_exit(myfd_cleanup)
