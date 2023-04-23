/*****************************************************************************
 *
 *                       um.c
 *
 *      Assignment: Homework 6 -- um
 *      Authors: Kabir Pamnani (kpamna01), Oliver Uy (ouy01)
 *      Date: Apr 13th, 2023
 *     
 *      Summary: Virtual machine — emulates CPU processes with virtual memory
 *               and registers for the provided UM language. 
 * 
 *               Executes any of the following UM lang instructions:
 *               Load_value, Add, Multiply, Divide, NAND, Map_segment,
 *               Unmap_segment, Segment_store, Segment_load, Halt,
 *               Load_program, Input, Output.
 *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "execute_inst.h"
#include "structs_and_constants.h"
#include "uarray.h"
#include "sys/stat.h"

/******************************global macros*********************************/
#define NUM_REG 8
#define INITIAL_MEM_SIZE 1
#define INITIAL_MAPPED_SIZE 0
#define INITIAL_UNMAPPED_SIZE 0
#define BYTES_PER_WORD 4
#define SPINE_SIZE 4294967295

/**************************function declarations******************************/
int main(int argc, char*argv[])
{
        if (argc != 2) {
                printf("Usage: ./um filename.um\n");
                exit(1);
        }

        /* open input file and get number of 32-bit words */
        FILE *um_fp = fopen(argv[1], "r"); 
        if (um_fp == NULL) {
                fprintf(stderr, "Could not open file %s\n", argv[1]);
                exit(1);
        }
        struct stat file_stats;
        assert(stat(argv[1], &file_stats) == 0);
        long num_words = (file_stats.st_size / BYTES_PER_WORD);

        /* initialize registers and program counter */
        uint32_t r[NUM_REG] = {0};
        unsigned *prog_counter = malloc(sizeof(*prog_counter));
        *prog_counter = 0;
        
        /* initialize virtual memory mem_struct */
        uint32_t **mem_seq = malloc(SPINE_SIZE);
        uint32_t *metadata = malloc(2 * sizeof(uint32_t));
        metadata[0] = SPINE_SIZE;
        metadata[1] = 2;
        mem_seq[0] = metadata;

        uint32_t *unmapped = malloc(SPINE_SIZE);
        unmapped[0] = 0;
        int curr_byte = fgetc(um_fp);
        uint32_t *m_0 = malloc((num_words + 1) * sizeof(uint32_t));
        m_0[0] = num_words;
        int word_idx = 0;
        while (curr_byte != EOF) {
                uint32_t curr_word = 0;
                for (int i = 0; i < BYTES_PER_WORD; i++) {
                        curr_word = curr_word | (curr_byte << (24 - 8 * i));
                        curr_byte = fgetc(um_fp);
                }
                m_0[word_idx + 1] = curr_word;
                word_idx++;
        }
        mem_seq[1] = m_0;
        fclose(um_fp);

        while (*prog_counter < ((mem_seq)[0][0])) {
                uint32_t *seg_0 = (mem_seq)[1];
                uint32_t inst = seg_0[*prog_counter + 1];
                if (inst >> 28 == 13) {
                        uint32_t reg_idx = (inst << 4) >> 29;
                        uint32_t val = (inst << 7) >> 7;
                        r[reg_idx] = val;
                        (*prog_counter)++;
                } else {
                        uint32_t OP = inst >> 28;
                        uint32_t abc = (inst << 23) >> 23;
                        uint32_t A = abc >> 6;
                        uint32_t lose_A = (A << 6);
                        uint32_t B = (abc - lose_A) >> 3;
                        uint32_t lose_B = B << 3;
                        uint32_t C = abc - lose_A - lose_B;
                        if (OP != 13) {
                                (*prog_counter)++;
                        }                             
                        if (OP == 7) {
                                free(prog_counter);
                        }
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
                                                break;
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
                                                break;
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
                                                break;
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
                                                break;
                                        }
                                        
                                        /* otherwise, copy segment from r[B] into mem[0] and free the 
                                        * previous contents of m[0]
                                        */
                                        uint32_t *old_prog = (mem_seq)[1];
                                        uint32_t *prog_seg = (mem_seq)[r[B]];
                                        size_t prog_bytes = (prog_seg[0] + 1) * 4;
                                        uint32_t *prog_copy = malloc(prog_bytes);
                                        unsigned l = prog_seg[0];
                                        for (unsigned i = 0; i < l; i++) {
                                                prog_copy[i] = prog_seg[i];
                                        }
                                        free(old_prog);
                                        mem_seq[1] = prog_copy;
                                        break;
                                }
                                default:
                                        {break;}
                        }
                }
                }
        return 0;
}