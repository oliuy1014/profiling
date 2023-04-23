/*****************************************************************************
 *
 *                       execute_inst.h
 *
 *      Assignment: Homework 6 -- um
 *      Authors: Kabir Pamnani (kpamna01), Oliver Uy (ouy01)
 *      Date: Apr 13th, 2023
 *     
 *      Summary: UM Execute header, contains declarations for functions used
 *               to execute UM instructions given their Opcodes, register
 *               indices, and/or values to load.
 *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "uarray.h"
#include "structs_and_constants.h"

void execute(uint32_t A, uint32_t B, uint32_t C, uint32_t OP,
             uint32_t **mem_seq, uint32_t *unmapped, uint32_t registers[],
             uint32_t *prog_counter);