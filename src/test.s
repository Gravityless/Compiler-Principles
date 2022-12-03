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

return2:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -32
  sw $s0, 0($sp)
  sw $s1, 4($sp)
  sw $s2, 8($sp)
  sw $s3, 12($sp)
  sw $s4, 16($sp)
  sw $s5, 20($sp)
  sw $s6, 24($sp)
  sw $s7, 28($sp)
  move $sp, $fp
  addi $sp, $sp, -32
  lw $s0, 0($sp)
  lw $s1, 4($sp)
  lw $s2, 8($sp)
  lw $s3, 12($sp)
  lw $s4, 16($sp)
  lw $s5, 20($sp)
  lw $s6, 24($sp)
  lw $s7, 28($sp)
  addi $sp, $sp, 32
  lw $fp, 0($sp)
  addi $sp, $sp, 4
  move $v0, $a1
  jr $ra

main:
  li $t0, 1
  li $t1, 2
  addi $sp, $sp, -40
  sw $t0, 0($sp)
  sw $t1, 4($sp)
  sw $t2, 8($sp)
  sw $t3, 12($sp)
  sw $t4, 16($sp)
  sw $t5, 20($sp)
  sw $t6, 24($sp)
  sw $t7, 28($sp)
  sw $t8, 32($sp)
  sw $t9, 36($sp)
  move $a0, $t0
  move $a1, $t1
  jal return2
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t2, $v0
  lw $t0, 0($sp)
  lw $t1, 4($sp)
  lw $t2, 8($sp)
  lw $t3, 12($sp)
  lw $t4, 16($sp)
  lw $t5, 20($sp)
  lw $t6, 24($sp)
  lw $t7, 28($sp)
  lw $t8, 32($sp)
  lw $t9, 36($sp)
  addi $sp, $sp, 40
  move $t3, $t2
  move $a0, $t3
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $v0, $0
  jr $ra
