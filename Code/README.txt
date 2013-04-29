Loadable Kernel Module
==============================
- Makefile
- lkm.c

make
insmod lkm.ko

Kernel Changes
==============================
Added files
******************************
- include/linux/sys_stat.h
- kernel/sys_stat.c

Modified files
******************************
- fs/read_write.c
- fs/open.c
- kernel/signal.c
