/*
 * Prototypes for vector handling.
 *
 * A vector is a list of strings, with dynamic resizing of the list as new
 * strings are added and support for various operations on strings (such as
 * splitting them on delimiters).
 *
 * Vectors require list of strings, not arbitrary binary data, and cannot
 * handle data elements containing nul characters.
 *
 * Written by Russ Allbery <rra@stanford.edu>
 *
 * The authors hereby relinquish any claim to any copyright that they may have
 * in this work, whether granted under contract or by operation of law or
 * international treaty, and hereby commit to the public, at large, that they
 * shall not, at any time in the future, seek to enforce any copyright in this
 * work against any person or entity, or prevent any person or entity from
 * copying, publishing, distributing or creating derivative works of this
 * work.
 */

#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H 1

#include <config.h>
#include <portable/macros.h>

#include <sys/types.h>

struct vector {
    size_t count;
    size_t allocated;
    char **strings;
};

struct cvector {
    size_t count;
    size_t allocated;
    const char **strings;
};

BEGIN_DECLS

/* Default to a hidden visibility for all util functions. */
#pragma GCC visibility push(hidden)

/* Create a new, empty vector. */
struct vector *vector_new(void)
    __attribute__((__malloc__));
struct cvector *cvector_new(void)
    __attribute__((__malloc__));

/* Add a string to a vector.  Resizes the vector if necessary. */
void vector_add(struct vector *, const char *string)
    __attribute__((__nonnull__));
void cvector_add(struct cvector *, const char *string)
    __attribute__((__nonnull__));

/* Add a counted string to a vector.  Only available for vectors. */
void vector_addn(struct vector *, const char *string, size_t length)
    __attribute__((__nonnull__));

/*
 * Resize the array of strings to hold size entries.  Saves reallocation work
 * in vector_add if it's known in advance how many entries there will be.
 */
void vector_resize(struct vector *, size_t size)
    __attribute__((__nonnull__));
void cvector_resize(struct cvector *, size_t size)
    __attribute__((__nonnull__));

/*
 * Reset the number of elements to zero, freeing all of the strings for a
 * regular vector, but not freeing the strings array (to cut down on memory
 * allocations if the vector will be reused).
 */
void vector_clear(struct vector *)
    __attribute__((__nonnull__));
void cvector_clear(struct cvector *)
    __attribute__((__nonnull__));

/* Free the vector and all resources allocated for it. */
void vector_free(struct vector *)
    __attribute__((__nonnull__));
void cvector_free(struct cvector *)
    __attribute__((__nonnull__));

/*
 * Split functions build a vector from a string.  vector_split splits on a
 * specified character, vector_split_multi splits on a set of characters, and
 * vector_split_space splits on any sequence of spaces or tabs (not any
 * sequence of whitespace, as just spaces or tabs is more useful).  The
 * cvector versions destructively modify the provided string in-place to
 * insert nul characters between the strings.  If the vector argument is NULL,
 * a new vector is allocated; otherwise, the provided one is reused.
 *
 * Empty strings will yield zero-length vectors.  Adjacent delimiters are
 * treated as a single delimiter by *_split_space and *_split_multi, but *not*
 * by *_split, so callers of *_split should be prepared for zero-length
 * strings in the vector.
 */
struct vector *vector_split(const char *string, char sep, struct vector *)
    __attribute__((__nonnull__(1)));
struct vector *vector_split_multi(const char *string, const char *seps,
                                  struct vector *)
    __attribute__((__nonnull__(1, 2)));
struct vector *vector_split_space(const char *string, struct vector *)
    __attribute__((__nonnull__(1)));
struct cvector *cvector_split(char *string, char sep, struct cvector *)
    __attribute__((__nonnull__(1)));
struct cvector *cvector_split_multi(char *string, const char *seps,
                                    struct cvector *)
    __attribute__((__nonnull__(1, 2)));
struct cvector *cvector_split_space(char *string, struct cvector *)
    __attribute__((__nonnull__(1)));

/*
 * Build a string from a vector by joining its components together with the
 * specified string as separator.  Returns a newly allocated string; caller is
 * responsible for freeing.
 */
char *vector_join(const struct vector *, const char *seperator)
    __attribute__((__malloc__, __nonnull__));
char *cvector_join(const struct cvector *, const char *separator)
    __attribute__((__malloc__, __nonnull__));

/*
 * Exec the given program with the vector as its arguments.  Return behavior
 * is the same as execv.  Note the argument order is different than the other
 * vector functions (but the same as execv).
 */
int vector_exec(const char *path, struct vector *)
    __attribute__((__nonnull__));
int cvector_exec(const char *path, struct cvector *)
    __attribute__((__nonnull__));

/* Undo default visibility change. */
#pragma GCC visibility pop

END_DECLS

#endif /* UTIL_VECTOR_H */