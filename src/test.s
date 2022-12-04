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

main:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  li $t0, 0
  li $t1, 1
  li $t2, 2
  li $t3, 3
  li $t4, 4
  li $t5, 5
  li $t6, 6
  li $t7, 7
  li $s0, 8
  li $s1, 9
  li $s2, 10
  li $s3, 11
  li $s4, 12
  li $s5, 13
  li $s6, 14
  li $s7, 15
  li $t8, 16
  li $t9, 17
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  li $t0, 18
  addi $sp, $sp, -4
  sw $t1, 0($sp)
  li $t1, 19
  addi $sp, $sp, -4
  sw $t2, 0($sp)
  li $t2, 20
  addi $sp, $sp, -4
  sw $t3, 0($sp)
  li $t3, 21
  addi $sp, $sp, -4
  sw $t4, 0($sp)
  li $t4, 22
  addi $sp, $sp, -4
  sw $t5, 0($sp)
  li $t5, 23
  addi $sp, $sp, -4
  sw $t6, 0($sp)
  li $t6, 24
  addi $sp, $sp, -4
  sw $t7, 0($sp)
  li $t7, 25
  addi $sp, $sp, -4
  sw $s0, 0($sp)
  lw $s0, -4($fp)
  move $a0, $s0
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  addi $sp, $sp, -4
  sw $t9, 0($sp)
  lw $t9, -8($fp)
  move $a0, $t9
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  addi $sp, $sp, -4
  sw $s0, 0($sp)
  lw $s0, -12($fp)
  move $a0, $s0
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $v0, $0
  jr $ra
