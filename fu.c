
/*
 * 
 * fu.c
 * 
   This module was originally written by Paul Kohout.  It was later
   adapted for the RISC-V ISA.

 * Donald Yeung
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fu.h"
#include "pipeline.h"

#define MAX_FIELD_LENGTH 100

const char fu_group_int_name[] = "INT";
const char fu_group_add_name[] =  "ADD";
const char fu_group_mult_name[] =  "MULT";
const char fu_group_div_name[] =  "DIV";


/*
  Instruction Decode Tables

  {{name, fu_group_num, operation, data_type}, sub_table}
*/

const op_level1_t op_table[] = {
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_int_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_fp_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"jal",    FU_GROUP_BRANCH, OPERATION_JAL,  DATA_TYPE_NONE},NULL},
  {{"jalr",   FU_GROUP_BRANCH, OPERATION_JALR, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_br_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_imm_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_load_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_store_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{"halt",   FU_GROUP_HALT,   OPERATION_NONE, DATA_TYPE_NONE},NULL}
};

const op_level2_t op_int_table[] = {
  {{"add",    FU_GROUP_INT,    OPERATION_ADD,  DATA_TYPE_NONE},NULL},
  {{"and",    FU_GROUP_INT,    OPERATION_AND,  DATA_TYPE_NONE},NULL},
  {{"or",     FU_GROUP_INT,    OPERATION_OR,   DATA_TYPE_NONE},NULL},
  {{"xor",    FU_GROUP_INT,    OPERATION_XOR,  DATA_TYPE_NONE},NULL},
  {{"sll",    FU_GROUP_INT,    OPERATION_SLL,  DATA_TYPE_NONE},NULL},
  {{"srl",    FU_GROUP_INT,    OPERATION_SRL,  DATA_TYPE_NONE},NULL},
  {{"slt",    FU_GROUP_INT,    OPERATION_SLT,  DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_intrr_table},
};

const op_level2_t op_fp_table[] = {
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},op_fprr_table},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
};

const op_level2_t op_br_table[] = {
  {{"beq",    FU_GROUP_BRANCH, OPERATION_BEQ,  DATA_TYPE_NONE},NULL},
  {{"bne",    FU_GROUP_BRANCH, OPERATION_BNE,  DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
};

const op_level2_t op_imm_table[] = {
  {{"addi",   FU_GROUP_INT,    OPERATION_ADD,  DATA_TYPE_NONE},NULL},
  {{"andi",   FU_GROUP_INT,    OPERATION_AND,  DATA_TYPE_NONE},NULL},
  {{"ori",    FU_GROUP_INT,    OPERATION_OR,   DATA_TYPE_NONE},NULL},
  {{"xori",   FU_GROUP_INT,    OPERATION_XOR,  DATA_TYPE_NONE},NULL},
  {{"slli",   FU_GROUP_INT,    OPERATION_SLL,  DATA_TYPE_NONE},NULL},
  {{"srli",   FU_GROUP_INT,    OPERATION_SRL,  DATA_TYPE_NONE},NULL},
  {{"slti",   FU_GROUP_INT,    OPERATION_SLT,  DATA_TYPE_NONE},NULL},
  {{"sltiu",  FU_GROUP_INT,    OPERATION_SLT,  DATA_TYPE_NONE},NULL},
};

const op_level2_t op_load_table[] = {
  {{"lw",     FU_GROUP_MEM,    OPERATION_LOAD, DATA_TYPE_W},   NULL},
  {{"flw",    FU_GROUP_MEM,    OPERATION_LOAD, DATA_TYPE_F},   NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
};

const op_level2_t op_store_table[] = {
  {{"sw",     FU_GROUP_MEM,    OPERATION_STORE,DATA_TYPE_W},   NULL},
  {{"fsw",    FU_GROUP_MEM,    OPERATION_STORE,DATA_TYPE_F},   NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
  {{NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},NULL},
};

const op_level3_t op_intrr_table[] = {
  {"sltu",   FU_GROUP_INT,    OPERATION_SLTU, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {"sub",    FU_GROUP_INT,    OPERATION_SUB,  DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE}
};

const op_level3_t op_fprr_table[] = {
  {"fadd.s", FU_GROUP_ADD,    OPERATION_ADD,  DATA_TYPE_F},
  {"fsub.s", FU_GROUP_ADD,    OPERATION_SUB,  DATA_TYPE_F},
  {"fmult.s",FU_GROUP_MULT,   OPERATION_MULT, DATA_TYPE_F},
  {"fdiv.s", FU_GROUP_DIV,    OPERATION_DIV,  DATA_TYPE_F},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE},
  {NULL,     FU_GROUP_INVALID,OPERATION_NONE, DATA_TYPE_NONE}
};


state_t *
state_create(int *data_count, FILE *bin_file, FILE *fu_file) {
  state_t *state;
  fu_int_stage_t *cur_int_stage;
  fu_fp_stage_t *cur_fp_stage;
  char field[MAX_FIELD_LENGTH];
  char field_fmt[16];
  int num;
  int i;

  state = (state_t *)malloc(sizeof(state_t));
  if(state == NULL) {
    fprintf(stderr, "error: unable to allocate resources\n");
    return NULL;
  }

  memset(state, 0, sizeof(state_t));

  /* read machine-code file into instruction/data memory (starting at address 0) */
  i=0;
  while (!feof(bin_file)) {
    if (fread(&state->mem[i], 1, 1, bin_file) != 0) {
      i++;
    } else if (!feof(bin_file)) {
      fprintf(stderr, "error: cannot read address 0x%X from binary file\n", i);
      return NULL;      
    }
  }
  if (data_count != NULL)
    *data_count = i;

  /* initialize if_id */
  state->if_id.instr = NOP;

  /* allocate and initialize functional unit linked lists */
  while (!feof(fu_file)) {

    sprintf(field_fmt, " %%%d[^,\n]", MAX_FIELD_LENGTH);
    if (fscanf(fu_file, field_fmt, field) != 1) {
      fprintf(stderr, "error: cannot parse options file\n");
      return NULL;
    }
    i = strlen(field)-1;
    while (i > 0 && (field[i] == ' ' || field[i] == '\t'))
      field[i--] = 0;

    if (strcmp(field, fu_group_int_name) == 0) {
      if (fu_int_read(&state->fu_int_list, fu_file) != 0) {
	return NULL;
      }
    } else if (strcmp(field, fu_group_add_name) == 0) {
      if (fu_fp_read(&state->fu_add_list, fu_file) != 0) {
	return NULL;
      }
    } else if (strcmp(field, fu_group_mult_name) == 0) {
      if (fu_fp_read(&state->fu_mult_list, fu_file) != 0) {
	return NULL;
      }
    } else if (strcmp(field, fu_group_div_name) == 0) {
      if (fu_fp_read(&state->fu_div_list, fu_file) != 0) {
	return NULL;
      }
    } else {
      fprintf(stderr,"error: invalid functional unit group name\n");
      return NULL;
    }
  }
  if (state->fu_int_list == NULL) {
    fprintf(stderr,"error: no %s functional units\n", fu_group_int_name);
    return NULL;
  }
  if (state->fu_add_list == NULL) {
    fprintf(stderr,"error: no %s functional units\n", fu_group_add_name);
    return NULL;
  }
  if (state->fu_mult_list == NULL) {
    fprintf(stderr,"error: no %s functional units\n", fu_group_mult_name);
    return NULL;
  }
  if (state->fu_div_list == NULL) {
    fprintf(stderr,"error: no %s functional units\n", fu_group_div_name);
    return NULL;
  }

  /* initialize wb ports */
  state->int_wb.instr = 0/*NOP*/;
  state->fp_wb.instr = 0/*NOP*/;

  state->fetch_lock = FALSE;

  return state;
}


/* functions to parse FU file */
int
fu_int_read(fu_int_t **fu_int_list, FILE *file) {
  fu_int_t *fu_int;
  fu_int_stage_t *stage;
  char field[MAX_FIELD_LENGTH];
  char field_fmt[16];
  int num;
  int i;

  sprintf(field_fmt, ", %%%d[^,\n]", MAX_FIELD_LENGTH);
  if (fscanf(file, field_fmt, field) != 1) {
    fprintf(stderr, "error: cannot parse options file\n");
    return -1;
  }
  i = strlen(field)-1;
  while (i > 0 && (field[i] == ' ' || field[i] == '\t'))
    field[i--] = 0;

  if (*fu_int_list == NULL) {
    *fu_int_list = (fu_int_t *)malloc(sizeof(fu_int_t));
    fu_int = *fu_int_list;
  } else {
    fu_int = *fu_int_list;
    while (fu_int->next != NULL) fu_int = fu_int->next;
    fu_int->next = (fu_int_t *)malloc(sizeof(fu_int_t));
    fu_int = fu_int->next;
  }
  if (fu_int == NULL) {
    fprintf(stderr, "error: unable to allocate resources\n");
    return -1;
  }

  fu_int->name = (char *)malloc(strlen(field)+1);
  if (fu_int->name == NULL) {
    fprintf(stderr, "error: unable to allocate resources\n");
    return -1;
  }
  fu_int->stage_list = NULL;
  fu_int->next = NULL;
  strcpy(fu_int->name, field);

  stage = NULL;
  while (fscanf(file, ", %d", &num) == 1) {

    if(num <= 0) {
      fprintf(stderr, "error: functional unit '%s' has a stage with an invalid number of cycles\n", fu_int->name);
      return -1;
    }

    stage = (fu_int_stage_t *)malloc(sizeof(fu_int_stage_t));
    if (stage == NULL) {
      fprintf(stderr, "error: unable to allocate resources\n");
      return -1;
    }
    stage->prev = fu_int->stage_list;
    fu_int->stage_list = stage;
    stage->num_cycles = num;
    stage->current_cycle = -1;
  }
  if (fu_int->stage_list == NULL) {
    fprintf(stderr, "error: functional unit '%s' has no stages\n", fu_int->name);
    return -1;
  }

  return 0;
}


int
fu_fp_read(fu_fp_t **fu_fp_list,FILE *file) {
  fu_fp_t *fu_fp;
  fu_fp_stage_t *stage;
  char field[MAX_FIELD_LENGTH];
  char field_fmt[16];
  int num;
  int i;

  sprintf(field_fmt,", %%%d[^,\n]",MAX_FIELD_LENGTH);
  if(fscanf(file,field_fmt,field) != 1) {
    fprintf(stderr,"error: cannot parse options file\n");
    return -1;
  }
  i = strlen(field)-1;
  while(i > 0 && (field[i] == ' ' || field[i] == '\t'))
    field[i--] = 0;

  if(*fu_fp_list == NULL) {
    *fu_fp_list = (fu_fp_t *)malloc(sizeof(fu_fp_t));
    fu_fp = *fu_fp_list;
  } else {
    fu_fp = *fu_fp_list;
    while(fu_fp->next != NULL) fu_fp = fu_fp->next;
    fu_fp->next = (fu_fp_t *)malloc(sizeof(fu_fp_t));
    fu_fp = fu_fp->next;
  }
  if(fu_fp == NULL) {
    fprintf(stderr,"error: unable to allocate resources\n");
    return -1;
  }

  fu_fp->name = (char *)malloc(strlen(field)+1);
  if(fu_fp->name == NULL) {
    fprintf(stderr,"error: unable to allocate resources\n");
    return -1;
  }
  fu_fp->stage_list = NULL;
  fu_fp->next = NULL;
  strcpy(fu_fp->name,field);

  stage = NULL;
  while(fscanf(file,", %d",&num) == 1) {

    if(num <= 0) {
      fprintf(stderr,"error: functional unit '%s' has a stage with an invalid number of cycles\n",fu_fp->name);
      return -1;
    }

    stage = (fu_fp_stage_t *)malloc(sizeof(fu_fp_stage_t));
    if(stage == NULL) {
      fprintf(stderr,"error: unable to allocate resources\n");
      return -1;
    }
    stage->prev = fu_fp->stage_list;
    fu_fp->stage_list = stage;
    stage->num_cycles = num;
    stage->current_cycle = -1;
  }
  if(fu_fp->stage_list == NULL) {
    fprintf(stderr,"error: functional unit '%s' has no stages\n",fu_fp->name);
    return -1;
  }

  return 0;
}


/* Functions to allocate functional units */
int
issue_fu_int(fu_int_t *fu_list, int instr, unsigned long pc) {
  fu_int_t *fu;
  fu_int_stage_t *stage;

  fu = fu_list;
  while (fu != NULL) {
    stage = fu->stage_list;
    while (stage->prev != NULL)
      stage = stage->prev;
    if (stage->current_cycle == -1) {
      stage->current_cycle = stage->num_cycles-1;
      stage->instr = instr;
      return 0;
    }
    fu = fu->next;
  }
  return -1;   // structural hazard... stall
}


int
issue_fu_fp(fu_fp_t *fu_list, int instr) {
  fu_fp_t *fu;
  fu_fp_stage_t *stage;

  fu = fu_list;
  while (fu != NULL) {
    stage = fu->stage_list;
    while (stage->prev != NULL)
      stage = stage->prev;
    if (stage->current_cycle == -1) {
      stage->current_cycle = stage->num_cycles-1;
      stage->instr = instr;
      return 0;
    }
    fu = fu->next;
  }
  return -1;   // structural hazard... stall
}


/* functions to cycle functional units */
void
advance_fu_int(fu_int_t *fu_list, wb_t *int_wb) {
  fu_int_t *fu;
  fu_int_stage_t *stage, *next_stage;
  int i;

  fu = fu_list;
  while (fu != NULL) {
    stage = fu->stage_list;
    next_stage = NULL;
    while (stage != NULL) {
      switch (stage->current_cycle) {
	/* is fu stage free? */
      case -1:
	break;                                      /* do nothing */

	/* is fu stage done processing? */
      case 0:
	if (next_stage == NULL) {                    /* is this the last stage in the fu? */
	  int_wb->instr = stage->instr;
	  stage->current_cycle = -1;
	} else {
	  if (next_stage->current_cycle == -1) {     /* move to next fu stage */
	    next_stage->current_cycle = next_stage->num_cycles-1;
	    next_stage->instr = stage->instr;
	    stage->current_cycle = -1;
	  }
	}
	break;

	/*  fu stage is still processing */
      default:
	stage->current_cycle--;
      }
      next_stage = stage;
      stage = stage->prev;
    }
    fu = fu->next;
  }
}


void
advance_fu_fp(fu_fp_t *fu_list, wb_t *fp_wb) {
  fu_fp_t *fu;
  fu_fp_stage_t *stage, *next_stage;
  int i;

  fu = fu_list;
  while (fu != NULL) {
    stage = fu->stage_list;
    next_stage = NULL;
    while(stage != NULL) {
      switch(stage->current_cycle) {
	/* is fu stage free? */
      case -1:                                      /* do nothing */
	break;

	/* is fu stage done processing? */
      case 0:
	if(next_stage == NULL) {                  /* is this the last stage in the fu? */
	  fp_wb->instr = stage->instr;
	  stage->current_cycle = -1;
	} else {
	  if(next_stage->current_cycle == -1) {                                  /* move to next fu stage */
	    next_stage->current_cycle = next_stage->num_cycles-1;
	    next_stage->instr = stage->instr;
	    stage->current_cycle = -1;
	  }
	}
	break;

	/* fu stage is still processing */
      default:   
	stage->current_cycle--;
      }
      next_stage = stage;
      stage = stage->prev;
    }
    fu = fu->next;
  }
}


int
fu_int_done(fu_int_t *fu_list)
{
  fu_int_t *fu;
  fu_int_stage_t *stage;

  fu = fu_list;
  while (fu != NULL) {
    stage = fu->stage_list;
    while (stage != NULL) {
      if (stage->current_cycle != -1)
	return FALSE;
      stage = stage->prev;
    }
    fu = fu->next;
  }

  return TRUE;
}


int
fu_fp_done(fu_fp_t *fu_list)
{
  fu_fp_t *fu;
  fu_fp_stage_t *stage;

  fu = fu_list;
  while (fu != NULL) {
    stage = fu->stage_list;
    while (stage != NULL) {
      if (stage->current_cycle != -1)
	return FALSE;
      stage = stage->prev;
    }
    fu = fu->next;
  }

  return TRUE;
}


/* decode an instruction */
const op_info_t *
decode_instr(int instr, int *use_imm) {
  const op_info_t *op_info;

  if (op_table[FIELD_OPCODE(instr)].level2_table == NULL) {
    /* Level 1 table */
    op_info = &op_table[FIELD_OPCODE(instr)].info;
  } else if (op_table[FIELD_OPCODE(instr)].level2_table[FIELD_FUNC3(instr)].level3_table == NULL) {
    /* Level 2 table */
    op_info = &op_table[FIELD_OPCODE(instr)].level2_table[FIELD_FUNC3(instr)].info;
  } else {
    /* Level 3 table */
    op_info = &op_table[FIELD_OPCODE(instr)].level2_table[FIELD_FUNC3(instr)].level3_table[FIELD_FUNC7(instr)].info;
  }

  if ((FIELD_OPCODE(instr) == 0) || (FIELD_OPCODE(instr) == 1))
    *use_imm = 0;
  else
    *use_imm = 1;

  return op_info;
}


/* perform an instruction */
void
perform_operation(int instr, unsigned long pc, operand_t operand1,
		  operand_t operand2)
{
  const op_info_t *op_info;
  int use_imm;
  operand_t result;

  op_info = decode_instr(instr, &use_imm);
  switch(op_info->fu_group_num) {
  case FU_GROUP_INT:
    switch(op_info->operation) {
    case OPERATION_ADD:
      result.integer.w = operand1.integer.w + operand2.integer.w;
      break;
    }
    break;

  case FU_GROUP_ADD:
  case FU_GROUP_MULT:
  case FU_GROUP_DIV:
    break;

  case FU_GROUP_MEM:
    break;

  case FU_GROUP_BRANCH:
    break;

  case FU_GROUP_HALT:
    break;

  case FU_GROUP_NONE:
    break;

  case FU_GROUP_INVALID:
    fprintf(stderr, "error (perform): invalid opcode (instr = %.8X)\n", instr);
  }

}
