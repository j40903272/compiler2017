.data
_delta: .word 0
.align 3
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
_AR_0: .word 88
.align 3
.text
ldr w19, _AR_0
sub sp, sp, w19
.data
_int_const_1: .word 4
.align 3
.text
ldr w19, _int_const_1
sub sp, sp, w19
.data
_const_2: .word 0
.align 3
.text
ldr w19, _const_2
sub x19, x29, x19
ldr s19, [x19, #0]
.data
_const_3: .word 92
.align 3
.text
ldr w20, _const_3
sub x20, x29, x20
fcvtzs w19, s19
str w19, [x20, #0]
.data
_const_4: .word 92
.align 3
.text
ldr w19, _const_4
sub x19, x29, x19
ldr w19, [x19, #0]
mov w0, w19
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
_start_ceil:
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
_AR_5: .word 88
.align 3
.text
ldr w19, _AR_5
sub sp, sp, w19
.data
_int_const_6: .word 8
.align 3
.text
ldr w20, _int_const_6
sub sp, sp, w20
_IF_7:
.data
_const_8: .word 0
.align 3
.text
ldr w20, _const_8
sub x20, x29, x20
ldr s20, [x20, #0]
str s20, [sp, #0]
sub sp, sp, #8
.data
_int_const_9: .word 0
.align 3
.text
ldr w20, _int_const_9
add sp, sp, #8
ldr s21, [sp, #0]
scvtf s20, w20
fcmp s21, s20
bgt _ELSE_10
mov w21, #0
b _END_10
_ELSE_10:
mov w21, #1
_END_10:
cmp w21, #0
beq _ELSE_7
.data
_SP_11: .skip 8
.text
ldr x20, =_SP_11
mov x21, sp
str x21, [x20, #0]
.data
_const_12: .word 0
.align 3
.text
ldr w20, _const_12
sub x20, x29, x20
ldr s20, [x20, #0]
str s20, [sp, #0]
sub sp, sp, #8
ldr s20, _delta
add sp, sp, #8
ldr s21, [sp, #0]
fadd s21, s21, s20
.data
_const_13: .word 92
.align 3
.text
ldr w20, _const_13
sub x20, x29, x20
fcvtzs w21, s21
str w21, [x20, #0]
ldr x20, =_SP_11
ldr x21, [x20, #0]
mov sp, x21
b _END_IF_7
_ELSE_7:
.data
_SP_14: .skip 8
.text
ldr x20, =_SP_14
mov x21, sp
str x21, [x20, #0]
.data
_const_15: .word 0
.align 3
.text
ldr w20, _const_15
sub x20, x29, x20
ldr s20, [x20, #0]
str s20, [sp, #0]
sub sp, sp, #8
ldr s20, _delta
add sp, sp, #8
ldr s21, [sp, #0]
fsub s21, s21, s20
.data
_const_16: .word 92
.align 3
.text
ldr w20, _const_16
sub x20, x29, x20
fcvtzs w21, s21
str w21, [x20, #0]
ldr x20, =_SP_14
ldr x21, [x20, #0]
mov sp, x21
_END_IF_7:
.data
_const_17: .word 92
.align 3
.text
ldr w20, _const_17
sub x20, x29, x20
ldr w20, [x20, #0]
mov w0, w20
b _end_ceil
_end_ceil:
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
_AR_18: .word 88
.align 3
.text
ldr w19, _AR_18
sub sp, sp, w19
.data
_int_const_19: .word 8
.align 3
.text
ldr w21, _int_const_19
sub sp, sp, w21
.data
_string_const_20: .asciz "Enter number :"
.align 3
.text
ldr x21, =_string_const_20
mov x0, x21
bl _write_str
bl _read_float
fmov s21, s0
.data
_const_21: .word 92
.align 3
.text
ldr w22, _const_21
sub x22, x29, x22
str s21, [x22, #0]
bl _start_ceil
mov w21, w0
mov w0, w21
bl _write_int
.data
_string_const_22: .asciz "\n"
.align 3
.text
ldr x21, =_string_const_22
mov x0, x21
bl _write_str
bl _start_floor
mov w21, w0
mov w0, w21
bl _write_int
.data
_string_const_23: .asciz "\n"
.align 3
.text
ldr x21, =_string_const_23
mov x0, x21
bl _write_str
bl _start_ceil
mov w21, w0
str w21, [sp, #0]
sub sp, sp, #8
bl _start_floor
mov w21, w0
add sp, sp, #8
ldr w22, [sp, #0]
add w22, w22, w21
str w22, [sp, #0]
sub sp, sp, #8
.data
_float_const_24: .float 2.000000
.align 3
.text
ldr s21, _float_const_24
add sp, sp, #8
ldr w22, [sp, #0]
scvtf s22, w22
fdiv s22, s22, s21
.data
_const_25: .word 96
.align 3
.text
ldr w21, _const_25
sub x21, x29, x21
str s22, [x21, #0]
.data
_const_26: .word 96
.align 3
.text
ldr w21, _const_26
sub x21, x29, x21
ldr s21, [x21, #0]
fmov s0, s21
bl _write_float
.data
_string_const_27: .asciz "\n"
.align 3
.text
ldr x21, =_string_const_27
mov x0, x21
bl _write_str
.data
_int_const_28: .word 0
.align 3
.text
ldr w21, _int_const_28
mov w0, w21
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
