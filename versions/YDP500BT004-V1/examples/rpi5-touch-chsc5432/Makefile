obj-m += chsc5432-touch.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sudo mkdir -p /lib/modules/$(shell uname -r)/extra
	sudo cp chsc5432-touch.ko /lib/modules/$(shell uname -r)/extra/
	sudo depmod -a

uninstall:
	sudo rm -f /lib/modules/$(shell uname -r)/extra/chsc5432-touch.ko
	sudo depmod -a
