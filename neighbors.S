#include <sys/regdef.h>

.text
.align 2
.globl neighbors
.ent neighbors

neighbors:
    .frame $fp, 40, ra
    .set noreoder
    .cpload t9
    .set reoder

    sub sp, sp, 40
    .cprestore 0

