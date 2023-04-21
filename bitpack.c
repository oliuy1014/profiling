/*****************************************************************************
 *
 *                 bitpack.c
 *
 *      Assignment: Homework 4 -- arith
 *      Authors: Rivan Mehra (rmehra2), Oliver Uy (ouy01)
 *      Date: March 9th, 2023
 *     
 *      Summary: 
 *      Interface that allows for storing, updating, and retrieving of
 *      values from/in 32-bit words.
 *
 ****************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "except.h"
#include <assert.h>
#include <stdio.h>

#define MAX_BITS 64

/**********function declarations*********************************************/
void print_stuff(int64_t n, unsigned width);
Except_T Bitpack_Overflow = { "Overflow packing bits" };

/**********lshift***********************************************************
 *
 * Purpose:
 *      Helper function for left shifting a 64-bit integer.
 * Parameters: 
 *      - uint64_t n: integer to be shifted
 *      - unsigned shift_sz: amount of bits to shift by
 * Returns: 
 *      Shifted 64-bit integer
 * Expects:
 *      None
 * Notes:
 *      Mainly ensures that shifting by 64 bits returns 0, rather than
 *      doing nothing to the input.
 ****************************************************************************/
uint64_t lshift(uint64_t n, unsigned shift_sz)
{
        if (shift_sz >= MAX_BITS) {
                return 0;
        } else {
                return n << shift_sz;
        }
}

/**********rshift_u***********************************************************
 *
 * Purpose:
 *      Helper function for right shifting a 64-bit unsigned integer.
 * Parameters: 
 *      - uint64_t n: integer to be shifted
 *      - unsigned shift_sz: amount of bits to shift by
 * Returns: 
 *      Shifted 64-bit integer
 * Expects:
 *      None
 * Notes:
 *      Mainly ensures that shifting by 64 bits returns 0, rather than
 *      doing nothing to the input.
 ****************************************************************************/
uint64_t rshift_u(uint64_t n, unsigned shift_sz)
{
        if (shift_sz >= MAX_BITS) {
                return 0;
        } else {
                return n >> shift_sz;
        }
}

/**********rshift_s***********************************************************
 *
 * Purpose:
 *      Helper function for right shifting a 64-bit signed integer.
 * Parameters: 
 *      - int64_t n: integer to be shifted
 *      - unsigned shift_sz: amount of bits to shift by
 * Returns: 
 *      Shifted 64-bit integer
 * Expects:
 *      None
 * Notes:
 *      Mainly ensures that shifting by 64 bits returns 0, rather than
 *      doing nothing to the input.
 ****************************************************************************/
int64_t rshift_s(int64_t n, unsigned shift_sz)
{
        if (shift_sz >= MAX_BITS) {
                return ~0;
        } else {
                return n >> shift_sz;
        }
}

/**********Bitpack_fitsu**************************************************
 *
 * Purpose:
 *      Checks whether an unsigned integer value can be stored in a given
 *      number of bits.
 * Parameters: 
 *      - uint64_t n: Number to check if it fits in width bits
 *      - unsigned width: Number of bits to store n inside of
 * Returns: 
 *      boolean indicating whether or not it fits
 * Expects:
 *      width no greater than 64
 * Notes:
 *      Raises CRE if width greater than 64
 ****************************************************************************/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= MAX_BITS);
        if (width <= 0) {
                return false;
        }
        uint64_t max_val = rshift_u(~0 , (MAX_BITS - width));
        return n <= max_val;
}

/**********Bitpack_fitss**************************************************
 *
 * Purpose:
 *      Checks whether an integer value can be stored in a given
 *      number of bits.
 * Parameters: 
 *      - int64_t n: Number to check if it fits in width bits
 *      - unsigned width: Number of bits to store n inside of
 * Returns: 
 *      boolean indicating whether or not it fits
 * Expects:
 *      width no greater than 64
 * Notes:
 *      Raises CRE if width greater than 64
 ****************************************************************************/
bool Bitpack_fitss(int64_t n, unsigned width)
{       
        assert(width <= MAX_BITS);
        if (width <= 0) {
                return false;
        }
        int64_t min_val = lshift(~0, width - 1);
        int64_t max_val = rshift_u(~0 , (MAX_BITS - width + 1));
        return (n >= min_val && n <= max_val);
}

/**********Bitpack_getu**************************************************
 *
 * Purpose:
 *      Extracts a 64-bit unsigned integer from a given field of a word.
 * Parameters: 
 *      - uint64_t word: word containing desired value
 *      - unsigned width: Number of bits in desired field
 *      - unsigned lsb: index of least significant bit of desired field
 * Returns: 
 *      Extracted 64-bit unsigned integer 
 * Expects:
 *      - width no greater than 64
 *      - sum of width and lsb no greater than 64
 * Notes:
 *      Raises CRE if width or sum of width and lsb greater than 64
 ****************************************************************************/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= MAX_BITS);
        assert(width + lsb <= MAX_BITS);
        if (width == 0) {
                return 0;
        }

        uint64_t mask = rshift_u(~0, MAX_BITS - width);
        mask = lshift(mask, lsb);
        uint64_t result = word & mask;
        result = rshift_u(result, lsb);
        return result;
}


/**********Bitpack_gets**************************************************
 *
 * Purpose:
 *      Extracts a 64-bit signed integer from a given field of a word.
 * Parameters: 
 *      - int64_t word: word containing desired value
 *      - unsigned width: Number of bits in desired field
 *      - unsigned lsb: index of least significant bit of desired field
 * Returns: 
 *      Extracted 64-bit signed integer 
 * Expects:
 *      - width no greater than 64
 *      - sum of width and lsb no greater than 64
 * Notes:
 *      Raises CRE if width or sum of width and lsb greater than 64
 ****************************************************************************/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= MAX_BITS);
        assert(width + lsb <= MAX_BITS);
        if (width == 0) {
                return 0;
        }

        int64_t result = Bitpack_getu(word, width, lsb);
        result = lshift(result, (MAX_BITS - width));
        result = rshift_s(result, (MAX_BITS - width));
        return result;
}


/**********Bitpack_newu**************************************************
 *
 * Purpose:
 *      Changes a field of a word to a given unsigned integer value.
 * Parameters: 
 *      - uint64_t word: word to be changed
 *      - unsigned width: Number of bits in desired field
 *      - unsigned lsb: index of least significant bit of desired field
 *      - uint64_t value: value to be inserted into word
 * Returns: 
 *      Changed word as new 64-bit integer
 * Expects:
 *      - width no greater than 64
 *      - sum of width and lsb no greater than 64
 *      - value to fit into given width
 * Notes:
 *      Raises CRE if width or sum of width and lsb greater than 64
 *      Throws Bitpack_Overflow exception if value does not fit in width
 ****************************************************************************/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
        assert(width <= MAX_BITS);
        assert(width + lsb <= MAX_BITS);

        if (!(Bitpack_fitsu(value, width))) {
                RAISE(Bitpack_Overflow);
        }

        uint64_t mask = rshift_u(~0, MAX_BITS - width);
        mask = ~(lshift(mask, lsb));
        word = word & mask;
        value = lshift(value, lsb);
        word = word | value;
        return word;
}

/**********Bitpack_news**************************************************
 *
 * Purpose:
 *      Changes a given field of a word to a desired signed integer.
 * Parameters: 
 *      - uint64_t word: word to be changed
 *      - unsigned width: Number of bits in desired field
 *      - unsigned lsb: index of least significant bit of desired field
 *      - int64_t value: value to be inserted into word
 * Returns: 
 *      Changed word as new 64-bit integer
 * Expects:
 *      - width no greater than 64
 *      - sum of width and lsb no greater than 64
 *      - value to fit into given width
 * Notes:
 *      Raises CRE if width or sum of width and lsb greater than 64
 *      Throws Bitpack_Overflow exception if value does not fit in width
 ****************************************************************************/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                      int64_t value)
{
        assert(width <= MAX_BITS);
        assert(width + lsb <= MAX_BITS);

        if (!(Bitpack_fitss(value, width))) {
                RAISE(Bitpack_Overflow);
        }

        value = Bitpack_getu(value, width, 0);
        word = Bitpack_newu(word, width, lsb, value);
        return word;
}