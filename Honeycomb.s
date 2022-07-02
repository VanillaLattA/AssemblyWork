.global get_combs
.equ ws, 4
.text
#int** get_combs(int* items, int k, int len)
find_combs:
prologue2:
	push %ebp
	movl %esp, %ebp
	movl %ebx, oldEbx(%ebp)
	#make space for locals
	subl $3*ws, %esp
	#save regs
	.equ list, (2*ws) #(%ebp)
	.equ comb, (3*ws) #(%ebp)
	.equ start, (4*ws) #(%ebp)
	.equ end, (5*ws) #(%ebp)
	.equ index, (6*ws)
	.equ k, (7*ws)
	.equ answer, (8*ws)
	.equ rowIndex, (9*ws)
	.equ x, (-1*ws)
	.equ i, (-2*ws) #(%ebp)
    .equ j, (-3*ws) # (%ebp)
    movl k(%ebp), %eax
    cmpl index(%ebp), %eax
    jnz elseState
        movl $0, %ebx
        for_top:
        cmpl k(%ebp), %ebx # j < k == j - k < 0 neg j - k >= 0
        jge for_bot
        movl answer(%ebp), %eax
        movl rowIndex(%ebp), %ecx
        movl (%eax, %ecx, ws), %eax # eax = answer[rowIndex]
        movl comb(%ebp), %edx 
        movl (%edx, %ebx, ws), %edx # ecx = comb[j]
        movl %edx, (%eax, %ebx, ws) # answer[rowIndex][j] = comb[j]
        incl %ebx
        jmp for_top
        for_bot:
        movl rowIndex(%ebp), %eax
        incl %eax # return rowIndex + 1
        jmp epilogue2
    elseState:
        movl start(%ebp), %ebx
        for_else:
        cmpl end(%ebp), %ebx # i<=end -> i - end <= 0 neg i - end > 0
        jg for_else_end
        movl comb(%ebp), %eax
        movl list(%ebp), %ecx
        movl (%ecx, %ebx, ws), %ecx # list[i]
        movl index(%ebp), %edx
        movl %ecx, (%eax, %edx, ws) # comb[index] = list[i]
        push rowIndex(%ebp)
        push answer(%ebp)
        push k(%ebp)
        movl index(%ebp), %ecx
        incl %ecx
        push %ecx
        push end(%ebp)
        incl %ebx # i + 1, also start of next loop
        push %ebx
        push comb(%ebp)
        push list(%ebp)
        call find_combs
        movl %eax, rowIndex(%ebp) # rowIndex = return value
        jmp for_else
        for_else_end:
        movl rowIndex(%ebp), %eax
        jmp epilogue2
epilogue2:
movl oldEbx(%ebp), %ebx
movl %ebp, %esp #remove locals
pop %ebp
ret
get_combs:
	prologue:
		push %ebp
		movl %esp, %ebp
		movl %ebx, oldEbx(%ebp)
		#make space for locals
		subl $3*ws, %esp
		#save regs
		.equ items, (2*ws) #(%ebp)
		.equ k, (3*ws) #(%ebp)
		.equ len, (4*ws) #(%ebp)
		.equ list, (-1*ws)
		.equ number_combs, (-2*ws) #(%ebp)
	    .equ oldEbx, (-3*ws) # (%ebp)
    movl k(%ebp), %eax
    push %eax
    movl len(%ebp), %eax
    push %eax
    movl oldEbx(%ebp), %ebx
    call max
    call num_combs
    movl %eax, number_combs(%ebp)
    addl $2*ws, %esp # remove locals
    shll $2, %eax # eax = eax * 4
    push %eax
    call malloc
    addl $1*ws, %esp
	movl %eax, list(%ebp)
	movl $0, %ebx # i = 0 
#ebx is i 
#eax is list 
#ecx is 
#edx is 
	setup_for_begin:
	    cmpl number_combs(%ebp), %ebx #i < x == i - x < 0 neg i - x >= 0
	    jge setup_for_end
	    movl k(%ebp), %eax #eax = k
	    shll $2, %eax # eax = k * sizeof(int)
	    push %eax
	    call malloc
	    addl $1*ws, %esp # remove stack variable for malloc
	    movl list(%ebp), %ecx # ecx = list
	    movl %eax, (%ecx, %ebx, ws) # list[i] = malloc(k * sizeof(int))
	    incl %ebx
	    jmp setup_for_begin
	setup_for_end:
	    push $0
	    push list(%ebp)
	    push k(%ebp)
	    push $0
	    movl len(%ebp), %eax
	    decl %eax
	    push %eax
	    push $0
	    movl k(%ebp), %eax #eax = k
	    shll $2, %eax # eax = k * sizeof(int)
	    push %eax
	    call malloc
	    addl $1*ws, %esp
	    push %eax
	    movl %eax, %ebx # save for calling free
	    push items(%ebp)
	    call find_combs
	    push %ebx
	    call free
	    addl $1*ws, %esp
	    movl list(%ebp), %eax
	    jmp epilogue
	    
	    

epilogue:
movl oldEbx(%ebp), %ebx
movl %ebp, %esp #remove locals
pop %ebp
ret
