.data
v1: .word 3
v2: .word 4
.globl main
.text
main:
    lw $t0, v1
    li $t1, 10
    sw $t1, 0x10010000
    jr $ra