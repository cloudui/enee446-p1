	lw	x1	x0	op1	reg[1] <- op1
	lw	x2	x0	op2	reg[2] <- op2
	lw	x3	x0	op3	reg[3] <- op3
	add	x4	x1	x2	reg[4] <- reg[1] + reg[2]
	sub	x5	x4	x3	reg[5] <- reg[4] - reg[3]
	sw	x5	x0	answer	reg[5] -> answer
done	halt
op1	.dw	50			
op2	.dw	30
op3	.dw	20
answer	.dw	0
