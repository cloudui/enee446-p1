
/*
 *
 * 
 * fu.h

   This module was originally written by Paul Kohout.  It was later
   adapted for the RISC-V ISA.

 * 
 * Donald Yeung
 */


#include <stdio.h>

#define MAXMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 32 /* number of machine registers */

#define FIELD_OPCODE(instr) ((unsigned int)((instr)&0x007F))
#define FIELD_FUNC3(instr)  ((unsigned int)(((instr)>>12)&0x07))
#define FIELD_FUNC7(instr)  ((unsigned int)(((instr)>>25)&0x07F))
#define FIELD_RS1(instr)    ((unsigned int)(((instr)>>15)&0x001F))
#define FIELD_RS2(instr)    ((unsigned int)(((instr)>>20)&0x001F))
#define FIELD_RD(instr)     ((unsigned int)(((instr)>>7)&0x001F))
#define FIELD_IMM_I(instr)  ((((signed int)instr)>>20))
#define FIELD_IMM_S(instr)  ((((((signed int)instr)>>25)<<5)|(((instr)>>7)&0x1F)))
#define FIELD_OFFSET(instr) ((((signed int)instr)>>12))

#define FU_GROUP_INT     0
#define FU_GROUP_ADD     1
#define FU_GROUP_MULT    2
#define FU_GROUP_DIV     3
#define FU_GROUP_MEM     4
#define FU_GROUP_BRANCH  5
#define FU_GROUP_NONE    -1
#define FU_GROUP_INVALID -2
#define FU_GROUP_HALT    -3

#define OPERATION_ADD   0
#define OPERATION_SUB   2
#define OPERATION_MULT  4
#define OPERATION_DIV   5
#define OPERATION_SLL   6
#define OPERATION_SRL   7
#define OPERATION_AND   8
#define OPERATION_OR    9
#define OPERATION_XOR   10
#define OPERATION_LOAD  11
#define OPERATION_STORE 12
#define OPERATION_JAL   14
#define OPERATION_JR    15
#define OPERATION_JALR  16
#define OPERATION_BEQ   17
#define OPERATION_BNE   18
#define OPERATION_SLT   19
#define OPERATION_SLTU  20
#define OPERATION_NONE  -1

#define DATA_TYPE_W    1
#define DATA_TYPE_F    2
#define DATA_TYPE_NONE -1

#define NOP 0x00000006

typedef struct _op_info_t {
  const char *name;
  const int fu_group_num;
  const int operation;
  const int data_type;
} op_info_t;

typedef struct _op_level1_t {
  const struct _op_info_t info;
  const struct _op_level2_t * const level2_table;
} op_level1_t;

typedef struct _op_level2_t {
  const struct _op_info_t info;
  const struct _op_level3_t * const level3_table;
} op_level2_t;

typedef struct _op_level3_t {
  const struct _op_info_t info;
} op_level3_t;

extern const op_level1_t op_table[];
extern const op_level2_t op_int_table[];
extern const op_level2_t op_fp_table[];
extern const op_level2_t op_br_table[];
extern const op_level2_t op_imm_table[];
extern const op_level2_t op_load_table[];
extern const op_level2_t op_store_table[];
extern const op_level3_t op_intrr_table[];
extern const op_level3_t op_fprr_table[];

extern const char fu_group_int_name[];
extern const char fu_group_add_name[];
extern const char fu_group_mult_name[];
extern const char fu_group_div_name[];


/* union to handle multiple fixed-point types */
typedef union _int_t {
  signed long w;
  unsigned long wu;
} int_t;

typedef union _operand_t {
  int_t integer;
  float flt;
} operand_t;


/* functional unit and memory port structures */
typedef struct _fu_int_stage_t {
  int num_cycles;
  int current_cycle;
  int instr;
  struct _fu_int_stage_t *prev;
} fu_int_stage_t;

typedef struct _fu_int_t {
  char *name;
  fu_int_stage_t *stage_list;
  struct _fu_int_t *next;
} fu_int_t;


typedef struct _fu_fp_stage_t {
  int num_cycles;
  int current_cycle;
  int instr;
  struct _fu_fp_stage_t *prev;
} fu_fp_stage_t;

typedef struct _fu_fp_t {
  char *name;
  fu_fp_stage_t *stage_list;
  struct _fu_fp_t *next;
} fu_fp_t;


/* writeback pipeline register */
typedef struct _wb_t {
  int instr;
} wb_t;


extern int fu_int_read(fu_int_t **, FILE *);
extern int fu_fp_read(fu_fp_t **, FILE *);

extern int issue_fu_int(fu_int_t *, int, unsigned long pc);
extern int issue_fu_fp(fu_fp_t *, int);

extern void advance_fu_int(fu_int_t *, wb_t *);
extern void advance_fu_fp(fu_fp_t *, wb_t *);

extern int fu_int_done(fu_int_t *);
extern int fu_fp_done(fu_fp_t *);

const op_info_t *decode_instr(int, int *);
extern void perform_operation(int, unsigned long, operand_t, operand_t);
