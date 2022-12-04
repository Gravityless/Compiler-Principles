.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
array: .word 8
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

func_a:
  div $a0, $a1
  mflo $t0
  mul $t1, $t0, $a1
  sub $t2, $a0, $t1
  move $v0, $t2
  jr $ra

func_b:
  li $t0, 2
label1:
  blt $t0, $a0, label2
  j label3
label2:
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
  addi $sp, $sp, -8
  sw $a0, 0($sp)
  move $t2, $a0
  move $a0, $t2
  move $a1, $t0
  jal func_a
  lw $a0, 0($sp)
  addi $sp, $sp, 8
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
  move $t1, $v0
  move $t2, $t1
  beq $t2, $0, label4
  j label5
label4:
  move $v0, $0
  jr $ra
label5:
  addi $t3, $t0, 1
  move $t0, $t3
  j label1
label3:
  li $t4, 1
  move $v0, $t4
  jr $ra

func_c:
  li $t0, 2
label6:
  mul $t1, $t0, $t0
  bge $t1, $a0, label7
  j label8
label7:
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
  addi $sp, $sp, -4
  sw $a0, 0($sp)
  move $t3, $a0
  move $a0, $t0
  jal func_b
  lw $a0, 0($sp)
  addi $sp, $sp, 4
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
  move $t2, $v0
  bne $t2, $0, label9
  j label10
label9:
  mul $t3, $t0, $t0
  beq $t3, $a0, label11
  j label12
label11:
  li $t4, 1
  move $v0, $t4
  jr $ra
label12:
label10:
  addi $t5, $t0, 1
  move $t0, $t5
  j label6
label8:
  move $v0, $0
  jr $ra

main:
  li $t0, 2
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t1, $v0
  li $t3, 4
  mul $t2, $0, $t3
  la $t4, array
  add $t5, $t4, $t2
  sw $t1, 0($t5)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t6, $v0
  li $s0, 1
  li $s1, 4
  mul $t7, $s0, $s1
  la $s2, array
  add $s3, $s2, $t7
  sw $t6, 0($s3)
  li $s4, 0
label13:
  blt $s4, $t0, label14
  j label15
label14:
  li $s6, 4
  mul $s5, $s4, $s6
  la $s7, array
  add $t8, $s7, $s5
  lw $t9, 0($t8)
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
  move $a0, $t9
  jal func_c
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
  move $t3, $v0
  bne $t3, $0, label16
  j label17
label16:
  li $s1, 4
  mul $s0, $s4, $s1
  la $s6, array
  add $s1, $s6, $s0
  lw $t1, 0($s1)
  move $a0, $t1
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
label17:
  addi $t2, $s4, 1
  move $s4, $t2
  j label13
label15:
  move $v0, $0
  jr $ra
