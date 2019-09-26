.data
_pi: .word 0
.align 3
.text
_start_sqr:
str x30, [sp, #0]
str x29, [sp, #-8]
add x29, sp, #-8
add sp, sp, #-16
str x19, [x29, #-8]
str x20, [x29, #-16]
str x21, [x29, #-24]
str x22, [x29, #-32]
str x23, [x29, #-40]
str x24, [x29, #-48]
str x25, [x29, #-56]
str x26, [x29, #-64]
str x27, [x29, #-72]
str x28, [x29, #-80]
str x29, [x29, #-88]
.data
_AR_0: .word 88
.align 3
.text
ldr w19, _AR_0
sub sp, sp, w19
.data
_const_1: .word -16
.align 3
.text
ldrsw x19, _const_1
sub x19, x29, x19
ldr s19, [x19, #0]
str s19, [sp, #0]
sub sp, sp, #8
.data
_const_2: .word -16
.align 3
.text
ldrsw x19, _const_2
sub x19, x29, x19
ldr s19, [x19, #0]
add sp, sp, #8
ldr s20, [sp, #0]
fmul s20, s20, s19
fmov s0, s20
b _end_sqr
_end_sqr:
ldr x19, [x29, #-8]
ldr x20, [x29, #-16]
ldr x21, [x29, #-24]
ldr x22, [x29, #-32]
ldr x23, [x29, #-40]
ldr x24, [x29, #-48]
ldr x25, [x29, #-56]
ldr x26, [x29, #-64]
ldr x27, [x29, #-72]
ldr x28, [x29, #-80]
ldr x29, [x29, #-88]
ldr x30, [x29, #8]
add sp, x29, #8
ldr x29, [x29, #0]
ret x30
.text
_start_calarea:
str x30, [sp, #0]
str x29, [sp, #-8]
add x29, sp, #-8
add sp, sp, #-16
str x19, [x29, #-8]
str x20, [x29, #-16]
str x21, [x29, #-24]
str x22, [x29, #-32]
str x23, [x29, #-40]
str x24, [x29, #-48]
str x25, [x29, #-56]
str x26, [x29, #-64]
str x27, [x29, #-72]
str x28, [x29, #-80]
str x29, [x29, #-88]
.data
_AR_3: .word 88
.align 3
.text
ldr w19, _AR_3
sub sp, sp, w19
.data
_integer_const_4: .word 4
.align 3
.text
ldr w19, _integer_const_4
sub sp, sp, w19
ldr s19, _pi
str s19, [sp, #0]
sub sp, sp, #8
.data
_integer_const_5: .word 16
.align 3
.text
ldr w19, _integer_const_5
sub sp, sp, w19
.data
_const_6: .word -16
.align 3
.text
ldrsw x21, _const_6
sub x21, x29, x21
ldr w21, [x21, #0]
.data
_const_7: .word 8
.align 3
.text
ldrsw x19, _const_7
add x19, sp, x19
scvtf s21, w21
str s21, [x19, #0]
bl _start_sqr
.text
ldr w19, _integer_const_5
add sp, sp, w19
fmov s19, s0
add sp, sp, #8
ldr s21, [sp, #0]
fmul s21, s21, s19
.data
_const_8: .word 92
.align 3
.text
ldrsw x19, _const_8
sub x19, x29, x19
str s21, [x19, #0]
.data
_const_9: .word 92
.align 3
.text
ldrsw x19, _const_9
sub x19, x29, x19
ldr s19, [x19, #0]
fmov s0, s19
b _end_calarea
_end_calarea:
ldr x19, [x29, #-8]
ldr x20, [x29, #-16]
ldr x21, [x29, #-24]
ldr x22, [x29, #-32]
ldr x23, [x29, #-40]
ldr x24, [x29, #-48]
ldr x25, [x29, #-56]
ldr x26, [x29, #-64]
ldr x27, [x29, #-72]
ldr x28, [x29, #-80]
ldr x29, [x29, #-88]
ldr x30, [x29, #8]
add sp, x29, #8
ldr x29, [x29, #0]
ret x30
.text
_start_floor:
str x30, [sp, #0]
str x29, [sp, #-8]
add x29, sp, #-8
add sp, sp, #-16
str x19, [x29, #-8]
str x20, [x29, #-16]
str x21, [x29, #-24]
str x22, [x29, #-32]
str x23, [x29, #-40]
str x24, [x29, #-48]
str x25, [x29, #-56]
str x26, [x29, #-64]
str x27, [x29, #-72]
str x28, [x29, #-80]
str x29, [x29, #-88]
.data
_AR_10: .word 88
.align 3
.text
ldr w19, _AR_10
sub sp, sp, w19
.data
_integer_const_11: .word 4
.align 3
.text
ldr w21, _integer_const_11
sub sp, sp, w21
.data
_const_12: .word -16
.align 3
.text
ldrsw x21, _const_12
sub x21, x29, x21
ldr s21, [x21, #0]
.data
_const_13: .word 92
.align 3
.text
ldrsw x22, _const_13
sub x22, x29, x22
fcvtzs w21, s21
str w21, [x22, #0]
.data
_const_14: .word 92
.align 3
.text
ldrsw x21, _const_14
sub x21, x29, x21
ldr w21, [x21, #0]
mov w0, w21
b _end_floor
_end_floor:
ldr x19, [x29, #-8]
ldr x20, [x29, #-16]
ldr x21, [x29, #-24]
ldr x22, [x29, #-32]
ldr x23, [x29, #-40]
ldr x24, [x29, #-48]
ldr x25, [x29, #-56]
ldr x26, [x29, #-64]
ldr x27, [x29, #-72]
ldr x28, [x29, #-80]
ldr x29, [x29, #-88]
ldr x30, [x29, #8]
add sp, x29, #8
ldr x29, [x29, #0]
ret x30
.text
_start_MAIN:
b _global_init
_global_init_back:
str x30, [sp, #0]
str x29, [sp, #-8]
add x29, sp, #-8
add sp, sp, #-16
str x19, [x29, #-8]
str x20, [x29, #-16]
str x21, [x29, #-24]
str x22, [x29, #-32]
str x23, [x29, #-40]
str x24, [x29, #-48]
str x25, [x29, #-56]
str x26, [x29, #-64]
str x27, [x29, #-72]
str x28, [x29, #-80]
str x29, [x29, #-88]
.data
_AR_15: .word 88
.align 3
.text
ldr w19, _AR_15
sub sp, sp, w19
.data
_integer_const_16: .word 12
.align 3
.text
ldr w22, _integer_const_16
sub sp, sp, w22
.data
_string_const_17: .asciz "Enter an Integer :"
.align 3
.text
ldr x22, =_string_const_17
mov x0, x22
bl _write_str
bl _read_int
mov w22, w0
.data
_const_18: .word 92
.align 3
.text
ldrsw x23, _const_18
sub x23, x29, x23
str w22, [x23, #0]
.data
_integer_const_19: .word 16
.align 3
.text
ldr w22, _integer_const_19
sub sp, sp, w22
.data
_const_20: .word 92
.align 3
.text
ldrsw x23, _const_20
sub x23, x29, x23
ldr w23, [x23, #0]
.data
_const_21: .word 8
.align 3
.text
ldrsw x22, _const_21
add x22, sp, x22
str w23, [x22, #0]
bl _start_calarea
.text
ldr w22, _integer_const_19
add sp, sp, w22
fmov s22, s0
.data
_const_22: .word 96
.align 3
.text
ldrsw x23, _const_22
sub x23, x29, x23
str s22, [x23, #0]
.data
_const_23: .word 96
.align 3
.text
ldrsw x22, _const_23
sub x22, x29, x22
ldr s22, [x22, #0]
str s22, [sp, #0]
sub sp, sp, #8
.data
_integer_const_24: .word 16
.align 3
.text
ldr w22, _integer_const_24
sub sp, sp, w22
.data
_const_25: .word 96
.align 3
.text
ldrsw x23, _const_25
sub x23, x29, x23
ldr s23, [x23, #0]
.data
_const_26: .word 8
.align 3
.text
ldrsw x22, _const_26
add x22, sp, x22
str s23, [x22, #0]
bl _start_floor
.text
ldr w22, _integer_const_24
add sp, sp, w22
mov w22, w0
add sp, sp, #8
ldr s23, [sp, #0]
scvtf s22, w22
fsub s23, s23, s22
.data
_const_27: .word 100
.align 3
.text
ldrsw x22, _const_27
sub x22, x29, x22
str s23, [x22, #0]
.data
_const_28: .word 96
.align 3
.text
ldrsw x22, _const_28
sub x22, x29, x22
ldr s22, [x22, #0]
fmov s0, s22
bl _write_float
.data
_string_const_29: .asciz " "
.align 3
.text
ldr x22, =_string_const_29
mov x0, x22
bl _write_str
.data
_const_30: .word 100
.align 3
.text
ldrsw x22, _const_30
sub x22, x29, x22
ldr s22, [x22, #0]
fmov s0, s22
bl _write_float
.data
_string_const_31: .asciz "\n"
.align 3
.text
ldr x22, =_string_const_31
mov x0, x22
bl _write_str
.data
_integer_const_32: .word 0
.align 3
.text
ldr w22, _integer_const_32
mov w0, w22
b _end_MAIN
_end_MAIN:
ldr x19, [x29, #-8]
ldr x20, [x29, #-16]
ldr x21, [x29, #-24]
ldr x22, [x29, #-32]
ldr x23, [x29, #-40]
ldr x24, [x29, #-48]
ldr x25, [x29, #-56]
ldr x26, [x29, #-64]
ldr x27, [x29, #-72]
ldr x28, [x29, #-80]
ldr x29, [x29, #-88]
ldr x30, [x29, #8]
add sp, x29, #8
ldr x29, [x29, #0]
ret x30
.text
_global_init:
.data
_float_const_33: .float 3.141600
.align 3
.text
ldr s23, _float_const_33
ldr x24, =_pi
str s23, [x24, #0]
b _global_init_back
