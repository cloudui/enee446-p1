
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

#define ADD_CYCLES 3
#define MULT_CYCLES 4
#define DIV_CYCLES 8

void
writeback(state_t *state, int *num_insn) {
  wb_t int_wb = state->int_wb;
  wb_t fp_wb = state->fp_wb;
  unsigned int rd;


  if (int_wb.instr != -1) {
    rd = FIELD_RD(int_wb.instr);
    state->rf_int.reg_int[rd] = state->int_wb_value;
    state->scoreboard_int[rd] = -1;
    int_wb.instr = -1;
  } 

  if (fp_wb.instr != -1) {
    rd = FIELD_RD(fp_wb.instr);
    state->rf_fp.reg_fp[rd] = state->fp_wb_value;
    state->scoreboard_fp[rd] = -1;
    fp_wb.instr = -1;
  }
}


void
execute(state_t *state) {
  advance_fu_fp(state->fu_add_list, &state->fp_wb);
  advance_fu_fp(state->fu_mult_list, &state->fp_wb);
  advance_fu_fp(state->fu_div_list, &state->fp_wb);
  advance_fu_int(state->fu_int_list, &state->int_wb);

  for (int i = 0; i < NUMREGS; i++) {
    if (state->scoreboard_int[i] != -1) {
      state->scoreboard_int[i]--;
    }
    if (state->scoreboard_fp[i] != -1) {
      state->scoreboard_fp[i]--;
    }
  }
}


int
decode(state_t *state) {
  int use_imm;
  const op_info_t *op_info = decode_instr(state->if_id.instr, &use_imm);
  operand_t result, op1, op2;  
  unsigned int rd, rs1, rs2;
  int_t op2_value;


  switch (op_info->fu_group_num) {
    case FU_GROUP_INT:
      rd = FIELD_RD(state->if_id.instr);
      rs1 = FIELD_RS1(state->if_id.instr);
      rs2 = FIELD_RS2(state->if_id.instr);

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

      state->fetch_lock = FALSE;

      // Set scoreboard for destination register (can cause RAW hazard if not -1)
      state->scoreboard_int[rd] = 1;

      // Set op1 value
      op1.integer = state->rf_int.reg_int[rs1];
      // Set op2 value, either immediate or register value
      if (use_imm) {
        switch (op_info->operation) {
          case OPERATION_ADD: case OPERATION_SUB: case OPERATION_SLT:
            op2_value.w = (signed long) rs2;
            break;
          default:
            op2_value.wu = (unsigned long) rs2;
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
    case FU_GROUP_ADD: case FU_GROUP_MULT: case FU_GROUP_DIV:
      rd = FIELD_RD(state->if_id.instr);
      rs1 = FIELD_RS1(state->if_id.instr);
      rs2 = FIELD_RS2(state->if_id.instr);

      int num_cycles;
      int rem_cycles;

      operand_t op1, op2;

      // Check for RAW hazard
      if (state->scoreboard_fp[rs1] != -1 || state->scoreboard_fp[rs2] != -1) {
        state->fetch_lock = TRUE;
        return 0;
      }

      // Check for WAW hazard
      switch (op_info->fu_group_num) {
        case FU_GROUP_ADD:
          num_cycles = ADD_CYCLES;
          break;
        case FU_GROUP_MULT:
          num_cycles = MULT_CYCLES;
          break;
        case FU_GROUP_DIV:
          num_cycles = DIV_CYCLES;
          break;
      }
      rem_cycles = state->scoreboard_fp[rd];
      if (num_cycles <= rem_cycles) {
        state->fetch_lock = TRUE;
        return 0;
      }

      // Check for writeback structural hazard
      for (int i = 0; i < NUMREGS; i++) {
        if (state->scoreboard_fp[i] == num_cycles) {
          state->fetch_lock = TRUE;
          return 0;
        }
      }

      // Check for FU structural hazard
      if (issue_fu_fp(state->fu_add_list, state->if_id.instr) == -1) {
        state->fetch_lock = TRUE;
        return 0;
      }

      // Set fetch lock to false
      state->fetch_lock = FALSE;

      // Set scoreboard for destination register
      state->scoreboard_fp[rd] = num_cycles;

      // Get result
      op1.flt = state->rf_fp.reg_fp[rs1];
      op2.flt = state->rf_fp.reg_fp[rs2];
      result = perform_operation(state->if_id.instr, state->if_id.pc, op1, op2);

      // Carry value for fp writeback
      state->fp_wb_value = result.flt;

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

