	addi	x1	x0	#12	X1 <- X0 + 3		n<-3
loop	beq	x1	x0	done	Have we run n times
	addi	x1	x1	#-1	X1  <- X1 - 1		X1--
	flw	f0	x2	a	FO  <- X2(a)		F0  <- a[n]
	flw	f8	x2	b	F8  <- X2(b)		F8  <- b[n]
	flw	f16	x2	c	F16 < X2(c)		F16 <- c[n]
	fmult.s	f0	f0	f8	F0  <- F0 * F8		F0  <- a[n]   * b[n]
	fadd.s	f0	f0	f16	F0  <- F0 + F16		F0  <- a[n]   + b[n] 
	fsw	f0	x2	answer	answer+X2 = [F0]
	addi	x2	x2	#4	X2  <- X2 + 4
	jal	x0	loop
done	halt
a	.df	0.01
	.df	0.02
	.df	0.03
	.df	0.04
	.df	0.05
	.df	0.06
	.df	0.07
	.df	0.08
	.df	0.09
	.df	0.10
	.df	0.11
	.df	0.12
b	.df	1
	.df	2
	.df	3
	.df	4
	.df	5
	.df	6
	.df	7
	.df	8
	.df	9
	.df	10
	.df	11
	.df	12
c	.df	100
	.df	101
	.df	102
	.df	103
	.df	104
	.df	105
	.df	106
	.df	107
	.df	108
	.df	109
	.df	110
	.df	111
answer	.df	0
	.df	0
	.df	0
	.df	0
	.df	0
	.df	0
	.df	0
	.df	0
	.df	0
	.df	0
	.df	0
	.df	0
