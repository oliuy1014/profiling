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
#include "decode_inst.h"
#include "structs_and_constants.h"
#include "uarray.h"
#include "bitpack.h"
#include "sys/stat.h"

/******************************global macros*********************************/
#define NUM_REG 8
#define INITIAL_MEM_SIZE 1
#define INITIAL_MAPPED_SIZE 0
#define INITIAL_UNMAPPED_SIZE 0
#define BYTES_PER_WORD 4
#define SPINE_SIZE 4294967295
// #define SPINE_SIZE 100


/**************************function declarations******************************/
FILE *open_file(char *filename, char *mode);
void populate_program(uint32_t **mem_seq, FILE *um_fp, long num_words);
long get_file_words(char *filename);

int main(int argc, char*argv[])
{
        // CPUTime_T timer = CPUTime_New();
        if (argc != 2) {
                printf("Usage: ./um filename.um\n");
                exit(1);
        }

        /* open input file and get number of 32-bit words */
        FILE *um_fp = open_file(argv[1], "r");
        long num_words = get_file_words(argv[1]);

        /* initialize registers and program counter */
        uint32_t registers[NUM_REG] = {0};
        unsigned *prog_counter = malloc(sizeof(*prog_counter));
        // assert(prog_counter != NULL);
        *prog_counter = 0;
        
        /* initialize virtual memory mem_struct */
        uint32_t **mem_seq = malloc(SPINE_SIZE);
        uint32_t *metadata = malloc(2 * sizeof(uint32_t));
        metadata[0] = SPINE_SIZE;
        metadata[1] = 2;
        mem_seq[0] = metadata;

        // assert(mem_seq != NULL);
        // assert(metadata != NULL);
        uint32_t *unmapped = malloc(SPINE_SIZE);
        unmapped[0] = 0;

        /* populate mem[0] with instructions from file */
        populate_program(mem_seq, um_fp, num_words);
        fclose(um_fp);

        /* iterate through m[0], decode and execute each instruction */
        // fprintf(stderr, "m_0 size: %u\n", ((uint32_t *)Seq_get(mem->mem_seq, 0))[0]);
        while (*prog_counter < ((mem_seq)[0][0])) {
                uint32_t *seg_0 = (mem_seq)[1];
                uint32_t inst = seg_0[*prog_counter + 1];
                if (inst >> 28 == 13) {
                        uint32_t reg_idx = (inst << 4) >> 29;
                        uint32_t val = (inst << 7) >> 7;
                        registers[reg_idx] = val;
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
                        execute(A, B, C, OP, mem_seq, unmapped, registers, prog_counter);
                }
        }
        return 0;
}

/**********open_file********
 *
 * Purpose: opens a file for reading. 
 * Parameters:
 *      char *filename: name of file to be opened
 *      char *mode: mode to open (read/write)
 * Returns: file pointer for reading
 * Expects:
 *      filename and mode are non-NULL.
 * Notes:
 *      Will CRE if any of filename, mode, or created file pointer are NULL.
 *      Main closes the file pointer with fclose.
 ************************/
FILE *open_file(char *filename, char *mode)
{
        // assert(filename != NULL);
        // assert(mode != NULL);

        FILE *fp = fopen(filename, mode); 
        if (fp == NULL) {
                fprintf(stderr, "Could not open file %s\n", filename);
                exit(1);
        } else {
                return fp;
        }
}

/**********populate_program********
 *
 * Function that sets up the initial state of our memory. Creates a 
 * UArray of size equal to the number of 32-bit instructions provided 
 * by the input .um file and stores it in segment 0 of our sequence in
 * memory. Populates the UArray so that each element holds a 32-bit word,
 * and initializes each word to 0.  
 * Parameters:
 *      mem_struct mem: struct that stores the memory sequence to be accessed
 *      File *um_fp: pointer to input um file
 *      long num_words: number of 32-bit words in the input um file
 * Returns: None
 * Expects:
 *      mem and um_fp to be non-NULL
 * Notes:
 *      Will CRE if:
 *      - any of mem or um_fp are NULL
 *      - memory cannot be allocated for m[0]
 *      Will raise Bitpack_Overflow if curr_word does not fit in 8 bytes 
 ************************/
void populate_program(uint32_t **mem_seq, FILE *um_fp, long num_words)
{
        // assert(mem != NULL);
        // assert(um_fp != NULL);
        
        int curr_byte = fgetc(um_fp);
        uint32_t *m_0 = malloc((num_words + 1) * sizeof(uint32_t));
        m_0[0] = num_words;
        int word_idx = 0;
        while (curr_byte != EOF) {
                uint32_t curr_word = 0;
                for (int i = 0; i < BYTES_PER_WORD; i++) {
                        curr_word = Bitpack_newu(curr_word, BYTE_W,
                                                 WORD_SZ - (i + 1) * BYTE_W,
                                                 curr_byte);
                        curr_byte = fgetc(um_fp);
                }
                m_0[word_idx + 1] = curr_word;
                word_idx++;
        }
        mem_seq[1] = m_0;
        return;
}

/**********get_file_words********
 *
 * Purpose: Calculates the number of 32-bit instructions/words in a given file  
 * Parameters:
 *      char *filename: name of file with 32-bit instructions
 * Returns: Number of 32-bit words in file
 * Expects:
 *      filename to be non-NULL
 * Notes:
 *      Will CRE if given filename is NULL
 *      Uses stat() to retrieve number of words in file
 ************************/
long get_file_words(char *filename)
{
        // assert(filename != NULL);
        struct stat file_stats;
        assert(stat(filename, &file_stats) == 0);
        return (file_stats.st_size / BYTES_PER_WORD);
}