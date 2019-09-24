#include "output.s"

	.cpu generic
	.file	"io_api.c"
	.section	.rodata
	.align	3
.LC0:
	.string	"%s"
	.text
	.align	2
	.global	_write_str
	.type	_write_str, %function
_write_str:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x0, [x29,24]
	adrp	x0, .LC0
	add	x0, x0, :lo12:.LC0
	ldr	x1, [x29,24]
	bl	printf
	ldp	x29, x30, [sp], 32
	ret
	.size	_write_str, .-_write_str
	.section	.rodata
	.align	3
.LC1:
	.string	"%d"
	.text
	.align	2
	.global	_write_int
	.type	_write_int, %function
_write_int:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	w0, [x29,28]
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	ldr	w1, [x29,28]
	bl	printf
	ldp	x29, x30, [sp], 32
	ret
	.size	_write_int, .-_write_int
	.section	.rodata
	.align	3
.LC2:
	.string	"%f"
	.text
	.align	2
	.global	_write_float
	.type	_write_float, %function
_write_float:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	s0, [x29,28]
	ldr	w0, [x29,28]
	fmov	s0, w0
	fcvt	d0, s0
	fmov	x1, d0
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	fmov	d0, x1
	bl	printf
	ldp	x29, x30, [sp], 32
	ret
	.size	_write_float, .-_write_float
	.align	2
	.global	_read_int
	.type	_read_int, %function
_read_int:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	add	x1, x29, 28
	bl	__isoc99_scanf
	ldr	w0, [x29,28]
	ldp	x29, x30, [sp], 32
	ret
	.size	_read_int, .-_read_int
	.align	2
	.global	_read_float
	.type	_read_float, %function
_read_float:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	add	x1, x29, 28
	bl	__isoc99_scanf
	ldr	w0, [x29,28]
	fmov	s0, w0
	ldp	x29, x30, [sp], 32
	ret
	.size	_read_float, .-_read_float
	.align	2
	.global	main
	.type	main, %function
main:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	bl	_start_MAIN
	mov	w0, 0
	ldp	x29, x30, [sp], 16
	ret
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 4.8.2-13ubuntu1) 4.8.2 20140110 (prerelease) [ibm/gcc-4_8-branch merged from gcc-4_8-branch, revision 205847]"
