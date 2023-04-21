/*****************************************************************************
 *
 *                       decode_inst.h
 *
 *      Assignment: Homework 6 -- um
 *      Authors: Kabir Pamnani (kpamna01), Oliver Uy (ouy01)
 *      Date: Apr 13th, 2023
 *     
 *      Summary: UM Decode header, contains declarations for functions used
 *               to decode UM format 32-bit word instructions into their
 *               individual parts.
 *
 ****************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include "bitpack.h"
#include "structs_and_constants.h"

inst_3reg_t decode_3reg_inst(uint32_t inst);
inst_loadval_t decode_loadval_inst(uint32_t inst);