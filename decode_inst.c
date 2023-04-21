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
        inst_3reg_t decoded;
        decoded.OP = inst >> 28;
        uint32_t abc = (inst << 23) >> 23;
        uint32_t A = abc >> 6;
        uint32_t lose_A = (A << 6);
        uint32_t B = (abc - lose_A) >> 3;
        uint32_t lose_B = B << 3;
        uint32_t C = abc - lose_A - lose_B;
        decoded.A = A;
        decoded.B = B;
        decoded.C = C;

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
        // fprintf(stderr, "inside decode lv\n");
        inst_loadval_t decoded;
        decoded.A = (inst << 4) >> 29;
        decoded.val = (inst << 7) >> 7;
        return decoded;
}