/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 * 
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>

/* Global Macros */
static unsigned OP_WIDTH = 4;
static unsigned REG_WIDTH = 3;
static unsigned RA_LSB = 6;
static unsigned RB_LSB = 3;
static unsigned RC_LSB = 0;
static unsigned LOAD_OP_VAL = 13;
static unsigned LOAD_REG_LSB = 25;
static unsigned LOAD_VAL_LSB = 0;
static unsigned LOAD_VAL_WIDTH = 25;
static unsigned WORD_LENGTH = 32;
/* */
typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(unsigned ra, unsigned val);

/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction multiply(Um_register a, Um_register b, Um_register c) 
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction divide(Um_register a, Um_register b, Um_register c) 
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction input(Um_register c)
{
        return three_register(IN, 0, 0, c);
}

static inline Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}

static inline Um_instruction segload(Um_register a, Um_register b, Um_register c)
{
        return three_register(SLOAD, a, b, c);
}

static inline Um_instruction cmov(Um_register a, Um_register b, Um_register c)
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction segstore(Um_register a, Um_register b, Um_register c)
{
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction map(Um_register b, Um_register c)
{
        return three_register(ACTIVATE, 0, b, c);
}

static inline Um_instruction unmap(Um_register c)
{
        return three_register(INACTIVATE, 0, 0, c);
}

static inline Um_instruction loadprog(Um_register b, Um_register c)
{
        return three_register(LOADP, 0, b, c);
}


static inline Um_instruction nand(Um_register a, Um_register b, Um_register c)
{
        return three_register(NAND, a, b, c);
}

/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/************************ UNIT TESTS for the UM ***************************/

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}

void build_print_digit_test(Seq_T stream)
{
        append(stream, loadval(r1, 54));
        append(stream, output(r1));
        append(stream, halt());
}

void build_1char_input_test(Seq_T stream)
{
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, halt());
}

void build_loadval_test(Seq_T stream)
{
        append(stream, loadval(r1, 117));
        append(stream, output(r1));
        append(stream, halt());
}

void build_add_test(Seq_T stream)
{
        append(stream, loadval(r3, 'l'));
        append(stream, loadval(r2, 1));
        append(stream, add(r1, r2, r3));
        append(stream, output(r1));
        append(stream, halt());
}

void build_multiply_test(Seq_T stream)
{
        append(stream, loadval(r1, 10));
        append(stream, loadval(r2, 10));
        append(stream, multiply(r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void build_divide_test(Seq_T stream)
{
        append(stream, loadval(r1, 132));
        append(stream, loadval(r2, 2));
        append(stream, divide(r3, r1, r2));
        append(stream, output(r3));
        append(stream, loadval(r1, 134));
        append(stream, loadval(r2, 2));
        append(stream, divide(r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void build_multiply_divide_test(Seq_T stream)
{
        append(stream, input(r3));
        append(stream, loadval(r2, 2));
        append(stream, divide(r1, r3, r2));
        append(stream, multiply(r3, r1, r2));
        append(stream, output(r3));
        append(stream, output(r1));
        append(stream, halt());
}



void build_cmov_test(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 97));
        append(stream, cmov(r3, r2, r1));
        append(stream, output(r3));
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 107));
        append(stream, cmov(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}

void build_nand_test(Seq_T stream)
{
        append(stream, loadval(r3, 255));
        append(stream, loadval(r2, 16777216));
        append(stream, multiply(r4, r2, r3));
        append(stream, loadval(r2, 16777215));
        append(stream, add(r1, r2, r4));

        append(stream, loadval(r2, 2096875));
        append(stream, loadval(r3, 64));
        append(stream, loadval(r4, 15625));
        append(stream, loadval(r5, 32));
        append(stream, loadval(r6, 67229));
        append(stream, multiply(r7, r2, r3));
        append(stream, add(r2, r7, r4));
        append(stream, multiply(r3, r2, r5));
        append(stream, add(r4, r3, r6));
        append(stream, nand(r2, r1, r4));
        append(stream, output(r2));
        append(stream, halt());
}

void build_map_test(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 3));
        append(stream, map(r1, r2));
        append(stream, halt());
}

void build_segload_test(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 1));
        append(stream, segload(r3, r1, r2)); 
        append(stream, halt());
}

void build_segstore_test(Seq_T stream)
{
        append(stream, loadval(r3, 0));
        append(stream, loadval(r2, 1));
        append(stream, loadval(r1, 58));
        append(stream, segstore(r3, r2, r1)); 
        append(stream, halt());
}

void build_initial_memory_test(Seq_T stream)
{
        append(stream, loadval(r2, 4)); // r2 = 4
        append(stream, loadval(r5, 1)); // r2 = 4
        append(stream, loadval(r3, 'B')); // r3 = 'B'
        append(stream, map(r1, r2)); // r1 = malloc 4 words
        append(stream, segstore(r1, r5, r3)); // r1[2] = 'B'
        append(stream, segload(r4, r1, r5)); // r4 = 'B'
        append(stream, output(r4)); 
        append(stream, halt());
}


void build_unmap_success_test(Seq_T stream)
{
        append(stream, loadval(r2, 3));
        append(stream, map(r1, r2));
        append(stream, unmap(r1));
        append(stream, map(r1, r2));
        append(stream, loadval(r3, 2));
        append(stream, segload(r4, r1, r3));
        append(stream, halt());
}


void build_long_memory_test_1(Seq_T stream)
{
        append(stream, loadval(r2, 4));   // r2 = 4
        append(stream, map(r1, r2));      // r1 = malloc(4words)
        append(stream, loadval(r4, 'M')); // r4 = 'M
        append(stream, loadval(r5, 2));   // r5 = 2
        append(stream, segstore(r1, r5, r4)); // *r1[2] = 'M'
        append(stream, loadval(r5, 0));
        append(stream, loadval(r4, 'D'));
        append(stream, segstore(r1, r5, r4));
        append(stream, loadval(r5, 1));
        append(stream, loadval(r4, 'a'));
        append(stream, segstore(r1, r5, r4));
        append(stream, loadval(r5, 3));
        append(stream, loadval(r4, '!'));
        append(stream, segstore(r1, r5, r4));
        for (int i = 0; i < 4; i++) {
                append(stream, loadval(r5, i));
                append(stream, segload(r6, r1, r5));
                append(stream, output(r6));
        }
        append(stream, halt());
}


void build_long_memory_test_2(Seq_T stream)
{
        append(stream, loadval(r2, 13));   // r2 = 4
        append(stream, map(r1, r2));
        for (int i = 0; i < 13; i++) {
                append(stream, loadval(r4, 'a' + i)); // r4 = 'M
                append(stream, loadval(r5, i));   // r5 = 2
                append(stream, segstore(r1, r5, r4)); // *r1[2] = 'M'
        }
        for (int i = 0; i < 13; i++) {
                append(stream, loadval(r2, i));
                append(stream, segload(r4, r1, r2));
                append(stream, output(r4));
        }
        for (int i = 0; i < 1000; i++) {
                append(stream, loadval(r2, i));
                append(stream, map(i % 8, r2));
                append(stream, unmap(i % 8));
        }
        append(stream, loadval(r2, 13));   // r2 = 4
        append(stream, map(r1, r2));
        for (int i = 0; i < 13; i++) {
                append(stream, loadval(r4, 'n' + i)); // r4 = 'M
                append(stream, loadval(r5, i));   // r5 = 2
                append(stream, segstore(r1, r5, r4)); // *r1[2] = 'M'
        }
        for (int i = 0; i < 13; i++) {
                append(stream, loadval(r2, i));
                append(stream, segload(r4, r1, r2));
                append(stream, output(r4));
        }
        append(stream, halt());
}


void build_loadprog_print_test(Seq_T stream)
{
        append(stream, loadval(r2, 3));
        append(stream, map(r1, r2));
        append(stream, loadval(r3, 2));
        append(stream, loadval(r5, 7));
        append(stream, loadval(r6, 16777216));
        append(stream, multiply(r7, r5, r6));
        append(stream, loadval(r6, 16));
        append(stream, multiply(r4, r7, r6));
        append(stream, segstore(r1, r3, r4));
        
        // append(stream, loadval(r4, (3690987586)));
        // (225000 * 128 + 28125 ) * 128 + 987586
        append(stream, loadval(r7, 225000));
        append(stream, loadval(r2, 128));
        append(stream, loadval(r3, 28125));
        append(stream, loadval(r4, 128));
        append(stream, loadval(r5, 987586));
        append(stream, multiply(r0, r7, r2));
        append(stream, add(r7, r0, r3));
        append(stream, multiply(r2, r7, r4));
        append(stream, add(r3, r2, r5));
        append(stream, loadval(r4, 0));
        append(stream, segstore(r1, r4, r3));

        // 54566 + (5242500 * 16 + 4375) * 32 
        append(stream, loadval(r0, 54566));
        append(stream, loadval(r7, 5242500));
        append(stream, loadval(r2, 16));
        append(stream, loadval(r3, 4375));
        append(stream, loadval(r4, 32));
        append(stream, multiply(r5, r7, r2));
        append(stream, add(r7, r5, r3));
        append(stream, multiply(r2, r7, r4));
        append(stream, add(r3, r2, r0));
        append(stream, loadval(r6, 1));
        append(stream, segstore(r1, r6, r3));
        append(stream, loadval(r3, 0));
        append(stream, loadprog(r1, r3));
}

void build_loadprog_noprint_test(Seq_T stream)
{
        append(stream, loadval(r2, 3));
        append(stream, map(r1, r2));
        append(stream, loadval(r3, 0));
        append(stream, loadval(r5, 7));
        append(stream, loadval(r6, 16777216));
        append(stream, multiply(r7, r5, r6));
        append(stream, loadval(r6, 16));
        append(stream, multiply(r4, r7, r6));
        append(stream, segstore(r1, r3, r4));
        
        // append(stream, loadval(r4, (3690987586)));
        // (225000 * 128 + 28125 ) * 128 + 987586
        append(stream, loadval(r7, 225000));
        append(stream, loadval(r2, 128));
        append(stream, loadval(r3, 28125));
        append(stream, loadval(r4, 128));
        append(stream, loadval(r5, 987586));
        append(stream, multiply(r0, r7, r2));
        append(stream, add(r7, r0, r3));
        append(stream, multiply(r2, r7, r4));
        append(stream, add(r3, r2, r5));
        append(stream, loadval(r4, 1));
        append(stream, segstore(r1, r4, r3));

        // 54566 + (5242500 * 16 + 4375) * 32 
        append(stream, loadval(r0, 54566));
        append(stream, loadval(r7, 5242500));
        append(stream, loadval(r2, 16));
        append(stream, loadval(r3, 4375));
        append(stream, loadval(r4, 32));
        append(stream, multiply(r5, r7, r2));
        append(stream, add(r7, r5, r3));
        append(stream, multiply(r2, r7, r4));
        append(stream, add(r3, r2, r0));
        append(stream, loadval(r6, 2));
        append(stream, segstore(r1, r6, r3));
        append(stream, loadval(r3, 0));
        append(stream, loadprog(r1, r3));
}


/************************ UNIT TESTS for the UM ABOVE ***********************/

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction inst = 0;
        assert(Bitpack_fitsu(op, OP_WIDTH));
        assert(Bitpack_fitsu(ra, REG_WIDTH));
        assert(Bitpack_fitsu(rb, REG_WIDTH));
        assert(Bitpack_fitsu(rc, REG_WIDTH));
        inst = Bitpack_newu(inst, OP_WIDTH, WORD_LENGTH - OP_WIDTH, op);
        inst = Bitpack_newu(inst, REG_WIDTH, RA_LSB, ra);
        inst = Bitpack_newu(inst, REG_WIDTH, RB_LSB, rb);
        inst = Bitpack_newu(inst, REG_WIDTH, RC_LSB, rc);
        return inst;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction inst = 0;
        assert(Bitpack_fitsu(ra, REG_WIDTH));
        inst = Bitpack_newu(inst, OP_WIDTH, WORD_LENGTH - OP_WIDTH, LOAD_OP_VAL);
        inst = Bitpack_newu(inst, REG_WIDTH, LOAD_REG_LSB, ra);
        inst = Bitpack_newu(inst, LOAD_VAL_WIDTH, LOAD_VAL_LSB, val);
        return inst;
}