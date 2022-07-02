.global knapsack
 
# unsigned int knapsack(int* weights, unsigned int* values, unsigned int num_items,
#      int capacity, unsigned int cur_value){
 
.equ ws, 4
.text
knapsack:
	prologue:
		push %ebp
		movl %esp, %ebp
		movl %ebx, oldEbx(%ebp)
		#make space for locals
		subl $3 * ws, %esp
		#save regs
		.equ weights, (2*ws) #(%ebp)
		.equ values, (3*ws) #(%ebp)
		.equ num_items, (4*ws) #(%ebp)
		.equ capacity, (5*ws) #ebp
		.equ cur_value, (6*ws) #ebp
		.equ  i, (-1*ws)
		.equ best_value, (-2*ws) # (%ebp)
	    .equ oldEbx, (-3*ws) # (%ebp)
	movl cur_value(%ebp), %eax
	movl %eax, best_value(%ebp) # best_value = cur_value
	movl $0, %eax
#eax is i 
#edx 
#ecx
#ebx
 
knap_loop:
#i < num_items -> i - num_items < 0 negation i - num_items >= 0
cmpl num_items(%ebp), %eax # i - num_items
jge knap_loop_end
    movl weights(%ebp), %edx
    movl (%edx, %eax, ws), %edx # %edx = weights[i]
    cmpl %edx, capacity(%ebp) # capacity - weights[i]
    jl knap_else
    #edx is weights[i] so we'll not use edx until pushing arg 2 
    movl values(%ebp), %ecx # ecx = values
    movl (%ecx, %eax, ws), %ecx # ecx = values[i]
    addl cur_value(%ebp), %ecx # = ecx = values[i] + cur_value
    push %ecx # push args right to left ^
    movl capacity(%ebp), %ecx # ecx = capacity
    subl %edx, %ecx # edx = capacity - weights[i]
    push %ecx # arg 2 on stack
    movl num_items(%ebp), %ecx # ecx = num_items
    subl %eax, %ecx # num_items - i
    subl $1, %ecx # num_items - i - 1
    push %ecx # arg 3 on stack
    movl values(%ebp), %ecx # ecx = values
    leal (%ecx, %eax, ws), %ecx # ecx = values + i
    leal 1*ws(%ecx), %ecx # ecx = values + i + 1
    push %ecx # arg 4 on stack
    movl weights(%ebp), %ecx # ecx = weights
    leal (%ecx, %eax, ws), %ecx # weights + i
    leal 1*ws(%ecx), %ecx # ecx = weights + i + 1
    push %ecx # arg 5 on stack
    movl %eax, i(%ebp) # save i before function call
    call knapsack # eax edx and ecx are overwritten but eax is result
    movl best_value(%ebp), %ecx # get best value
    call max # %eax is max value
    movl %eax, best_value(%ebp) # save new best value
    movl i(%ebp), %eax # restore i
    knap_else:
 
incl %eax
jmp knap_loop
knap_loop_end:
movl best_value(%ebp), %eax # set return value
jmp epilogue # done
 
max:
#eax is a / return value
#ecx is b
cmpl %ecx, %eax # a - b > 0
jge max_end
movl %ecx, %eax # a = b , return b
max_end:
ret
 
 
	epilogue:
	movl oldEbx(%ebp), %ebx
    movl %ebp, %esp #remove locals
    pop %ebp
    ret
