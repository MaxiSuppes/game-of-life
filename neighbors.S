#include <sys/regdef.h>

// SRA
#define STACK_SIZE 40
#define SRA_GP 36
#define SRA_FP 32

// LTA
#define LTA_a 0
#define LTA_i 4
#define LTA_j 8
#define LTA_M 12
#define LTA_N 16

// Caller ABA
#define ABA_a 40
#define ABA_i 44
#define ABA_j 48
#define ABA_M 52
#define ABA_N 56

.text
.align 2
.globl neighbors
.ent neighbors

neighbors:
// To be position independent
.frame fp, STACK_SIZE, ra
.set noreorder
.cpload t9
.set reorder

// Creating stack
subu sp, sp, STACK_SIZE
sw gp, SRA_GP(sp)
sw fp, SRA_FP(sp)
move fp, sp

// Loading parameters
sw a0, ABA_a(fp) # matrix pointer
sw a1, ABA_i(fp) # i
sw a2, ABA_j(fp) # j
sw a3, ABA_M(fp) # M
lw t0, ABA_N(fp) # N

// Storing in LTA
sw t0, LTA_N(fp)

lw t1, ABA_a(fp)
sw t1, LTA_a(fp)

lw t2, ABA_i(fp)
sw t2, LTA_i(fp)

lw t3, ABA_j(fp)
sw t3, LTA_j(fp)

lw t4, ABA_M(fp)
sw t4, LTA_M(fp)

// Defining variables
li t0, 0                # living_neighbors = t0 = 0

addi t1, t2, -1         # t1 = i - 1
div t1, t4              # H0 = (i-1) % M
mfhi t1                 # upPosition = t1 = (i-1) % M

lw t6, LTA_i(fp)        # t6 = i
addi t2, t6, 1          # t2 = i + 1
div t2, t4              # H0 = (i+1) % M
mfhi t2                 # downPosition = t2 = (i+1) % M

lw t6, LTA_j(fp)        # t6 = j
lw t7, LTA_N(fp)        # t7 = N
addi t3, t6, -1         # t3 = j - 1
div t3, t7              # H0 = (j-1) % N
mfhi t3                 # leftPosition = t3 = (j-1) % N

addi t4, t6, 1          # t4 = j + 1
div t4, t7              # H0 = (j+1) % N
mfhi t4                 # rightPosition = t4 = (j+1) % N

columns_edge_case:
bne t6, 0, rows_edge_case  # if (j != 0) go to ciclo
addi t3, t7, -1         # leftPosition = N - 1

rows_edge_case:
lw t6, LTA_i(fp)        # t6 = i
lw t7, LTA_M(fp)        # t7 = M
bne t6, 0, up_and_left  # if (i != 0) go to ciclo
addi t1, t7, -1         # upPosition = M - 1

up_and_left:
lw t6, LTA_a(fp)        # t6 = puntero a (0,0)
lw t7, LTA_N(fp)        # t7 = N
mul t5, t1, t7          # index = t5 = upPosition * N
add t5, t5, t3          # index = t5 = upPosition * N + leftPosition
addu t5, t5, t6         # index = t5 = puntero a (upPosition, leftPosition)
li t7, '-'              # t7 = '-'
lbu t5, (t5)            # t5 = matrix[upPosition, leftPosition]
bne t5, t7, up
addi t0, t0, 1          # living_neighbors += 1

up:
lw t6, LTA_j(fp)        # t6 = j
lw t7, LTA_N(fp)        # t7 = N
mul t5, t1, t7          # index = t5 = upPosition * N
add t5, t5, t6          # index = t5 = upPosition * N + j
lw t6, LTA_a(fp)        # t6 = puntero a (0,0)
addu t5, t5, t6         # index = t5 = puntero a (upPosition, j)
li t7, '-'              # t7 = '-'
lbu t5, (t5)            # t5 = matrix[upPosition, j]
bne t5, t7, up_and_right
addi t0, t0, 1          # living_neighbors += 1

up_and_right:
lw t7, LTA_N(fp)        # t7 = N
mul t5, t1, t7          # index = t5 = upPosition * N
add t5, t5, t4          # index = t5 = upPosition * N + rightPosition
lw t6, LTA_a(fp)        # t6 = puntero a (0,0)
addu t5, t5, t6         # index = t5 = puntero a (upPosition, rightPosition)
li t7, '-'              # t7 = '-'
lbu t5, (t5)            # t5 = matrix[upPosition, rightPosition]
bne t5, t7, left
addi t0, t0, 1          # living_neighbors += 1

left:
lw t6, LTA_i(fp)        # t6 = i
lw t7, LTA_N(fp)        # t7 = N
mul t5, t6, t7          # index = t5 = i * N
add t5, t5, t3          # index = t5 = i * N + leftPosition
lw t6, LTA_a(fp)        # t6 = puntero a (0,0)
addu t5, t5, t6         # index = t5 = puntero a (i, leftPosition)
li t7, '-'              # t7 = '-'
lbu t5, (t5)            # t5 = matrix[i, leftPosition]
bne t5, t7, right
addi t0, t0, 1          # living_neighbors += 1

right:
lw t6, LTA_i(fp)        # t6 = i
lw t7, LTA_N(fp)        # t7 = N
mul t5, t6, t7          # index = t5 = i * N
add t5, t5, t4          # index = t5 = i * N + rightPosition
lw t6, LTA_a(fp)        # t6 = puntero a (0,0)
addu t5, t5, t6         # index = t5 = puntero a (i, rightPosition)
li t7, '-'              # t7 = '-'
lbu t5, (t5)            # t5 = matrix[i, rightPosition]
bne t5, t7, down_and_left
addi t0, t0, 1          # living_neighbors += 1

down_and_left:
lw t7, LTA_N(fp)        # t7 = N
mul t5, t2, t7          # index = t5 = downPosition * N
add t5, t5, t3          # index = t5 = downPosition * N + leftPosition
lw t6, LTA_a(fp)        # t6 = puntero a (0,0)
addu t5, t5, t6         # index = t5 = puntero a (downPosition, leftPosition)
li t7, '-'              # t7 = '-'
lbu t5, (t5)            # t5 = matrix[downPosition, leftPosition]
bne t5, t7, down
addi t0, t0, 1          # living_neighbors += 1

down:
lw t6, LTA_j(fp)        # t6 = j
lw t7, LTA_N(fp)        # t7 = N
mul t5, t2, t7          # index = t5 = downPosition * N
add t5, t5, t6          # index = t5 = downPosition * N + j
lw t6, LTA_a(fp)        # t6 = puntero a (0,0)
addu t5, t5, t6         # index = t5 = puntero a (downPosition, j)
li t7, '-'              # t7 = '-'
lbu t5, (t5)            # t5 = matrix[downPosition, j]
bne t5, t7, down_and_right
addi t0, t0, 1          # living_neighbors += 1

down_and_right:
lw t7, LTA_N(fp)        # t7 = N
mul t5, t2, t7          # index = t5 = downPosition * N
add t5, t5, t4          # index = t5 = downPosition * N + rightPosition
lw t6, LTA_a(fp)        # t6 = puntero a (0,0)
addu t5, t5, t6         # index = t5 = puntero a (downPosition, rightPosition)
li t7, '-'              # t7 = '-'
lbu t5, (t5)            # t5 = matrix[downPosition, rightPosition]
bne t5, t7, return
addi t0, t0, 1          # living_neighbors += 1

return:
move v0, t0

// Unwinding stack
lw fp, SRA_FP(sp)
lw gp, SRA_GP(sp)
addu sp, sp, STACK_SIZE
jr ra
.end neighbors






