/*
 * Test suite for Kerberos error handling routines.
 *
 * The canonical version of this file is maintained in the rra-c-util package,
 * which can be found at <http://www.eyrie.org/~eagle/software/rra-c-util/>.
 *
 * Written by Russ Allbery <rra@stanford.edu>
 * Copyright 2010, 2011
 *     The Board of Trustees of the Leland Stanford Junior University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <config.h>
#include <portable/krb5.h>
#include <portable/system.h>

#include <tests/tap/basic.h>
#include <tests/tap/process.h>
#include <util/macros.h>
#include <util/messages-krb5.h>
#include <util/messages.h>
#include <util/xmalloc.h>


/*
 * Test functions.
 */
static void
test_warn(void *data UNUSED)
{
    krb5_context ctx;
    krb5_error_code code;
    krb5_principal princ;

    code = krb5_init_context(&ctx);
    if (code < 0)
        die_krb5(ctx, code, "cannot create context");
    code = krb5_parse_name(ctx, "foo@bar@EXAMPLE.COM", &princ);
    if (code < 0)
        warn_krb5(ctx, code, "principal parse failed");
    else
        die("unexpected success parsing principal");
    exit(0);
}

static void
test_die(void *data UNUSED)
{
    krb5_context ctx;
    krb5_error_code code;
    krb5_principal princ;

    code = krb5_init_context(&ctx);
    if (code < 0)
        die_krb5(ctx, code, "cannot create context");
    code = krb5_parse_name(ctx, "foo@bar@EXAMPLE.COM", &princ);
    if (code < 0)
        die_krb5(ctx, code, "principal parse failed");
    else
        die("unexpected success parsing principal");
    exit(0);
}


/*
 * Run the tests.
 */
int
main(void)
{
    krb5_context ctx;
    krb5_error_code code;
    krb5_principal princ;
    const char *message;
    char *wanted;

    plan(6 * 3);

    /* First, we have to get what the correct error message is. */
    code = krb5_init_context(&ctx);
    if (code < 0)
        bail("cannot create context");
    code = krb5_parse_name(ctx, "foo@bar@EXAMPLE.COM", &princ);
    message = krb5_get_error_message(ctx, code);

    xasprintf(&wanted, "principal parse failed: %s\n", message);
    is_function_output(test_warn, NULL, 0, wanted, "warn_krb5");
    is_function_output(test_die, NULL, 1, wanted, "die_krb5");
    free(wanted);

    message_program_name = "msg-test";
    xasprintf(&wanted, "msg-test: principal parse failed: %s\n", message);
    is_function_output(test_warn, NULL, 0, wanted, "warn_krb5 with name");
    is_function_output(test_die, NULL, 1, wanted, "die_krb5 with name");
    free(wanted);

    message_handlers_warn(0);
    is_function_output(test_warn, NULL, 0, "", "warn_krb5 with no handlers");
    message_handlers_die(0);
    is_function_output(test_die, NULL, 1, "", "warn_krb5 with no handlers");

    return 0;
}