
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
  operand_t result;  

  switch (op_info->fu_group_num) {
    case FU_GROUP_INT:
      unsigned int rd = FIELD_RD(state->if_id.instr);
      unsigned int rs1 = FIELD_RS1(state->if_id.instr);
      unsigned int rs2 = FIELD_RS2(state->if_id.instr);

      operand_t op1, op2;
      int_t op2_value;

      // If there is RAW Hazard
      if (state->scoreboard_int[rs1] != -1 || (use_imm && state->scoreboard_int[rs2] != -1)) {
        state->fetch_lock = TRUE;
        return 0;
      }
      // If there is a structural hazard
      if (issue_fu_int(state->fu_int_list, state->if_id.instr, state->if_id.pc) == -1) {
        state->fetch_lock = TRUE;
        return 0;
      }

      // Set scoreboard for destination register (can cause RAW hazard if not -1)
      state->scoreboard_int[rd] = 1;

      // Set op1 value
      op1.integer = state->rf_int.reg_int[rs1];
      // Set op2 value, either immediate or register value
      if (use_imm) {
        switch (op_info->operation) {
          case OPERATION_ADD: case OPERATION_SUB: case OPERATION_SLT:
            op2_value.w = (int) rs2;
            break;
          default:
            op2_value.wu = rs2;
            break;
        }
        op2.integer = op2_value;
      } else {
        op2.integer = state->rf_int.reg_int[rs2];
      }

      // Perform the operation
      result = perform_operation(state->if_id.instr, 
                                state->if_id.pc, 
                                op1, 
                                op2);

      // Carry value for int writeback 
      state->int_wb_value = result.integer;

      break;
  }

  
  return 0;
}


void
fetch(state_t *state) {
  state->if_id.instr = state->mem[state->pc] | (state->mem[state->pc+1] << 8) |
    (state->mem[state->pc+2] << 16) | (state->mem[state->pc+3] << 24);
  state->if_id.pc = state->pc;
  state->pc += 4;
}

