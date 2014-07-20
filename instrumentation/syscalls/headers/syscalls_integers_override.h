#define OVERRIDE_32_sys_mmap
#define OVERRIDE_64_sys_mmap

#ifndef CREATE_SYSCALL_TABLE

SC_TRACE_EVENT(sys_mmap,
	TP_PROTO(sc_exit(long ret,) unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, unsigned long off),
	TP_ARGS(sc_exit(ret,) addr, len, prot, flags, fd, off),
	TP_STRUCT__entry(sc_exit(__field(long, ret)) sc_in(__field_hex(unsigned long, addr)) sc_in(__field(size_t, len)) sc_in(__field(int, prot)) sc_in(__field(int, flags)) sc_in(__field(int, fd)) sc_in(__field(off_t, offset))),
	TP_fast_assign(sc_exit(tp_assign(ret, ret)) sc_in(tp_assign(addr, addr)) sc_in(tp_assign(len, len)) sc_in(tp_assign(prot, prot)) sc_in(tp_assign(flags, flags)) sc_in(tp_assign(fd, fd)) sc_in(tp_assign(offset, off))),
	TP_printk()
)

#endif /* CREATE_SYSCALL_TABLE */
