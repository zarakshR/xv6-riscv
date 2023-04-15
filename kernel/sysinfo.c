#include "kernel/sysinfo.h"
#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/spinlock.h"
#include "kernel/param.h"
#include "kernel/proc.h"
#include "kernel/defs.h"

void sys_sysinfo(void) {

    struct sysinfo sinfo = {
        .freemem = 420,
        .nproc = 69,
    };

    uint64 ptr;

    argaddr(0, &ptr);

    copyout(myproc()->pagetable, ptr, (char *) &sinfo, sizeof(struct sysinfo));
}
