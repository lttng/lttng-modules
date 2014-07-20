#ifndef CONFIG_UID16

#define OVERRIDE_32_getuid16
#define OVERRIDE_32_getgid16
#define OVERRIDE_32_geteuid16
#define OVERRIDE_32_getegid16
#define OVERRIDE_32_setuid16
#define OVERRIDE_32_setgid16
#define OVERRIDE_32_setfsuid16
#define OVERRIDE_32_setfsgid16
#define OVERRIDE_32_setreuid16
#define OVERRIDE_32_setregid16
#define OVERRIDE_32_fchown16
#define OVERRIDE_32_setresuid16
#define OVERRIDE_32_setresgid16

#define OVERRIDE_TABLE_32_getuid16
#define OVERRIDE_TABLE_32_getgid16
#define OVERRIDE_TABLE_32_geteuid16
#define OVERRIDE_TABLE_32_getegid16
#define OVERRIDE_TABLE_32_setuid16
#define OVERRIDE_TABLE_32_setgid16
#define OVERRIDE_TABLE_32_setreuid16
#define OVERRIDE_TABLE_32_setregid16
#define OVERRIDE_TABLE_32_fchown16
#define OVERRIDE_TABLE_32_setfsuid16
#define OVERRIDE_TABLE_32_setfsgid16
#define OVERRIDE_TABLE_32_setresuid16
#define OVERRIDE_TABLE_32_setresgid16

#endif

#ifdef CREATE_SYSCALL_TABLE

#define OVERRIDE_TABLE_32_mmap
TRACE_SYSCALL_TABLE(mmap, mmap, 90, 6)

#endif
