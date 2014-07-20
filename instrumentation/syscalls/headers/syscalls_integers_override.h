#define OVERRIDE_32_sys_mmap
#define OVERRIDE_64_sys_mmap

#ifndef CREATE_SYSCALL_TABLE

SC_TRACE_EVENT(sys_mmap,
	TP_PROTO(sc_exit(long ret,) unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, unsigned long off),
	TP_ARGS(sc_exit(ret,) addr, len, prot, flags, fd, off),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) __field_hex(unsigned long, addr) __field(size_t, len) __field(int, prot) __field(int, flags) __field(int, fd) __field(off_t, offset)),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) tp_assign(addr, addr) tp_assign(len, len) tp_assign(prot, prot) tp_assign(flags, flags) tp_assign(fd, fd) tp_assign(offset, off)),
	TP_printk()
)

#endif /* CREATE_SYSCALL_TABLE */
