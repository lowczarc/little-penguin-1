MODULE_NAME=fix_style
CFLAGS_fix_style.o = -Wall -Wtype-limits

obj-m += $(MODULE_NAME).o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
