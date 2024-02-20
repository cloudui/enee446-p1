start	flw	f0	x0	a	flt[0] <- a
	flw	f2	x0	b	flt[2] <- b
	flw	f4	x0	c	flt[4] <- c
	lw	x1	x0	n	reg[1] <- n
	flw	f6	x0	neg	flt[6] <- neg
	flw	f10	x0	two	flt[10] <- two
	fmult.s	f8	f6	f2	flt[8] <- flt[6] * flt[2]	-b
	fdiv.s	f8	f8	f0	flt[8] <- flt[8] * flt[0]	-b/a
	fadd.s	f8	f8	f6	flt[8] <- flt[8] * flt[6]	-b/a-1
loop	beq	x1	x0	done					Have we run n times
	addi	x1	x1	#-1	reg[1] <- reg[1] - 1		c--
	fmult.s	f12	f8	f8	flt[12] <- flt[8] * flt[8]	x^2
	fmult.s	f12	f12	f0	flt[12] <- flt[12] * flt[0]	ax^2
	fmult.s	f14	f8	f2	flt[14] <- flt[8] * flt[2]	bx
	fadd.s	f12	f12	f4	flt[12] <- flt[12] + flt[4]	ax^2+c
	fadd.s	f12	f12	f14	flt[12] <- flt[12] + flt[14]	ax^2+bx+c
	fmult.s	f14	f0	f8	flt[14] <- flt[0] * flt[8]	ax
	fmult.s	f14	f10	f14	flt[14] <- flt[10] * flt[14]	2ax
	fadd.s	f14	f14	f2	flt[14] <- flt[14] + flt[2]	2ax + b
	fdiv.s	f12	f12	f14	flt[12] <- flt[12] / flt[14]	f(x)/f'(x)
	fsub.s	f8	f8	f12	flt[8] <- flt[8] - flt[12]	G- f(x)/f'(x)
	jal	x0	loop
done	fsw	f8	x0	answer	flt[8] -> answer
	halt
a	.df	1.0
b	.df	2.0
c	.df	0
neg	.df	-1.0
n	.dw	3
two	.df	2.0
answer	.df	0
