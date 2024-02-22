
/*
 * 
 * pipeline.c
 * 
   This is the primary place for student's code.

 */


#include <stdlib.h>
#include <string.h>
#include "fu.h"
#include "pipeline.h"

void
writeback(state_t *state, int *num_insn) {
  wb_t int_wb = state->int_wb;
  wb_t fp_wb = state->fp_wb;

  unsigned int rd = FIELD_RD(int_wb.instr);

  int_t x;
  x.w = 0;

  if (int_wb.instr != -1) {
    state->rf_int.reg_int[rd] = x;
    int_wb.instr = -1;
  } 
}


void
execute(state_t *state) {
  advance_fu_fp(state->fu_add_list, &state->fp_wb);
  advance_fu_fp(state->fu_mult_list, &state->fp_wb);
  advance_fu_fp(state->fu_div_list, &state->fp_wb);
  advance_fu_int(state->fu_int_list, &state->int_wb);
}


int
decode(state_t *state) {
  int use_imm;
  op_info_t *op_info = decode_instr(state->if_id.instr, &use_imm);

  
  
  return 0;
}


void
fetch(state_t *state) {
  state->if_id.instr = state->mem[state->pc] | (state->mem[state->pc+1] << 8) |
    (state->mem[state->pc+2] << 16) | (state->mem[state->pc+3] << 24);
  state->if_id.pc = state->pc;
  state->pc += 4;
}

