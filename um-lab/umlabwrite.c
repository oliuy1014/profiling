#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"

extern void Um_write_sequence(FILE *output, Seq_T instructions);

extern void build_halt_test(Seq_T instructions);
extern void build_verbose_halt_test(Seq_T instructions);
extern void build_add_test(Seq_T instructions);
extern void build_print_digit_test(Seq_T instructions);
extern void build_segload_test(Seq_T instructions);
extern void build_multiply_divide_test(Seq_T instructions);
extern void build_divide_test(Seq_T instructions);
extern void build_cmov_test(Seq_T instructions);
extern void build_initial_memory_test(Seq_T instructions);
extern void build_unmap_success_test(Seq_T instructions);
extern void build_long_memory_test_1(Seq_T instructions);
extern void build_long_memory_test_2(Seq_T instructions);
extern void build_loadprog_print_test(Seq_T instructions);
extern void build_loadprog_noprint_test(Seq_T instructions);
extern void build_loadval_test(Seq_T instructions);
extern void build_1char_input_test(Seq_T instructions);
extern void build_nand_test(Seq_T instructions);
extern void build_multiply_test(Seq_T instructions);
extern void build_segstore_test(Seq_T instructions);
extern void build_map_test(Seq_T instructions);

/* The array `tests` contains all unit tests for the lab. */

static struct test_info {
        const char *name;
        const char *test_input;          /* NULL means no input needed */
        const char *expected_output;
        /* writes instructions into sequence */
        void (*build_test)(Seq_T stream);
} tests[] = {
        { "halt",         NULL, "",  build_halt_test },
        { "halt-verbose", NULL, "",  build_verbose_halt_test },
        { "loadval", NULL, "u", build_loadval_test},
        { "add",          "L", "M", build_add_test },
        { "print-six",    NULL, "6", build_print_digit_test },
        { "1char_input", "x", "x", build_1char_input_test },
        { "segload", NULL, NULL, build_segload_test },
        { "multiply", NULL, "d", build_multiply_test },
        { "divide", NULL, "BC", build_divide_test},
        { "mult-div", "z", "z=4h", build_multiply_divide_test },
        { "cmov", NULL, "k", build_cmov_test },
        { "nand", NULL, "B", build_nand_test},
        { "segstore", NULL, NULL, build_segstore_test} ,
        { "map", NULL, NULL, build_map_test} ,
        { "initial_memory", NULL, "B", build_initial_memory_test},
        { "long_memory_1", NULL, "DaM!", build_long_memory_test_1 },
        { "unmap_success", NULL, NULL, build_unmap_success_test },
        { "loadprogprint", NULL, NULL, build_loadprog_print_test } ,
        { "loadprognoprint", NULL, NULL, build_loadprog_noprint_test } ,
        { "long_memory_2", NULL, "abcdefg", build_long_memory_test_2 }
};

  
#define NTESTS (sizeof(tests)/sizeof(tests[0]))

/*
 * open file 'path' for writing, then free the pathname;
 * if anything fails, checked runtime error
 */
static FILE *open_and_free_pathname(char *path);

/*
 * if contents is NULL or empty, remove the given 'path', 
 * otherwise write 'contents' into 'path'.  Either way, free 'path'.
 */
static void write_or_remove_file(char *path, const char *contents);

static void write_test_files(struct test_info *test);


int main (int argc, char *argv[])
{
        bool failed = false;
        if (argc == 1)
                for (unsigned i = 0; i < NTESTS; i++) {
                        printf("***** Writing test '%s'.\n", tests[i].name);
                        write_test_files(&tests[i]);
                }
        else
                for (int j = 1; j < argc; j++) {
                        bool tested = false;
                        for (unsigned i = 0; i < NTESTS; i++)
                                if (!strcmp(tests[i].name, argv[j])) {
                                        tested = true;
                                        write_test_files(&tests[i]);
                                }
                        if (!tested) {
                                failed = true;
                                fprintf(stderr,
                                        "***** No test named %s *****\n",
                                        argv[j]);
                        }
                }
        return failed; /* failed nonzero == exit nonzero == failure */
}


static void write_test_files(struct test_info *test)
{
        FILE *binary = open_and_free_pathname(Fmt_string("%s.um", test->name));
        Seq_T instructions = Seq_new(0);
        test->build_test(instructions);
        Um_write_sequence(binary, instructions);
        Seq_free(&instructions);
        fclose(binary);

        write_or_remove_file(Fmt_string("%s.0", test->name),
                             test->test_input);
        write_or_remove_file(Fmt_string("%s.1", test->name),
                             test->expected_output);
}


static void write_or_remove_file(char *path, const char *contents)
{
        if (contents == NULL || *contents == '\0') {
                remove(path);
        } else {
                FILE *input = fopen(path, "wb");
                assert(input != NULL);

                fputs(contents, input);
                fclose(input);
        }
        free(path);
}

static FILE *open_and_free_pathname(char *path)
{
        FILE *fp = fopen(path, "wb");
        assert(fp != NULL);

        free(path);
        return fp;
}