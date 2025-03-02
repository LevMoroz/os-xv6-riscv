.data
#Обсуждали, что отрицательные тут не считаем.
#gets пришлось подправить, чтобы \n не попадал в строку (зачем так сделали?)

msg_intro: .asciz "Write two numbers with whitespace to add:\n"
msg_err: .asciz "Not correct format of numbers!\n"

lim: .asciz "|%s|\n"
res_format: .asciz "Result: %d\n"

#define buf_len 100

buf: .space buf_len

.text
.global main

main:
    la a0, msg_intro
    call printf

    la a0, buf
    li a1, buf_len
    call gets

    la a0, lim
    la a1, buf
    call printf
    
    la a0, buf
    li a1, ' '
    call strchr

    beq a0, x0, error

    mv s1, a0
    sb x0, 0(s1)
    addi s1, s1, 1

    la a0, buf
    jal checker

    mv a0, s1
    jal checker

    la a0, buf
    call atoi
    mv s2, a0

    mv a0, s1
    call atoi

    add a1, a0, s2
    la a0, res_format
    call printf

    li a7, 2
    li a0, 0
    ecall

checker:
    li t2, '0'
    li t3, '9'

    lb t1, 0(a0)
    beq t1, x0, error

loop:
    lb t1, 0(a0)

    beq t1, x0, return
    blt t1, t2, error
    bgt t1, t3, error

    addi a0, a0, 1
    j loop

return:
    ret


error:
    la a0, msg_err
    call printf

    li a7, 2
    li a0, 1
    ecall
