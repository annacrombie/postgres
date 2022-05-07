/*
 * base64.h
 *	  Encoding and decoding routines for base64 without whitespace
 *	  support.
 *
 * Portions Copyright (c) 2001-2022, PostgreSQL Global Development Group
 *
 * src/include/common/base64.h
 */
#ifndef BASE64_H
#define BASE64_H

/* base 64 */
extern PGDLLIMPORT int	pg_b64_encode(const char *src, int len, char *dst, int dstlen);
extern PGDLLIMPORT int	pg_b64_decode(const char *src, int len, char *dst, int dstlen);
extern PGDLLIMPORT int	pg_b64_enc_len(int srclen);
extern PGDLLIMPORT int	pg_b64_dec_len(int srclen);

#endif							/* BASE64_H */
