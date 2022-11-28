FUNCTION add :
PARAM a
PARAM b
RETURN #0
FUNCTION main :
ARG y
ARG x
t1 := CALL add
z := t1
ARG y
ARG x
t2 := CALL add
z := t2
RETURN #0
.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra

add:
  move $v0, $0
  jr $ra

main:
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  addi $sp, $sp, -72
  sw $t0, 0($sp)
  sw $t1, 4($sp)
  sw $t2, 8($sp)
  sw $t3, 12($sp)
  sw $t4, 16($sp)
  sw $t5, 20($sp)
  sw $t6, 24($sp)
  sw $t7, 28($sp)
  sw $s0, 32($sp)
  sw $s1, 36($sp)
  sw $s2, 40($sp)
  sw $s3, 44($sp)
  sw $s4, 48($sp)
  sw $s5, 52($sp)
  sw $s6, 56($sp)
  sw $s7, 60($sp)
  sw $t8, 64($sp)
  sw $t9, 68($sp)
  move $a0, $t1
  move $a1, $t2
  jal add
  lw $t0, 0($sp)
  lw $t1, 4($sp)
  lw $t2, 8($sp)
  lw $t3, 12($sp)
  lw $t4, 16($sp)
  lw $t5, 20($sp)
  lw $t6, 24($sp)
  lw $t7, 28($sp)
  lw $s0, 32($sp)
  lw $s1, 36($sp)
  lw $s2, 40($sp)
  lw $s3, 44($sp)
  lw $s4, 48($sp)
  lw $s5, 52($sp)
  lw $s6, 56($sp)
  lw $s7, 60($sp)
  lw $t8, 64($sp)
  lw $t9, 68($sp)
  addi $sp, $sp, 72
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t0, $v0
  move $t3, $t0
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  addi $sp, $sp, -72
  sw $t0, 0($sp)
  sw $t1, 4($sp)
  sw $t2, 8($sp)
  sw $t3, 12($sp)
  sw $t4, 16($sp)
  sw $t5, 20($sp)
  sw $t6, 24($sp)
  sw $t7, 28($sp)
  sw $s0, 32($sp)
  sw $s1, 36($sp)
  sw $s2, 40($sp)
  sw $s3, 44($sp)
  sw $s4, 48($sp)
  sw $s5, 52($sp)
  sw $s6, 56($sp)
  sw $s7, 60($sp)
  sw $t8, 64($sp)
  sw $t9, 68($sp)
  move $a0, $t1
  move $a1, $t2
  jal add
  lw $t0, 0($sp)
  lw $t1, 4($sp)
  lw $t2, 8($sp)
  lw $t3, 12($sp)
  lw $t4, 16($sp)
  lw $t5, 20($sp)
  lw $t6, 24($sp)
  lw $t7, 28($sp)
  lw $s0, 32($sp)
  lw $s1, 36($sp)
  lw $s2, 40($sp)
  lw $s3, 44($sp)
  lw $s4, 48($sp)
  lw $s5, 52($sp)
  lw $s6, 56($sp)
  lw $s7, 60($sp)
  lw $t8, 64($sp)
  lw $t9, 68($sp)
  addi $sp, $sp, 72
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t4, $v0
  move $t3, $t4
  move $v0, $0
  jr $ra
