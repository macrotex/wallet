/*
 * Internal support functions for the wallet client.
 *
 * Written by Russ Allbery <eagle@eyrie.org>
 * Copyright 2018 Russ Allbery <eagle@eyrie.org>
 * Copyright 2007-2008, 2010
 *     The Board of Trustees of the Leland Stanford Junior University
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CLIENT_INTERNAL_H
#define CLIENT_INTERNAL_H 1

#include <portable/macros.h>
#include <portable/krb5.h>

#include <sys/types.h>

/*
 * Allow defaults to be set for a particular site with configure options if
 * people don't want to use krb5.conf for some reason.
 */
#ifndef WALLET_SERVER
# define WALLET_SERVER NULL
#endif
#ifndef WALLET_PORT
# define WALLET_PORT 0
#endif

/* Forward declarations to avoid unnecessary includes. */
struct remctl;
struct iovec;

/*
 * Basic wallet behavior options set either on the command line or via
 * krb5.conf.  If set via krb5.conf, we allocate memory for the strings, but
 * we never free them.
 */
struct options {
    char *type;
    char *server;
    char *principal;
    char *user;
    unsigned short port;
};

BEGIN_DECLS

/*
 * Set default options from the system krb5.conf or from compile-time
 * defaults.
 */
void default_options(krb5_context ctx, struct options *options);

/*
 * Given a Kerberos context and a principal name, obtain Kerberos credentials
 * for that principal and store them in a temporary ticket cache for use by
 * later operations.  kdestroy() then cleans up that cache.
 */
void kinit(krb5_context, const char *principal);
void kdestroy(void);

/*
 * Given a remctl object, either a NULL-terminated array of strings or an
 * array of iovecs and the number of elements in the array, and optional data
 * and size output variables, run a remctl command.  If data is non-NULL,
 * saves the standard output from the command into data with the length in
 * length.  Otherwise, prints it to standard output.  Either way, prints
 * standard error output and errors to standard error and returns the exit
 * status or 255 for a remctl internal error.
 */
int run_command(struct remctl *, const char **command, char **data,
                size_t *length);
int run_commandv(struct remctl *, const struct iovec *command, size_t count,
                 char **data, size_t *length);

/*
 * Check whether an object exists using the exists wallet interface.  Returns
 * true if it does, false if it doesn't, and dies on remctl errors.
 */
int object_exists(struct remctl *, const char *prefix, const char *type,
                  const char *name);

/* Attempt autocreation of an object.  Dies if autocreation fails. */
void object_autocreate(struct remctl *, const char *prefix, const char *type,
                       const char *name);

/*
 * Given a remctl object, the type for the wallet interface, object type,
 * object name, and a file (which may be NULL), send a wallet get command and
 * write the results to the provided file.  If the file is NULL, write the
 * results to standard output instead.  Returns 0 on success and an exit
 * status on failure.
 */
int get_file(struct remctl *, const char *prefix, const char *type,
             const char *name, const char *file);

/*
 * Given a remctl object, the Kerberos context, the type for the wallet
 * interface, the name of a keytab object, and a file name, call the correct
 * wallet commands to download a keytab and write it to that file.  If srvtab
 * is not NULL, write a srvtab based on the keytab after a successful
 * download.
 */
int get_keytab(struct remctl *, krb5_context, const char *type,
               const char *name, const char *file, const char *srvtab);

/*
 * Given a remctl object, the Kerberos context, the type for the wallet
 * interface, and a file name of a keytab, iterate through every existing
 * principal in the keytab in the local realm, get fresh keys for those
 * principals, and save the old and new keys to that file.  Returns true on
 * success and false on partial failure to retrieve all the keys.
 */
bool rekey_keytab(struct remctl *, krb5_context, const char *type,
                  const char *file);

/*
 * Given a filename, some data, and a length, write that data to the given
 * file with error checking, overwriting any existing contents.
 */
void overwrite_file(const char *name, const void *data, size_t length);

/*
 * Given a filename, some data, and a length, append that data to an existing
 * file.  Dies on any failure.
 */
void append_file(const char *name, const void *data, size_t length);

/*
 * Given a filename, some data, and a length, write that data to the given
 * file safely and atomically by creating file.new, writing the data, linking
 * file to file.bak, and then renaming file.new to file.
 */
void write_file(const char *name, const void *data, size_t length);

/*
 * Given a Kerberos context, a srvtab file, the Kerberos v5 principal, and the
 * keytab file, write a srvtab file for the corresponding Kerberos v4
 * principal.
 */
void write_srvtab(krb5_context, const char *srvtab, const char *principal,
                  const char *keytab);

/*
 * Read all of a file into memory and return the contents in newly allocated
 * memory.  Handles a file name of "-" to mean standard input.  Stores the
 * length of the data in the second argument if it isn't NULL.  Dies on any
 * failure.
 */
void *read_file(const char *, size_t *);

END_DECLS

#endif /* !CLIENT_INTERNAL_H */
