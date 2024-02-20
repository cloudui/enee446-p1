 	flw	f0	x0	x	flt[0] <- x			x
	fmult.s	f0	f0	f0	flt[0] <- flt[0] * flt[0]	x^2
	lw	x1	x0	n	reg[1] <- n			counter
	flw	f2	x0	one	flt[2] <- one			coeff
	flw	f4	x0	zero	flt[4] <- zero			k
	flw	f6	x0	one	flt[6] <- one			sum
	flw	f8	x0	neg	flt[8] <- neg			-1
	flw	f10	x0	one	flt[10] <- one			1
loop	beq	x1	x0	done					Have we run n times
	fadd.s	f4	f4	f10	flt[4] <- flt[4] + flt[10]	k+1
	fdiv.s	f2	f2	f4	flt[2] <- flt[4] / flt[2]	coeff/k-1
	fadd.s	f4	f4	f10	flt[4] <- flt[4] + flt[10]	k+1
	fdiv.s	f2	f2	f4	flt[2] <- flt[4] / flt[2]	coeff/k
	fmult.s	f2	f2	f0	flt[2] <- flt[2] * flt[0]	coeff*x^2
	fmult.s	f2	f2	f8	flt[2] <- flt[2] * flt[8]	coeff*-1
	fadd.s	f6	f6	f2	flt[6] <- flt[6] * flt[2]	sum+coeff
	addi	x1	x1	#-1	reg[1] <- reg[1] - 1		n-1
	jal	x0	loop						loop
done	fsw	f6	x0	answer	flt[6] -> answer
	halt
answer	.df	0
n	.dw	10
x	.df	1.0
zero	.df	0.0
neg	.df	-1.0
one	.df	1.0
