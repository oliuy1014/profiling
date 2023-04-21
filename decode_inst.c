/*****************************************************************************
 *
 *                       decode_inst.c
 *
 *      Assignment: Homework 6 -- um
 *      Authors: Kabir Pamnani (kpamna01), Oliver Uy (ouy01)
 *      Date: Apr 13th, 2023
 *     
 *      Summary: UM Decode module, contains definitions for functions used
 *               to decode UM format 32-bit word instructions into their
 *               individual parts. 
 *
 ****************************************************************************/
#include <stdio.h>
#include <assert.h>
#include "bitpack.h"
#include "decode_inst.h"
#include "structs_and_constants.h"

/**********decode_3reg********************************************************
 *
 * Purpose:
 *      decodes an instruction of the three_register format expected in a .um
 *      file, returns an instr_t with the decoded instruction
 * Parameters: 
 *      uint32_t inst: 32-bit word to be decoded
 * Returns: 
 *      instr_3reg_t containing the decoded instruction
 * Expects:
 *      None
 * Notes:
 *      None
 ****************************************************************************/
inst_3reg_t decode_3reg_inst(uint32_t inst)
{
        assert(!is_loadval(inst));
        inst_3reg_t decoded;
        decoded.OP = Bitpack_getu(inst, OP_W, OP_LSB);
        decoded.A = Bitpack_getu(inst, REG_W, RA_LSB);
        decoded.B = Bitpack_getu(inst, REG_W, RB_LSB);
        decoded.C = Bitpack_getu(inst, REG_W, RC_LSB);
        return decoded;
}

/**********decode_loadval*****************************************************
 *
 * Purpose:
 *      decodes an instruction of the load_value format expected in a .um
 *      file, returns an instr_lv_t with the decoded instruction
 * Parameters: 
 *      uint32_t inst: 32-bit word to be decoded
 * Returns: 
 *      instr_t containing the decoded instruction
 * Expects:
 *      None
 * Notes:
 *      None
 ****************************************************************************/
inst_loadval_t decode_loadval_inst(uint32_t inst)
{
        assert(is_loadval(inst));
        inst_loadval_t decoded;
        decoded.A = Bitpack_getu(inst, REG_W, LOADVAL_REG_LSB);
        decoded.val = Bitpack_getu(inst, LOADVAL_VAL_W, LOADVAL_VAL_LSB);
        return decoded;
}

/**********is_loadval*********************************************************
 *
 * Purpose:
 *      checks if an instruction is a loadval instruction
 * Parameters: 
 *      uint32_t inst: 32-bit word to be checked
 * Returns:
 *      boolean indicating whether or not the instruction is a loadval
 * Expects:
 *      None
 * Notes:
 *      None
 ****************************************************************************/
bool is_loadval(uint32_t inst)
{
        return (Bitpack_getu(inst, OP_W, OP_LSB) == LOADVAL_OP);
}