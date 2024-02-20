
/*
 * 
 * output.c
 * 
   This is the output module, adapted for the RISC-V ISA.

 * Donald Yeung
 */


#include <stdio.h>
#include "fu.h"
#include "pipeline.h"
#include "output.h"



/************************************************************/
void
print_state(state_t *state,int num_memory) {
  int i, index;

  printf("Memory\n");
  printf("\tAddress\t\tData");
  for (i = 0; i < num_memory; i++) {
    if((i & 0x0000000F) == 0) {
      printf("\n\t0x%.8X\t%.2X",i,(unsigned int)state->mem[i]);
    } else {
      printf(" %.2X",(unsigned int)state->mem[i]);
    }
  }
  printf("\n");

  printf("Registers (integer):\n");
  for (i = 0; i < NUMREGS; i += 4)
    printf("\tx%d=0x%.8X\tx%d=0x%.8X\tx%d=0x%.8X\tx%d=0x%.8X\n",
	   i, state->rf_int.reg_int[i].wu,
	   i+1, state->rf_int.reg_int[i+1].wu,
	   i+2, state->rf_int.reg_int[i+2].wu,
	   i+3, state->rf_int.reg_int[i+3].wu);

  printf("Registers (floating point):\n");
  for (i = 0; i < NUMREGS; i += 4)
    printf("\tf%d=%-10.6g\tf%d=%-10.6g\tf%d=%-10.6g\tf%d=%-10.6g\n", 
	   i, state->rf_fp.reg_fp[i],
	   i+1, state->rf_fp.reg_fp[i+1],
	   i+2, state->rf_fp.reg_fp[i+2],
	   i+3, state->rf_fp.reg_fp[i+3]);

  printf("pc:\n");
  printf("\tpc\t0x%.8X\n",state->pc);

  printf("ifid:\n");
  printf("\tinstr\t");
  printInstruction(state->if_id.instr);
  printf("\n");

  printf("%s fu:\n",fu_group_int_name);
  print_fu_group_int(state->fu_int_list);
  printf("%s fu:\n",fu_group_add_name);
  print_fu_group_fp(state->fu_add_list);
  printf("%s fu:\n",fu_group_mult_name);
  print_fu_group_fp(state->fu_mult_list);
  printf("%s fu:\n",fu_group_div_name);
  print_fu_group_fp(state->fu_div_list);

  printf("wb:\n");
  if (state->int_wb.instr) {
    printf("\t\t");
    printInstruction(state->int_wb.instr);
  }
  if (state->fp_wb.instr) {
    printf("\t\t");
    printInstruction(state->fp_wb.instr);
  }

  printf("\n");
}
/************************************************************/


void
print_fu_group_int(fu_int_t *fu_int_list) {
  fu_int_t *fu_int;
  fu_int_stage_t *stage;
  int j;

  fu_int = fu_int_list;
  while (fu_int != NULL) {
    j = 0;
    stage = fu_int->stage_list;
    while (stage != NULL) {
      if (stage->current_cycle != -1) {
	printf("\t\tname '%s', stage %d (from end), cycle %d/%d, ",
	       fu_int->name, j, stage->current_cycle, stage->num_cycles);
	printInstruction(stage->instr);
	printf("\n");
      }
      j++;
      stage = stage->prev;
    }
    fu_int = fu_int->next;
  }
}

void
print_fu_group_fp(fu_fp_t *fu_fp_list) {
  fu_fp_t *fu_fp;
  fu_fp_stage_t *stage;
  int j;

  fu_fp = fu_fp_list;
  while(fu_fp != NULL) {
    j = 0;
    stage = fu_fp->stage_list;
    while(stage != NULL) {
      if(stage->current_cycle != -1) {
	printf("\t\tname '%s', stage %d (from end), cycle %d/%d, ",
	       fu_fp->name, j, stage->current_cycle, stage->num_cycles);
	printInstruction(stage->instr);
	printf("\n");
      }
      j++;
      stage = stage->prev;
    }
    fu_fp = fu_fp->next;
  }
}


/************************************************************/
void
printInstruction(int instr) {
  const op_info_t *op_info;

  if (instr == NOP) {
    printf("NOP");
    return;
  }

  if (op_table[FIELD_OPCODE(instr)].level2_table == NULL) {
    op_info = &op_table[FIELD_OPCODE(instr)].info;
    if(op_info->name == NULL)
      printf("0x%.8X",instr);
    else {
      switch(op_info->fu_group_num) {
      case FU_GROUP_BRANCH:
	switch(op_info->operation) {
	case OPERATION_JAL:
	  printf("%s x%d #%d",op_info->name,FIELD_RD(instr),FIELD_OFFSET(instr));
	  break;
	}
	break;
      default:
	printf("%s",op_info->name);
      }
    }
  } else if (op_table[FIELD_OPCODE(instr)].level2_table[FIELD_FUNC3(instr)].level3_table == NULL) {
    op_info = &op_table[FIELD_OPCODE(instr)].level2_table[FIELD_FUNC3(instr)].info;
    if(op_info->name == NULL)
      printf("0x%.8X",instr);
    else {
      switch(op_info->fu_group_num) {
      case FU_GROUP_INT:
	if (FIELD_OPCODE(instr) == 0)
	  printf("%s x%d x%d x%d",op_info->name,FIELD_RD(instr),FIELD_RS1(instr),FIELD_RS2(instr));
	else
	  printf("%s x%d x%d #%d",op_info->name,FIELD_RD(instr),FIELD_RS1(instr),FIELD_IMM_I(instr));
	break;
      case FU_GROUP_MEM:
	switch(op_info->data_type) {
	case DATA_TYPE_W:
	  if (op_info->operation == OPERATION_LOAD)
	    printf("%s x%d (%d)x%d",op_info->name,FIELD_RD(instr),FIELD_IMM_I(instr),FIELD_RS1(instr));
	  else
	    printf("%s x%d (%d)x%d",op_info->name,FIELD_RS2(instr),FIELD_IMM_S(instr),FIELD_RS1(instr));
	  break;
	case DATA_TYPE_F:
	  if (op_info->operation == OPERATION_LOAD)
	    printf("%s f%d (%d)x%d",op_info->name,FIELD_RD(instr),FIELD_IMM_I(instr),FIELD_RS1(instr));
	  else
	    printf("%s f%d (%d)x%d",op_info->name,FIELD_RS2(instr),FIELD_IMM_S(instr),FIELD_RS1(instr));
	  break;
	}
	break;
      case FU_GROUP_BRANCH:
	switch(op_info->operation) {
	case OPERATION_JALR:
	  printf("%s x%d x%d",op_info->name,FIELD_RD(instr),FIELD_RS1(instr));
	  break;
	case OPERATION_BEQ:
	case OPERATION_BNE:
	  printf("%s x%d x%d #%d",op_info->name,FIELD_RS1(instr),FIELD_RS2(instr),FIELD_IMM_S(instr));
	  break;
	}
	break;
      default:
	printf("%s",op_info->name);
      }
    }
  } else {
    op_info = &op_table[FIELD_OPCODE(instr)].level2_table[FIELD_FUNC3(instr)].level3_table[FIELD_FUNC7(instr)].info;
    if(op_info->name == NULL)
      printf("0x%.8X",instr);
    else {
      switch(op_info->fu_group_num) {
      case FU_GROUP_INT:
	printf("%s x%d x%d x%d",op_info->name,FIELD_RD(instr),FIELD_RS1(instr),FIELD_RS2(instr));
	break;
      case FU_GROUP_ADD:
      case FU_GROUP_MULT:
      case FU_GROUP_DIV:
	printf("%s f%d f%d f%d",op_info->name,FIELD_RD(instr),FIELD_RS1(instr),FIELD_RS2(instr));
	break;
      default:
	printf("%s",op_info->name);
      }
    }
  }
}
/************************************************************/
