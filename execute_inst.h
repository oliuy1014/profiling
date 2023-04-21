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

void Cmov(uint32_t rA, uint32_t rB, uint32_t rC, uint32_t registers[]);
void Seg_load(uint32_t rA, uint32_t rB, uint32_t rC, mem_struct mem,
              uint32_t registers[]);
void Seg_store(uint32_t rA, uint32_t rB, uint32_t rC, mem_struct mem,
              uint32_t registers[]);
void Add(uint32_t rA, uint32_t rB, uint32_t rC, uint32_t registers[]);
void Mult(uint32_t rA, uint32_t rB, uint32_t rC, uint32_t registers[]);
void Div(uint32_t A, uint32_t B, uint32_t C, uint32_t r[]);
void Nand(uint32_t rA, uint32_t rB, uint32_t rC, uint32_t registers[]);
void Halt(mem_struct mem);
void Map(uint32_t rB, uint32_t rC, mem_struct mem, uint32_t registers[]);
void Unmap(uint32_t C, mem_struct mem, uint32_t r[]);
void Output(uint32_t rC, uint32_t registers[]);
void Input(uint32_t rC, uint32_t registers[]);
void Load_prog(uint32_t B, uint32_t C, mem_struct mem, uint32_t r[],
               uint32_t *prog_counter);
void Load_val(uint32_t reg, uint32_t val, uint32_t registers[]);
void execute(uint32_t A, uint32_t B, uint32_t C, uint32_t OP,
             mem_struct memory, uint32_t registers[],
             uint32_t *prog_counter);