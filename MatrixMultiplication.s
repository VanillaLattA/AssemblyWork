.global matMult
 
# int** matMult(int **a, int num_rows_a, int num_cols_a, int** b, int num_rows_b, int num_cols_b);
 
 
.equ ws, 4
.text
matMult:
	prologue:
		push %ebp
		movl %esp, %ebp
		#make space for locals
		subl $7 * ws, %esp
		movl %ebx, oldEbx(%ebp)
		movl %esi, oldEsi(%ebp)
		movl %edi, oldEdi(%ebp)
		#save regs
		.equ a, (2*ws) #(%ebp)
		.equ num_rows_a, (3*ws) #(%ebp)
		.equ num_cols_a, (4*ws) #(%ebp)
		.equ b, (5*ws) #ebp
		.equ num_rows_b, (6*ws) #ebp
		.equ num_cols_b, (7*ws)
		.equ  i, (-1*ws)
		.equ  j, (-2*ws) 
		.equ  k, (-3*ws)
		.equ result, (-4*ws)
		.equ oldEbx, (-5*ws)
		.equ oldEsi, (-6*ws)
		.equ oldEdi, (-7*ws)
 
movl num_rows_a(%ebp), %eax
shll $2, %eax # eax = eax * 4 for malloc((int**) * 2)
push %eax
call malloc
addl $1*ws, %esp # remove malloc arg from stack
movl %eax, result(%ebp) # put result in result
 
# eax is result
# ecx is i
# ebx is j
# edx is num_rows_a
# esi is num_cols_b
movl $0, %ecx
movl $0, %ebx
movl num_rows_a(%ebp), %edx
movl num_cols_b(%ebp), %esi
 
for_loop_1_begin:
# i < num_rows_a == num_rows_a - i >= 0
cmpl %ecx, %edx
jle for_loop_1_end
movl %ecx, i(%ebp)
movl num_cols_b(%ebp), %eax
shll $2, %eax
push %eax
call malloc # eax now holds address of result
addl $1*ws, %esp
movl i(%ebp), %ecx # restore i
movl result(%ebp), %edx # borrow edx for moving malloc
movl %eax, (%edx, %ecx, ws) # result[i] = malloc of row
movl num_rows_a(%ebp), %edx #restore edx
movl result(%ebp), %eax #restore eax = result
 
movl (%eax, %ecx, ws), %eax # eax = C[row] before loop for optimization
movl $0, %ebx # set j = 0 before loop
 
    for_loop_1_inner:
    cmpl %ebx, %esi #j < num_cols_b == num_cols_b - j >= 0
    jle for_loop_1_inner_end
    movl $0, (%eax, %ebx, ws) # C[row][col] = 0 initialize the malloc to 0
    incl %ebx
    jmp for_loop_1_inner
    for_loop_1_inner_end:
 
incl %ecx
jmp for_loop_1_begin
for_loop_1_end:
movl result(%ebp), %eax
# eax is b
# ecx is i
# ebx is j
# edi is k
# edx is free, a
# esi is free, result
movl $0, %ecx # i = 0
movl $0, %ebx #reset loop j = 0
movl $0, %edi # k = 0
add_loop_start:
# i < num_rows_a == num_rows_a - i >= 0
cmpl %ecx, num_rows_a(%ebp)
jle add_loop_end
    movl $0, %ebx # j = 0
    add2_loop:
    cmpl %ebx, num_cols_b(%ebp) #j < num_cols_b == num_cols_b - j >= 0
    jle add2_loop_end
        movl $0, %edi # k = 0
        add3_loop:
        # k < num_rows_a == num_rows_a - k >= 0
        cmpl %edi, num_cols_a(%ebp)
        jle add3_loop_end
        # result[i][j] += a[i][k] * b[k][j];
        movl a(%ebp), %edx # edx = a
        movl (%edx, %ecx, ws), %edx # edx = a[i]
        movl (%edx, %edi, ws), %edx # edx = a[i][k]
        movl b(%ebp), %eax # %eax = b
        movl (%eax, %edi, ws), %eax # %eax = b[k]
        movl (%eax, %ebx, ws), %eax # %eax = b[k][j]
        imull %edx # edx/eax = eax * edx with lower half in eax
        movl result(%ebp), %esi # esi = result
        movl (%esi, %ecx, ws), %esi # %esi = result[i]
        addl %eax, (%esi, %ebx, ws) # result[i][j] += a[i][k] * b[k][j]
        incl %edi
        jmp add3_loop
        add3_loop_end:
    incl %ebx
    jmp add2_loop
    add2_loop_end:
incl %ecx
jmp add_loop_start
add_loop_end:
 
movl result(%ebp), %eax # set return value
epilogue:
    movl oldEbx(%ebp), %ebx
    movl oldEsi(%ebp), %esi # restore esi
    movl oldEdi(%ebp), %edi # restore edi
    movl %ebp, %esp #remove locals
    pop %ebp
    ret
