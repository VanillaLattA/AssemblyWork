#EDX:EAX concatenates them both so that it turns into a 64 bit number.


# 1. Do .long stuff to get space for each of the num1 and num2
# 2. Create labels for num1 upper and num1 lower. Then do the same for num2 upper and num2 lower
# 3. Then use these labels and first add the lowers of both num1 and num2.
# 4. See if there is a carry
# 5. Then add the num1 upper and the num2 upper values plus the carry we find.

# No need to ask for a user's input. You can just assign them a value using .long.





.global _start

.data
    # i should write how much space the variable will hold.
    # refer to an example --> .long --> assigns values to integers.
    num1:
        .long -45867
        .long 562437
    num2:
        .long -48758
        .long 10

#.text section is where you write your code in
.text

_start:
    movl $0, %eax #EAX will store the lower sum
    movl $0, %edx #EDX will store the upper sum
    mov num1, %edx
    mov num1+4 ,%eax
    # adding the lower bits
    add num2+4,%eax
    # if no carry, then jump to label and add the upper bits
    jnc nocarry
    # if there is carry, then add 1 to upper bits
    add $1 , %edx

nocarry:

    # adding upper bits
    add num2, %edx
    
done:
    nop
