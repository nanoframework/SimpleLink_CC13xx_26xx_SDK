/******************************************************************************
 @file ini_test.c

 @brief TIMAC 2.0 API Unit test for INI file parser.

 Group: WCS LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2021, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#include "compiler.h"
#include "ini_file.h"
#include "stream.h"
#include "log.h"
#include "fatal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * @var test_str
 * @brief This is a sample INI file as a string used as a test case
 */
static const char test_str[] =
    ";; Comment..\n"
    ";; this has no section\n"
    "numbertest = true\n"
    "\n"
    "[strings]\n"
    "quoted = \"Hichhiker's Guide To the Galaxy\"\n"
    "noquote = Hello world how are you\n"
    "quoted2 = \" leading space\"\n"
    "quoted3 = \"trailing space \"\n"
    "\n"
    "[numbers]\n"
    "decnumber = 42\n"
    "hexnumber = 0x42\n"
    "octnumber = 042\n"
    "; An astrix is decimal 42 also\n"
    "singlequote = '*'\n"
    "\n"
    "[binstr]\n"
    "bin = \"Hello\\x00World\"\n"
    "\n"
    ;

/*
 * @brief test ini file integer parsing
 * @param pINI - parse info
 * @param s - string
 * @param v - expected value
 * @param is_bad - is this expected to be a bad value?
 *
 * @returns void
 */
static void test_integer(struct ini_parser *pINI,
                          const char *s,
                          int v, bool
                          is_bad)
{
    int rv;

    /* this is a white box test
     * we know something about the internals
     * of the parser, and how it works.
     *
     * For example:
     *    We know that pINI->item_value points
     *    into the buffer: pINI->workbuf
     *
     * So .. we can shove any text into
     * the workbuf and point pINI->item_value
     * at the workbuf and try to parse it
     * both as an integer and as a double
     */

    /* clear error */
    pINI->is_error = false;

    /* copy our fake parsed result in */
    strcpy(pINI->workbuf, s);

    /* Setup the work value */
    pINI->item_value = pINI->workbuf;

    LOG_printf(LOG_ALWAYS,"Testing (as integer): %s\n", pINI->item_value);
    rv = (int)INI_valueAsS64(pINI);
    if(pINI->is_error)
    {
        if(is_bad)
        {
            /*ok it was expected to be bad */
        }
        else
        {
            FATAL_printf("WRONG error state for %s\n", s);
        }
    }
    else
    {
        if(v != rv)
        {
            FATAL_printf("WRONG value, got %d, want: %d\n", rv, v);
        }
    }

    /* clear error */
    pINI->is_error = false;

    /* copy our fake parsed result in */
    strcpy(pINI->workbuf, s);

    /* Setup the work value */
    pINI->item_value = pINI->workbuf;

    LOG_printf(LOG_ALWAYS,"Testing (as double): %s\n", pINI->item_value);
    double d;
    d = INI_valueAsDouble(pINI);

    if(pINI->is_error)
    {
        if(is_bad)
        {
            /* good was expected to be bad */
        }
        else
        {
            FATAL_printf("wrong double parse for %s\n", s);
        }
    }
    else
    {
        if(pINI->item_value[0] == '0')
        {
            /* this is probably an octal value */
            /* we skip the test */
            LOG_printf(LOG_ALWAYS,"skip dbl verify (might be octal number)\n");
        }
        else
        {
            if(d != v)
            {
                FATAL_printf("Wrong double value for %s, want: %f, got %f\n",
                             s,
                             (double)(d),
                             (double)(v));
            }
        }
    }
}

/*
 * @brief Test various numeric form parsing
 * @param pINI - The parser info
 */
static void test_all_nums(struct ini_parser *pINI)
{
    LOG_printf(LOG_ALWAYS,
                "Expect 7 syntax errors *BELOW* as we try invalid combos\n");
    /* decimal */
    test_integer(pINI, "1234", 1234, false);
    /* decimal */
    test_integer(pINI, "-1234", -1234, false);
    /* space middle */
    test_integer(pINI, "12 34", 1234, true);
    /* hex */
    test_integer(pINI, "0x1234", 0x1234, false);
    /* octal */
    test_integer(pINI, "01234", 01234, false);
    /* quoted char */
    test_integer(pINI, "'A'", 0x41, false);

    /* alert (bell) */
    test_integer(pINI, "'\\a'", '\a', false);
    /* backspace */
    test_integer(pINI, "'\\b'", '\b', false);
    /* form feed */
    test_integer(pINI, "'\\f'", '\f', false);
    /* cr */
    test_integer(pINI, "'\\r'", '\r', false);
    /* lf */
    test_integer(pINI, "'\\n'", '\n', false);
    /* vert tab */
    test_integer(pINI, "'\\v'", '\v', false);
    /* tab */
    test_integer(pINI, "'\\t'", '\t', false);
    /* single backslash */
    test_integer(pINI, "'\\\\'", '\\', false);
    /* double quote */
    test_integer(pINI, "'\\\"'", '"', false);
    /* single quote */
    test_integer(pINI, "'\\''", '\'', false);

    /* something wrong not expected */
    test_integer(pINI, "'\\Z'", 0, true);
    /* invalid sequence of another type */
    test_integer(pINI, "'\\'", 0, true);

    /* hex conversion */
    test_integer(pINI, "'\\x12'", 0x12, false);
    /* octal conversion */
    test_integer(pINI, "'\\123'", 0123, false);

    /* float */
    strcpy(pINI->workbuf, "-3.14159");
    pINI->item_value = pINI->workbuf;
    double dbl;

    dbl= INI_valueAsDouble(pINI);
    if(pINI->is_error)
    {
        FATAL_printf("parse float is wrong\n");
    }
    if(dbl != -3.14159)
    {
        FATAL_printf("parse foatis wrong\n");
    }
    LOG_printf(LOG_ALWAYS,
                "Expect 7 syntax errors *ABOVE* as we try invalid combos\n");
}

/*
 * @brief Test parsing of various string values
 * @param pINI - The parser info
 * @param given - the given string (in ascii)
 * @param expect - what we expect
 * @param quotechar - the type of external quote we are testing with
 * @returns void
 */
static void do_string2(struct ini_parser *pINI,
                        const char *given,
                        const char *expect,
                        int quotechar)
{
    int len;

    sprintf(pINI->workbuf, "%c%s%c", quotechar, given, quotechar);
    pINI->item_value = pINI->workbuf;
    pINI->is_error = false;
    LOG_printf(LOG_ALWAYS, "Testing string: %s\n", pINI->workbuf);

    len = INI_dequote(pINI);
    /* add 1 to include the null byte */
    len = len + 1;
    if(0 == memcmp(pINI->item_value, expect, len))
    {
        /* all is well */
    }
    else
    {
        LOG_printf(LOG_ERROR, "Given...\n");
        LOG_hexdump(LOG_ALWAYS, 0, given, strlen(given)+1);
        LOG_printf(LOG_ERROR, "Expect...\n");
        LOG_hexdump(LOG_ALWAYS, 0, expect, strlen(given)+1);
        FATAL_printf("Wrong\n");
    }

    LOG_printf(LOG_ALWAYS, "DONE\n");

    /* do the test again using the other quote */
    if(quotechar == '"')
    {
        do_string2(pINI, given, expect, '\'');
    }
}

/*
 * @brief Given a preparsed (C string) convert it and try to parse it.
 * @param pINI - the parser info
 * @param expected - the string to test
 * @param how - how are we testing (type of quote)
 */
static void do_string1(struct ini_parser *pINI, const char *expected, int how)
{
    char given[100];
    char *w;
    int c;
    const char *r;

    /* setup rd & wr pointers */
    w = given;
    r = expected;

    /* read through the string and turn it into backslashes */
    /* w is where we are writing back into the buffer. */
    while(*r)
    {
        c = *r++;
        switch(c)
        {
        default:
            /* normal char */
            if((c >= 0x20) & (c <= 0x7e))
            {
                *w = (char)c;
                w++;
            }
            else
            {
                /* do we encode octal or hex */
                if(how == 0)
                {
                    sprintf(w, "\\%03o", c);
                    w = w + 4;
                }
                if(how == 1)
                {
                    sprintf(w, "\\x%02x", c);
                    w = w + 4;
                }
            }
            break;
            /* these we do as C escapes */
        case '"':  *w++ = '\\';  *w++ = '"'; break;
        case '\'': *w++ = '\\'; *w++ = '\''; break;
        case '\n': *w++ = '\\'; *w++ = 'n'; break;
        case '\r': *w++ = '\\'; *w++ = 'r'; break;
        case '\a': *w++ = '\\'; *w++ = 'a'; break;
        case '\b': *w++ = '\\'; *w++ = 'b'; break;
        case '\t': *w++ = '\\'; *w++ = 't'; break;
        case '\v': *w++ = '\\'; *w++ = 'v'; break;
        case '\\': *w++ = '\\'; *w++ = '\\'; break;
        }
    }
    /* terminate the converted string. */
    *w = 0;

    /* now test - with a double quote the first time */
    /* and a single quote the second time */
    if(how == 0)
    {
        /* try this with a double quote */
        do_string2(pINI, given, expected, '"');
        /* recursive, try with single quote */
        do_string1(pINI, given, 1);
    }
    else
    {
        /* now that we have re-encoded(2nd time), do it with a single quote */
        do_string2(pINI, given, expected, '\'');
    }
}

/*
 * @brief Test various strings
 * @param pINI - The parser info
 * @returns void
 */

static void test_strings(struct ini_parser *pINI)
{

    do_string1(pINI, "" , 0);
    do_string1(pINI, "Duane is here" , 0);
    do_string1(pINI, "Duane\ris\nhere", 0);
    do_string1(pINI, "quote \"string\" here" , 0);
    do_string1(pINI, "quote 'string' two" , 0);

    do_string1(pINI, " \a \b \f \n \r \t \v \" '" , 0);

    /* now some conversions */

    do_string2(pINI,
                " \\a \\b \\f \\n \\r \\t \\v \\\" \\'" ,
                " \a \b \f \n \r \t \v \" '" , '"');

    do_string2(pINI, " \\x12 \\212 ", " \x12 \212 " , '"');
}

/*
 * @brief We have parsed something ... section, name and value, validate
 * @param pINI - the parser info
 * @param s - the expected section name
 * @param n - the expected name
 * @param e - the expected result string
 * @returns 0 if this is not a match, 1 if the test succeeded
 */
static int check_string(struct ini_parser *pINI,
                         const char *s,
                         const char *n,
                         const char *e,
                         bool *handled)
{
    int l;
    if(!INI_itemMatches(pINI, s,n))
    {
        /* no match */
        return (0);
    }
    *handled = true;

    LOG_printf(LOG_ALWAYS,"Verify: [%s] %s\n",
               pINI->cur_section,
               pINI->item_name);
    LOG_printf(LOG_ALWAYS,  "Expect: |%s|\n", e);
    LOG_printf(LOG_ALWAYS, " Quote: |%s|\n", pINI->item_value);
    l = INI_dequote(pINI);
    if(l < 0)
    {
        FATAL_printf("Bad\n");
    }
    LOG_printf(LOG_ALWAYS, "   Got: |%s|\n", pINI->item_value);
    if(0 != strcmp(e, pINI->item_value))
    {
        FATAL_printf("Does not match\n");
    }
    /* success */
    return (0);
}

/*
 * @brief Check a number we parsed from the file
 * @param pINI - the parser info
 * @param s - the section name
 * @param i - the item name
 * @param v - the integer value we expect
 * @returns 0 if not a match, 1 if this is a match and the test passed.
 */
static int check_number(struct ini_parser *pINI,
                         const char *s,
                         const char *i,
                         int v,
                         bool *handled)
{
    int actual;
    if(!INI_itemMatches(pINI, s,i))
    {
        return (0);
    }

    *handled = true;

    LOG_printf(LOG_ALWAYS,
                "Verify item: %s equals: %d\n", pINI->item_value, v);
    actual = (int)(INI_valueAsS64(pINI));
    if(actual != v)
    {
        FATAL_printf("Value does not match\n");
    }
    return (0);
}

/*
 * @brief parser callback we use this to test/verify our parsed results
 * @param pINI - the parser info
 * @param handled - set to true if handled.
 * @returns 0 to tell the parser to keep parsing.
 */
static int my_callback(struct ini_parser *pINI, bool *handled)
{
    int l;
    /* this is prior to any section */
    if(INI_itemMatches(pINI, "", "numbertest"))
    {
        /* we cheat while testing here */
        /* our main focus is various number forms */
        /* and syntax errors related */
        /* so we test the INI_valueAs??() */
        /* function calls by cheating a little */
        test_all_nums(pINI);

        /* Same with various quoted strings */
        /* here we are going to test */
        /* various backslash escaped sequences */
        /* and quoting rules */
        test_strings(pINI);
        *handled = true;
        return (0);
    }

    if(pINI->item_value == NULL)
    {
        /* we just entered a new section */
        /* we don't care about this */
        /* we only care about ITEMs within a section */
        /* so leave now */
        return (0);
    }

    check_string(pINI, "strings", "quoted",
                  "Hichhiker's Guide To the Galaxy", handled);
    check_string(pINI, "strings", "noquote",
                  "Hello world how are you", handled);
    check_string(pINI, "strings", "quoted2", " leading space", handled);
    check_string(pINI, "strings", "quoted3", "trailing space ", handled);

    if(INI_itemMatches(pINI, "binstr", "bin"))
    {
        *handled = true;
        l = INI_dequote(pINI);
        if(l == 11)
        {
            if(0 == memcmp((void*)("Hello\x00World"), pINI->item_value,l))
                LOG_printf(LOG_ALWAYS, "Binary string success\n");
                return (0);
        }
        FATAL_printf("Binary string is wrong\n");
    }

    check_number(pINI, "numbers", "decnumber", 42, handled);
    check_number(pINI, "numbers", "hexnumber", 0x42, handled);
    check_number(pINI, "numbers", "octnumber", 042, handled);
    check_number(pINI, "numbers", "singlequote", '*', handled);
    if(handled)
    {
        return (0);
    }
    FATAL_printf("Someting is wacky\n");
    return (-1);
}

/*
 * @brief Main entry point for this test/example app
 * @param argc - standard parameter
 * @param argv - standard parameter
 * @return exits 0 upon success
 */
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    STREAM_init();
    LOG_init("/dev/stdout");
    intptr_t s;

    s = STREAM_stringCreate(test_str);

    INI_parse(s, "string", my_callback, 0xdeadbeef);

    STREAM_close(s);
    s = 0;

    LOG_printf(LOG_ALWAYS, "Success\n");
    exit(0);
}

/*
 *  ========================================
 *  Texas Instruments Micro Controller Style
 *  ========================================
 *  Local Variables:
 *  mode: c
 *  c-file-style: "bsd"
 *  tab-width: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 *  End:
 *  vim:set  filetype=c tabstop=4 shiftwidth=4 expandtab=true
 */
