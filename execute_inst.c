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
 * Parameters:
 *      uint32_t A: idx of register A
 *      uint32_t B: idx of register B
 *      uint32_t C: idx of register C
 *      mem_struct memory: virtual memory struct pointer
 *      uint32_t r[]: array of registers
 *      uint32_t *prog_counter: pointer to program counter
 * Returns:
 *      None
 * Expects:
 *      - A, B, and C to be within the range 0-7
 *      - mem to be non-NULL
 * Notes:
 *      For any non-Halt instruction, may change values contained within the
 *      virtual memory struct, registers array, and/or the program counter.
 ****************************************************************************/
void execute(uint32_t A, uint32_t B, uint32_t C, uint32_t OP,
             mem_struct mem, uint32_t r[], uint32_t *prog_counter)
{
        // assert(r != NULL);
        // assert(prog_counter != NULL);
        // assert(mem != NULL);
        // assert(A < NUM_REG);
        // assert(B < NUM_REG);
        // assert(C < NUM_REG);
        switch (OP) {
                case 0:
                        Cmov(A, B, C, r);
                        break;
                case 1:
                        Seg_load(A, B, C, mem, r);
                        break;
                case 2:
                        Seg_store(A, B, C, mem, r);
                        break;
                case 3:
                        Add(A, B, C, r);
                        break;
                case 4:
                        Mult(A, B, C, r);
                        break;
                case 5:
                        Div(A, B, C, r);
                        break;
                case 6:
                        Nand(A, B, C, r);
                        break;
                case 7:
                        Halt(mem);
                        break;
                case 8:
                        Map(B, C, mem, r);
                        break;
                case 9:
                        Unmap(C, mem, r);
                        break;
                case 10:
                        Output(C, r);
                        break;
                case 11:
                        Input(C, r);
                        break;
                case 12:
                        Load_prog(B, C, mem, r, prog_counter);
                        break;
                default:
                        // assert(OP <= 12);
                        break;
        }
}

/**********Cmov********************************************************
 *
 * Purpose:
 *      If r[C] is nonzero, moves the value stored at r[B] into r[A],
 *      otherwise does nothing.
 * Parameters:
 *      uint32_t A: idx of destination register
 *      uint32_t B: idx of register holding value to move
 *      uint32_t C: idx of register holding value that determines condition
 *      uint32_t r[]: array of registers
 * Returns:
 *      None
 * Expects:
 *      - A, B, and C to be within the range 0-7
 *      - r[] to be non-NULL
 * Notes:
 *      - Called by main driver when executing conditional move instruction
 *      - Will CRE if:
 *              - A, B or C are out of range 0-7
 *              - r points to NULL
 ****************************************************************************/
void Cmov(uint32_t A, uint32_t B, uint32_t C, uint32_t r[])
{
        // assert(A < NUM_REG);
        // assert(B < NUM_REG);
        // assert(C < NUM_REG);
        // assert(r != NULL);
        if (r[C] != 0) {
                r[A] = r[B];
        }
}

/**********Seg_load********************************************************
 *
 * Purpose:
 *      Loads value from mem_struct at segment r[B] and offset r[c] into r[A]
 * Parameters: 
 *      uint32_t A: idx of destination register
 *      uint32_t B: idx of register holding mem segment identifier
 *      uint32_t C: idx of register holding offset for target word in segment
 *      mem_struct mem: pointer to memory struct
 *      uint32_t r[]: array of registers
 * Returns:
 *      None
 * Expects:
 *      - A, B, and C to be within the range 0-7
 *      - mem to be non-NULL
 *      - r[] to be non-NULL
 * Notes:
 *      - Called by main driver when executing segmented load instruction
 *      - Will CRE if:
 *              - mem or r points to NULL, eg calling Seg_load on an 
 *              unmapped segment
 *              - A, B or C are out of range 0-7
 ****************************************************************************/
void Seg_load(uint32_t A, uint32_t B, uint32_t C,
              mem_struct mem, uint32_t r[])
{
        // assert(A < NUM_REG);
        // assert(B < NUM_REG);
        // assert(C < NUM_REG);
        // assert(r != NULL);
        // assert(mem != NULL);
        uint32_t **memseq = mem->mem_seq;
        if (r[B] == 0) {
                // assert((memseq)[r[B] + 1] != NULL);
                uint32_t *seg = (memseq)[1];
                r[A] = seg[r[C] + 1];
                return;
        }
        // assert((memseq)[r[B]] != NULL);
        uint32_t *seg = (memseq)[r[B]];
        r[A] = seg[r[C] + 1];
}

/**********Seg_store********************************************************
 *
 * Purpose:
 *      Stores value from r[C] in mem_struct at segment r[A] and offset r[B]
 * Parameters:
 *      uint32_t A: idx of register holding segment identifier
 *      uint32_t B: idx of register holding offset of value in segment
 *      uint32_t C: idx of register holding word to store in mem
 *      mem_struct mem: pointer to memory struct 
 *      uint32_t r[]: array of registers  
 * Returns: 
 *      None
 * Expects:
 *      - A, B, and C to be within the range 0-7
 *      - mem to be non-NULL
 *      - r[] to be non-NULL
 * Notes:
 *      - Called by main driver when executing segmented store instruction.
 *      - Will CRE if:
 *              - mem or r are NULL, eg calling Seg_store on an 
 *              unmapped segment
 *              - A, B or C are out of range 0-7
 ****************************************************************************/
void Seg_store(uint32_t A, uint32_t B, uint32_t C,
               mem_struct mem, uint32_t r[])
{
        // assert(A < NUM_REG);
        // assert(B < NUM_REG);
        // assert(C < NUM_REG);
        // assert(r != NULL);
        // assert(mem != NULL);
        uint32_t **memseq = mem->mem_seq;
        uint32_t *segA = memseq[r[A]];
        if (r[A] == 0) {
                // assert((memseq)[r[A] + 1] != NULL);
                uint32_t *seg = (memseq)[1];
                seg[r[B] + 1] = r[C];
                return;
        }
        // assert(segA != NULL);
        segA[r[B] + 1] = r[C];
}

/**********Add********************************************************
 *
 * Purpose:
 *      Adds the values r[B] and r[C] and stores them in r[A]
 * Parameters:
 *      uint32_t A: idx of destination register
 *      uint32_t B, C: idx of registers holding values to be added and stored
 *      uint32_t r[]: array of registers 
 * Expects:
 *      - A, B, and C to be within the range 0-7
 *      - r to be non-NULL
 * Notes:
 *      Called by main driver when executing add instruction.
 *      Will CRE if:
 *      - r is NULL
 *      - A, B or C are out of range 0-7 
 ****************************************************************************/
void Add(uint32_t A, uint32_t B, uint32_t C, uint32_t r[])
{
        // assert(A < NUM_REG);
        // assert(B < NUM_REG);
        // assert(C < NUM_REG);
        // assert(r != NULL);
        r[A] = (r[B] + r[C]);
}

/**********Mult********************************************************
 *
 * Purpose:
 *      Multiplies the values B and C and stores them in A
 * Parameters:
 *      uint32_t A: idx of destination register
 *      uint32_t B, uint32_t C: idx of registers holding values to multiply
 *      uint32_t r[]: array of registers 
 * Expects:
 *      - A, B, and C to be within the range 0-7
 *      - r to be non-NULL
 * Notes:
 *      Called by main driver when executing multiply instruction.
 *      Will CRE if:
 *      - r is NULL
 *      - A, B or C are out of range 0-7      
 ****************************************************************************/
void Mult(uint32_t A, uint32_t B, uint32_t C, uint32_t r[])
{
        // assert(A < NUM_REG);
        // assert(B < NUM_REG);
        // assert(C < NUM_REG);
        // assert(r != NULL);
        r[A] = (r[B] * r[C]);
}

/**********Div********************************************************
 *
 * Purpose:
 *      Divides r[B] by r[C] and stores the floor of the quotient in r[A]
 * Parameters:
 *      uint32_t A: idx of destination register
 *      uint32_t B: idx of registers holding dividend
 *      uint32_t C: idx of registers holding divisor
 *      uint32_t r[]: array of registers
 * Expects:
 *      - A, B, and C to be within the range 0-7
 *      - r to be non-NULL
 * Notes:
 *      Called by main driver when executing multiply instruction.
 *      Will CRE if:
 *      - r is NULL
 *      - A, B or C are out of range 0-7
 ****************************************************************************/
void Div(uint32_t A, uint32_t B, uint32_t C, uint32_t r[])
{
        // assert(A < NUM_REG);
        // assert(B < NUM_REG);
        // assert(C < NUM_REG);
        // assert(r != NULL);

        r[A] = ((r[B] / r[C]));
}

/**********Nand********************************************************
 *
 * Purpose:
 *      Bitwise NAND's the values r[B] and r[C] and stores the result in r[A]
 * Parameters:
 *      uint32_t A: idx of destination register
 *      uint32_t B, uint32_t C: idx of registers holding values to be NAND-ed
 *      uint32_t r[]: array of registers 
 * Expects:
 *      - A, B, and C to be within the range 0-7
 *      - r to be non-NULL
 * Notes:
 *      Called by main driver when executing NAND instruction.
 *      Will CRE if:
 *      - r is NULL
 *      - A, B or C are out of range 0-7      
 ****************************************************************************/
 void Nand(uint32_t A, uint32_t B, uint32_t C, uint32_t r[])
 {
        // assert(A < NUM_REG);
        // assert(B < NUM_REG);
        // assert(C < NUM_REG);
        // assert(r != NULL);

        r[A] = ~((r[B] & r[C]));
 }

 /**********Halt********************************************************
 *
 * Purpose:
 *      Stops um program, frees any currently allocated memory and exits.
 * Parameters:
 *      *mem: pointer to mem_struct to be freed
 * Expects:
 *      mem to be non-NULL
 * Notes:
 *      Called by main driver when executing Halt instruction. Frees any
 *      memory allocated inside mem
 *      Will CRE if mem is NULL
 ****************************************************************************/
void Halt(mem_struct mem) 
{
        // assert(mem != NULL);

        uint32_t num_segs = (mem->mem_seq[0][1]);
        for (long seg = 0; seg < num_segs; seg++) {
                uint32_t *curr_seg = (mem->mem_seq)[seg];
                if (curr_seg != NULL) {
                        free(curr_seg);
                }
        }
        free(mem->mem_seq);
        free(mem->unmapped);
        free(mem);
        exit(0);
}

/**********Map********************************************************
 *
 * Purpose:
 *      Maps a segment in mem of size r[C], stores the segment identifier
 *      in r[B].
 * Parameters:
 *      uint32_t B: idx of destination register
 *      uint32_t C: idx of register holding the size of segment to map
 *      uint32_t r[]: array of registers 
 *      *mem: pointer to mem_struct
 * Expects:
 *      - A, B, and C to be within the range 0-7
 *      - mem to be non-NULL
 *      - r to be non-NULL
 * Notes:
 *      Called by main driver when executing Map Segment instruction.
 *      Will CRE if:
 *      - Memory cannot be allocated for new UArray_T segment being mapped
 *      - mem or r are NULL
 *      - A, B or C are out of  range 0-7
 ****************************************************************************/
void Map(uint32_t B, uint32_t C, mem_struct mem, uint32_t r[])
{
        // assert(B < NUM_REG);
        // assert(C < NUM_REG);
        // assert(r != NULL);
        // assert(mem != NULL);
        
        uint32_t *new_seg = calloc(((size_t) r[C]) + 1, sizeof(uint32_t));
        new_seg[0] = r[C] + 1;
        uint32_t unmapped_size = mem->unmapped[0];
        // fprintf(stderr, "size ID: %d\n", unmapped_size);
        if (unmapped_size != 0) {
                uint32_t unmapped_id = mem->unmapped[unmapped_size];
                // for (unsigned i = 0; i < unmapped_size; i++) {
                //         fprintf(stderr, "ID: %d\n", mem->unmapped[i]);
                // }
                
                (mem->mem_seq)[unmapped_id] = new_seg;
                r[B] = unmapped_id;
                mem->unmapped[0]--;
        } else {
                uint32_t *meta = mem->mem_seq[0];
                uint32_t seg_id = meta[1];
                (mem->mem_seq)[seg_id] = new_seg;
                r[B] = seg_id;
                meta[1] = seg_id + 1;
        }
}

/**********Unmap********************************************************
 *
 * Purpose:
 *      Unmaps the mem_struct segment at mem[r[C]]. Adds unmapped identifier
 *      to unmapped sequence within mem_struct. Frees unmapped segment.
 * Parameters:
 *      uint32_t C: idx of register holding identifier of target memory
 *      segment to unmap
 *      *mem: pointer to mem_struct
 *      uint32_t r[]: array of registers 
 * Expects:
 *      - C to be within the range 0-7
 *      - r to be non-NULL
 *      - mem to be non-NULL
 * Notes:
 *      Will CRE if C is out of range or r or mem are NULL, or if memory
 *      cannot be malloced for segment id being added to unmapped sequence
 *      Frees UArray_T at segment being unmapped
 ****************************************************************************/
void Unmap(uint32_t C, mem_struct mem, uint32_t r[])
{

        // assert(C < NUM_REG);
        // assert(r != NULL);
        // assert(mem != NULL);

        /* free segment being unmapped */
        uint32_t *seg = (mem->mem_seq)[r[C]];
        free(seg);

        /* set pointer in mem_struct to NULL and add id to unmapped sequence */
        mem->mem_seq[r[C]] = NULL;
        mem->unmapped[mem->unmapped[0] + 1] = r[C];
        mem->unmapped[0]++;
        // fprintf(stderr, "idx: %d, val: %u\n", mem->unmapped[0], mem->unmapped[mem->unmapped[0]]);
}

/**********Output********************************************************
 *
 * Purpose:
 *      Prints provided uint32_t as a char to stdout
 * Parameters:
 *      uint32_t C: idx of register holding int value of char to print
 *      uint32_t r[]: array of registers 
 * Returns: 
 *      None
 * Expects:
 *      - C to be within the range 0-7
 *      - r to be non-NULL
 *      - r[C] to be within the range 0-255
 * Notes:
 *      Will CRE if C or r[C] is out of range or r is NULL
 ****************************************************************************/
void Output(uint32_t C, uint32_t r[])
{
        // assert(C < NUM_REG);
        // assert(r != NULL);
        // assert(r[C] <= MAX_CHAR);

        /* if r[C] is all 1's, do not print */
        if (r[C] == (uint32_t) ~0) {
                return;
        }
        /* otherwise, print char of value at r[C] */
        putc(r[C], stdout);
}

/**********Input********************************************************
 *
 * Purpose:
 *      Asks for input on stdin, stores input at registers[C]
 * Parameters:
 *      uint32_t C: idx of destination register
 *      uint32_t r[]: array of registers 
 * Returns: 
 *      None
 * Expects:
 *      - C to be within the range 0-7
 *      - r to be non-NULL
 * Notes:
 *      Will CRE if C is out of range or r is NULL
 ****************************************************************************/
void Input(uint32_t C, uint32_t r[])
{
        // assert(C < NUM_REG);
        // assert(r != NULL);

        int val = getc(stdin);

        /* if input reaches EOF, fill r[C] with all 1's, otherwise just
         * put int value of input char in r[C]
         */
        if (val != EOF) {
                r[C] = (uint32_t) val;
        } else {
                r[C] = (uint32_t) ~0;
        }
}

/**********Load_prog********************************************************
 *
 * Purpose:
 *      Loads program contained at mem segment r[B] into mem[0], deletes 
 *      previous contents of m[0]. Changes prog_counter to point to mem[0][C].
 * Parameters:
 *      uint32_t B: idx of register holding identifier of memory segment
 *                  holding program to load
 *      *mem: pointer to mem_struct
 *      uint32_t C: idx of register holding value to set prog_counter to
 *      uint32_t prog_counter: pointer to program counter
 *      uint32_t r[]: array of registers 
 * Returns: 
 *      None
 * Expects:
 *      - B and C to be within the range 0-7
 *      - mem to be non-NULL
 *      - r to be non-NULL
 *      - prog_counter to be non_NULL
 *      - segment r[B] to be mapped
 * Notes:
 *      Called by main driver when executing Load Program instruction.
 *      Will CRE if:
 *      - mem, r, or prog_counter is NULL
 *      - B or C is out of range
 *      - memory cannot be allocated for copy of segment mem[r[B]]
 *      - segment at r[B] has been unmapped
 ****************************************************************************/
void Load_prog(uint32_t B, uint32_t C, mem_struct mem, uint32_t r[],
               uint32_t *prog_counter)
{
        // assert(B < NUM_REG);
        // assert(C < NUM_REG);
        // assert(mem != NULL);
        // assert(r != NULL);
        // assert(prog_counter != NULL);
        // assert((mem->mem_seq)[r[B]] != NULL);

        *prog_counter = r[C];
        
        /* if loading program from mem[0] just return */
        if (r[B] == 0) {
                return;
        }
        
        /* otherwise, copy segment from r[B] into mem[0] and free the 
         * previous contents of m[0]
         */
        uint32_t *old_prog = (mem->mem_seq)[1];
        uint32_t *prog_seg = (mem->mem_seq)[r[B]];
        size_t prog_bytes = (prog_seg[0] + 1) * 4;
        uint32_t *prog_copy = malloc(prog_bytes);
        memcpy(prog_copy, prog_seg, prog_bytes);
        free(old_prog);
        mem->mem_seq[1] = prog_copy;
}

/**********Load_val********************************************************
 *
 * Purpose:
 *      Loads value val into register pointed to by reg
 * Parameters:
 *      uint32_t val: value to load into register
 *      uint32_t A: idx of destination register
 *      uint32_t r[]: array of registers 
 * Returns: 
 *      None
 * Expects:
 *      - A to be within the range 0-7
 *      - val to be less than 2^25
 * Notes:
 *      Will CRE val or A is out of range
 ****************************************************************************/
void Load_val(uint32_t A, uint32_t val, uint32_t r[])
{
        // assert(A < NUM_REG);
        // assert(r != NULL);

        r[A] = val;
        return;
}