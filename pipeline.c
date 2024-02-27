
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

#define INT_CYCLES 2
#define ADD_CYCLES 3
#define MULT_CYCLES 4
#define DIV_CYCLES 8

void
writeback(state_t *state, int *num_insn) {
  wb_t int_wb = state->int_wb;
  wb_t fp_wb = state->fp_wb;
  unsigned int rd, rs1, rs2;
  int_t result; 

  int use_imm;

  const op_info_t *op_info = decode_instr(int_wb.instr, &use_imm);
  // printf("Number of instructions: %d\n", *num_insn);

  if (int_wb.instr != 0 && int_wb.instr != NOP) {
    int_t dest_addr;
    // load store value
    operand_t ls_value;
    float *store_float;
    int ls_loc; 
    (*num_insn)++;
    rd = FIELD_RD(int_wb.instr);
    rs1 = FIELD_RS1(int_wb.instr);
    rs2 = FIELD_RS2(int_wb.instr);
    result = int_wb.value.integer; 

    switch (op_info->fu_group_num) {
      case FU_GROUP_INT:
        // printf("INT WRITEBACK\n");
        if (rd != 0) {
          state->rf_int.reg_int[rd] = result;
        }
        
        state->scoreboard_int[rd] = -1;
        
        break;
      case FU_GROUP_MEM:
        // printf("MEM WRITEBACK\n");
        switch (op_info->operation) {
          case OPERATION_LOAD:
            ls_loc = result.w;
            // printf("Load from location: %d with value %d\n", result.w, state->mem[ls_loc] | (state->mem[ls_loc+1] << 8) |
                      // (state->mem[ls_loc+2] << 16) | (state->mem[ls_loc+3] << 24));
            switch (op_info->data_type) {
              case DATA_TYPE_W:
                ls_value.integer.w = state->mem[ls_loc] | (state->mem[ls_loc+1] << 8) |
                                    (state->mem[ls_loc+2] << 16) | (state->mem[ls_loc+3] << 24);
                if (rd != 0) {
                  state->rf_int.reg_int[rd] = ls_value.integer;
                }
                break;
              case DATA_TYPE_F:
                store_float = (float *) &(state->mem[ls_loc]);
                ls_value.flt = *store_float;
                state->rf_fp.reg_fp[rd] = ls_value.flt;
                break;
            }
            // printf("Reset scoreboard for rd: %d\n", rd);
            state->scoreboard_int[rd] = -1;
            break;
          case OPERATION_STORE:
            ls_loc = result.w;
            switch (op_info->data_type) {
              case DATA_TYPE_W:
                ls_value.integer = state->rf_int.reg_int[rs2];
                state->mem[ls_loc] = ls_value.integer.w & 0xFF;
                state->mem[ls_loc+1] = (ls_value.integer.w >> 8) & 0xFF;
                state->mem[ls_loc+2] = (ls_value.integer.w >> 16) & 0xFF;
                state->mem[ls_loc+3] = (ls_value.integer.w >> 24) & 0xFF;
                break;
              case DATA_TYPE_F:
                ls_value.flt = state->rf_fp.reg_fp[rs2];
                state->mem[ls_loc] = ls_value.integer.w & 0xFF;
                state->mem[ls_loc+1] = (ls_value.integer.w >> 8) & 0xFF;
                state->mem[ls_loc+2] = (ls_value.integer.w >> 16) & 0xFF;
                state->mem[ls_loc+3] = (ls_value.integer.w >> 24) & 0xFF;
                break;
            }

            break;
        }
        break;
      case FU_GROUP_BRANCH:
        switch (op_info->operation) {
          case OPERATION_JAL:
            state->pc = result.wu;
            dest_addr.wu = state->pc + 4;
            if (rd != 0) {
              state->rf_int.reg_int[rd] = dest_addr;
            }
            // clear fetched instruction
            state->if_id.instr = NOP;
            break;
          case OPERATION_JALR:
            state->pc = state->rf_int.reg_int[FIELD_RS1(int_wb.instr)].wu;
            dest_addr.wu = state->pc + 4;
            if (rd != 0) {
              state->rf_int.reg_int[rd] = dest_addr;
            }
            // clear fetched instruction
            state->if_id.instr = NOP;
            break;
          case OPERATION_BEQ:
            if (state->rf_int.reg_int[FIELD_RS1(int_wb.instr)].wu == state->rf_int.reg_int[FIELD_RS2(int_wb.instr)].wu) {
              state->pc = result.wu;
              // clear fetched instruction
              state->if_id.instr = NOP;
            }              
            break;
          case OPERATION_BNE:
            if (state->rf_int.reg_int[FIELD_RS1(int_wb.instr)].wu != state->rf_int.reg_int[FIELD_RS2(int_wb.instr)].wu) {
              state->pc = result.wu;
              // clear fetched instruction
              state->if_id.instr = NOP;
            }      
            break;
        }
        
        // Start fetching again after branch logic
        state->fetch_lock = FALSE;

        break;
      case FU_GROUP_HALT:
        break;
      case FU_GROUP_NONE: case FU_GROUP_INVALID: 
        fprintf(stderr, "error (perform): invalid opcode");
        break;
    }
    
    state->int_wb.instr = 0;
  } 

  if (fp_wb.instr != 0 && fp_wb.instr != NOP) {
    (*num_insn)++;

    rd = FIELD_RD(fp_wb.instr);
    state->rf_fp.reg_fp[rd] = fp_wb.value.flt;
    state->scoreboard_fp[rd] = -1;
    state->fp_wb.instr = 0;
  }
}


void
execute(state_t *state) {
  int i;
  advance_fu_fp(state->fu_add_list, &state->fp_wb);
  advance_fu_fp(state->fu_mult_list, &state->fp_wb);
  advance_fu_fp(state->fu_div_list, &state->fp_wb);
  advance_fu_int(state->fu_int_list, &state->int_wb);

  for (i=0; i < NUMREGS; i++) {
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
  int i;
  int use_imm;
  const op_info_t *op_info = decode_instr(state->if_id.instr, &use_imm);
  operand_t result, op1, op2;  
  unsigned int rd, rs1, rs2;
  int_t op1_value, op2_value;

  if (state->if_id.instr == NOP) {
    return 0;
  }


  switch (op_info->fu_group_num) {
    case FU_GROUP_INT:
      rd = FIELD_RD(state->if_id.instr);
      rs1 = FIELD_RS1(state->if_id.instr);
      rs2 = FIELD_RS2(state->if_id.instr);

      // If there is a RAW Hazard
      // printf("INT use_imm: %d\n", use_imm);
      // printf("INT rs2: %d\n", rs2);
      // printf("INT rs2 scoreboard: %d\n", state->scoreboard_int[rs2]);
      if (state->scoreboard_int[rs1] != -1 || (!use_imm && state->scoreboard_int[rs2] != -1)) {
        state->pc -= 4;
        return 0;
      }

      // Set op1 value
      op1.integer = state->rf_int.reg_int[rs1];
      // Set op2 value, either immediate or register value
      if (use_imm) {
        op2_value.w = FIELD_IMM_I(state->if_id.instr);
        // switch (op_info->operation) {
        //   case OPERATION_ADD: case OPERATION_SUB: case OPERATION_SLT:
        //     op2_value.w = (signed long) FIELD_IMM_I(state->if_id.instr);
        //     break;
        //   default:
        //     op2_value.wu = (unsigned long) FIELD_IMM_I(state->if_id.instr);
        //     break;
        // }
        op2.integer = op2_value;
      } else {
        op2.integer = state->rf_int.reg_int[rs2];
      }

      // Perform the operation
      result = perform_operation(state->if_id.instr, 
                                state->if_id.pc, 
                                op1, 
                                op2);

      // If there is a structural hazard
      if (issue_fu_int(state->fu_int_list, state->if_id.instr, result) == -1) {
        state->pc -= 4;
        return 0;
      }

      // Set scoreboard for destination register (can cause RAW hazard if not -1)
      state->scoreboard_int[rd] = INT_CYCLES;

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
        state->pc -= 4;
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
        state->pc -= 4;
        return 0;
      }

      // Check for writeback structural hazard
      for (i=0; i < NUMREGS; i++) {
        if (state->scoreboard_fp[i] == num_cycles) {
          state->pc -= 4;
          return 0;
        }
      }

      // Check for FU structural hazard

      // Get result
      op1.flt = state->rf_fp.reg_fp[rs1];
      op2.flt = state->rf_fp.reg_fp[rs2];
      result = perform_operation(state->if_id.instr, state->if_id.pc, op1, op2);

      // Carry value for fp writeback
      // state->fp_wb_value = result.flt;

      switch (op_info->fu_group_num) {
        case FU_GROUP_ADD:
          if (issue_fu_fp(state->fu_add_list, state->if_id.instr, result) == -1) {
            state->pc -= 4;
            return 0;
          }
          break;
        case FU_GROUP_MULT:
          if (issue_fu_fp(state->fu_mult_list, state->if_id.instr, result) == -1) {
            state->pc -= 4;
            return 0;
          }
          break;
        case FU_GROUP_DIV:
          if (issue_fu_fp(state->fu_div_list, state->if_id.instr, result) == -1) {
            state->pc -= 4;
            return 0;
          }
          break;
      }

      // Set scoreboard for destination register
      state->scoreboard_fp[rd] = num_cycles;

      break;
    case FU_GROUP_MEM:
      rd = FIELD_RD(state->if_id.instr);
      rs1 = FIELD_RS1(state->if_id.instr);
      rs2 = FIELD_RS2(state->if_id.instr);

      // If there is a RAW Hazard RS1, applies to all memory instructions
      if (state->scoreboard_int[rs1] != -1) {
        state->pc -= 4;
        return 0;
      }

      // Dealing with hazards
      switch (op_info->operation) {
        case OPERATION_STORE:
          switch (op_info->data_type) {
          case DATA_TYPE_W:
            // RAW Hazard RS2 (STORE ONLY)
            if (state->scoreboard_int[rs2] != -1) {
              state->pc -= 4;
              return 0;
            }
            break;
          case DATA_TYPE_F:
            // RAW Hazard RS2 (STORE ONLY)
            if (state->scoreboard_fp[rs2] != -1) {
              state->pc -= 4;
              return 0;
            }
            break;
        }
        // end switch
          break;
        case OPERATION_LOAD:
          if (op_info->data_type == DATA_TYPE_F) {
            int num_cycles = INT_CYCLES;
            int rem_cycles = state->scoreboard_fp[rd];
            // WAW Hazard
            if (num_cycles <= rem_cycles) {
              state->pc -= 4;
              return 0;
            }

            // Writeback structural hazard
            for (i=0; i < NUMREGS; i++) {
              if (state->scoreboard_fp[i] == num_cycles) {
                state->pc -= 4;
                return 0;
              }
            }
          }

          break;
      }


      // Set op1 value
      op1.integer = state->rf_int.reg_int[rs1];
      // Set op2 value, immediate
      switch (op_info->operation) {
        case OPERATION_LOAD:
          op2_value.w = FIELD_IMM_I(state->if_id.instr);
          break;
        case OPERATION_STORE:
          op2_value.w = FIELD_IMM_S(state->if_id.instr);
          break;
      }
      op2.integer = op2_value;

      // Perform the operation
      result = perform_operation(state->if_id.instr, 
                                state->if_id.pc, 
                                op1, 
                                op2);


      // If there is a structural hazard
      if (issue_fu_int(state->fu_int_list, state->if_id.instr, result) == -1) {
        state->pc -= 4;
        return 0;
      }

      // Set scoreboard for destination register (can cause RAW hazard if not -1)
      if (op_info->operation == OPERATION_LOAD) {
        // printf("HERE with rd value: %d\n", rd);
        switch (op_info->data_type) {
          case DATA_TYPE_W:
            state->scoreboard_int[rd] = INT_CYCLES;
            break;
          case DATA_TYPE_F:
            state->scoreboard_fp[rd] = INT_CYCLES;
            break;
        }
      } 

      break;
    case FU_GROUP_BRANCH:
      rd = FIELD_RD(state->if_id.instr);
      rs1 = FIELD_RS1(state->if_id.instr);
      rs2 = FIELD_RS2(state->if_id.instr);
      // Check for RAW hazard (JALR, BEQ, BNE)
      switch (op_info->operation) {
        case OPERATION_JAL:
          break;
        case OPERATION_JALR:
          if (state->scoreboard_int[rs1] != -1) {
            state->pc -= 4;
            return 0;
          }
          break;
        case OPERATION_BEQ: case OPERATION_BNE:
          // Check for RAW hazard
          if (state->scoreboard_int[rs1] != -1 || 
              state->scoreboard_int[rs2] != -1) {
            state->pc -= 4;
            return 0;
          }
          break;
      }

      // Perform the operation for branch instructions
      switch (op_info->operation) {
        case OPERATION_JAL:
          op1_value.w = state->if_id.pc;
          op1.integer = op1_value;

          op2_value.w = FIELD_OFFSET(state->if_id.instr);
          op2.integer = op2_value;
          
          result = perform_operation(state->if_id.instr, state->if_id.pc, op1, op2);
          break;
        case OPERATION_JALR:
          break;
        case OPERATION_BEQ: case OPERATION_BNE:
          op1_value.w = state->if_id.pc;
          op1.integer = op1_value;

          // not sure if signed
          op2_value.w = FIELD_IMM_S(state->if_id.instr);
          op2.integer = op2_value;

          result = perform_operation(state->if_id.instr, state->if_id.pc, op1, op2);
          // printf("op1: %d, op2: %d\n", op1.integer.w, op2.integer.w);
          // printf("result: %d\n", result.integer.w);
          break;
      }
      // Check for FU structural hazard
      if (issue_fu_int(state->fu_int_list, state->if_id.instr, result) == -1) {
        state->pc -= 4;
        return 0;
      }

      state->fetch_lock = TRUE;

      break;
    case FU_GROUP_HALT:
      // printf("FU INT DONE: %d\n", fu_int_done(state->fu_int_list));
      state->fetch_lock = TRUE;
      state->halt = TRUE;
      break;
    case FU_GROUP_NONE: case FU_GROUP_INVALID:
      return -1;
  }

  
  return 0;
}


void
fetch(state_t *state) {
  // int i = 0;
  // printf("Scoreboard:\n");
  // for (i = 0; i < NUMREGS; i++) {
  //   printf("%d, ", state->scoreboard_int[i]);
  // }

  state->if_id.instr = state->mem[state->pc] | (state->mem[state->pc+1] << 8) |
    (state->mem[state->pc+2] << 16) | (state->mem[state->pc+3] << 24);
  state->if_id.pc = state->pc;
  state->pc += 4;

  
}

