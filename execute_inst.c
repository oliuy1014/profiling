/*****************************************************************************
 *
 *                       execute_inst.c
 *
 *      Assignment: Homework 6 -- um
 *      Authors: Kabir Pamnani (kpamna01), Oliver Uy (ouy01)
 *      Date: Apr 13th, 2023
 *     
 *      Summary: UM Execute module, contains definitions for functions used
 *               to execute UM instructions given their Opcodes, register
 *               indices, and/or values to load.
 *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "execute_inst.h"
#include "structs_and_constants.h"
// #include <assert.h>
#include <string.h>

/**********execute********************************************************
 *
 * Purpose:
 *      Executes an instruction of the three register format for given Opcode
 *      and registers. Executes an instruction with 0 <= Opcode <= 12.
 * Par[a]meters:
 *      uint32_t A: idx of register A
 *      uint32_t B: idx of register B
 *      uint32_t C: idx of register C
 *      mem_struct memory: virtual memory struct pointer
 *      uint32_t r[]: arr[a]y of registers
 *      uint32_t *prog_counter: pointer to progr[a]m counter
 * Returns:
 *      None
 * Expects:
 *      - A, B, and C to be within the r[a]nge 0-7
 *      - mem to be non-NULL
 * Notes:
 *      For any non-Halt instruction, may change values contained within the
 *      virtual memory struct, registers arr[a]y, and/or the progr[a]m counter.
 ****************************************************************************/
void execute(uint32_t A, uint32_t B, uint32_t C, uint32_t OP,
             uint32_t **mem_seq, uint32_t *unmapped, uint32_t r[], uint32_t *prog_counter)
{
        switch (OP) {
                case 0: 
                {
                        if (r[C] != 0) {
                                r[A] = r[B];
                        }
                        break;
                }
                case 1:
                {
                        if (r[B] == 0) {
                                uint32_t *seg = (mem_seq)[1];
                                r[A] = seg[r[C] + 1];
                                return;
                        }
                        uint32_t *seg = (mem_seq)[r[B]];
                        r[A] = seg[r[C] + 1];
                        break;
                }
                case 2:
                {
                        uint32_t *segA = mem_seq[r[A]];
                        if (r[A] == 0) {
                                uint32_t *seg = (mem_seq)[1];
                                seg[r[B] + 1] = r[C];
                                return;
                        }
                        segA[r[B] + 1] = r[C];
                        break;
                }
                case 3:
                {
                        r[A] = (r[B] + r[C]);
                        break;
                }
                case 4:
                {
                        r[A] = (r[B] * r[C]);
                        break;
                }
                case 5:
                {
                        r[A] = (r[B] / r[C]);
                        break;
                }
                case 6:
                {
                        r[A] = ~(r[B] & r[C]);
                        break;
                }
                case 7:
                {
                        uint32_t num_segs = (mem_seq[0][1]);
                        for (long seg = 0; seg < num_segs; seg++) {
                                uint32_t *curr_seg = (mem_seq)[seg];
                                if (curr_seg != NULL) {
                                        free(curr_seg);
                                }
                        }
                        free(mem_seq);
                        free(unmapped);
                        exit(0);
                        break;
                }
                case 8:
                {
                        uint32_t *new_seg = calloc(((size_t) r[C]) + 1, sizeof(uint32_t));
                        new_seg[0] = r[C] + 1;
                        uint32_t unmapped_size = unmapped[0];
                        if (unmapped_size != 0) {
                                uint32_t unmapped_id = unmapped[unmapped_size];
                                (mem_seq)[unmapped_id] = new_seg;
                                r[B] = unmapped_id;
                                unmapped[0]--;
                        } else {
                                uint32_t *meta = mem_seq[0];
                                uint32_t seg_id = meta[1];
                                (mem_seq)[seg_id] = new_seg;
                                r[B] = seg_id;
                                meta[1] = seg_id + 1;
                        }
                        break;
                }
                case 9:
                {
                        /* free segment being unmapped */
                        uint32_t *seg = (mem_seq)[r[C]];
                        free(seg);

                        /* set pointer in mem_struct to NULL and add id to unmapped sequence */
                        mem_seq[r[C]] = NULL;
                        unmapped[unmapped[0] + 1] = r[C];
                        unmapped[0]++;
                        break;
                }
                case 10:
                {
                        if (r[C] == (uint32_t) ~0) {
                                return;
                        }
                        /* otherwise, print char of value at r[C] */
                        putc(r[C], stdout);
                        break;
                }
                case 11:
                {
                        int val = getc(stdin);

                        /* if input reaches EOF, fill r[C] with all 1's, otherwise just
                        * put int value of input char in r[C]
                        */
                        if (val != EOF) {
                                r[C] = (uint32_t) val;
                        } else {
                                r[C] = (uint32_t) ~0;
                        }
                        break;
                }
                case 12:
                {
                        *prog_counter = r[C];
        
                        /* if loading progr[a]m from mem[0] just return */
                        if (r[B] == 0) {
                                return;
                        }
                        
                        /* otherwise, copy segment from r[B] into mem[0] and free the 
                        * previous contents of m[0]
                        */
                        uint32_t *old_prog = (mem_seq)[1];
                        uint32_t *prog_seg = (mem_seq)[r[B]];
                        size_t prog_bytes = (prog_seg[0] + 1) * 4;
                        uint32_t *prog_copy = malloc(prog_bytes);
                        memcpy(prog_copy, prog_seg, prog_bytes);
                        free(old_prog);
                        mem_seq[1] = prog_copy;
                        break;
                }
                default:
                        {break;}
        }
}