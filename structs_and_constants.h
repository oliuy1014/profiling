/*****************************************************************************
 *
 *                 structs_and_constants.h
 *
 *      Assignment: Homework 6 -- um
 *      Authors: Kabir Pamnani (kpamna01), Oliver Uy (ouy01)
 *      Date: Apr 13th, 2023
 *     
 *      Summary: Contains structs and constants used by multiple um modules
 *
 *
 ****************************************************************************/
#ifndef STRUCTS_AND_CONSTS
#define STRUCTS_AND_CONSTS
#include <stdlib.h>
#include <stdint.h>
#include "seq.h"
#include "set.h"
#include "atom.h"

/**********struct definitions************************************************/

/* three register instruction struct
 *
 * Purpose: stores opcode and registers for a three_register instruction
 * Members:
 *      - uint32_t OP: opcode      
 *      - uint32_t A, B, C: indices of registers A, B, and C
 */
typedef struct three_reg_instruction {
        uint32_t OP, A, B, C;
} inst_3reg_t;

/* load value instruction struct
 *
 * Purpose: stores register and value for a load_value instruction
 * Members:
 *      - uint32_t A: idx of register to load value into
 *      - uint32_t val: value to load into r[A]
 */
typedef struct load_val_instruction {
        uint32_t A, val;
} inst_loadval_t;

typedef struct memory {
        Seq_T mem_seq;
        // Set_T mapped;
        Seq_T unmapped;
} *mem_struct;

/**********global macros ****************************************************/
#define OP_W 4
#define OP_LSB 28
#define REG_W 3
#define RA_LSB 6
#define RB_LSB 3
#define RC_LSB 0
#define LOADVAL_OP 13
#define LOADVAL_REG_LSB 25
#define LOADVAL_VAL_LSB 0
#define LOADVAL_VAL_W 25
#define WORD_SZ 32
#define BYTE_W 8
#define MAX_CHAR 255
#define NUM_REG 8
#endif