#ifndef SSCE_NC_HASH_H
#define SSCE_NC_HASH_H
/**
 * @file
 * @brief Methods about finding the maximum or minimum from a set of integers.
 */

#include <Macros.h>

#include <stddef.h>

/**
 * Calculates a non cryptographic hash for some data.
 * Note that the returned hash may differ based on cpu architecture.
 * 
 * @param data pointer to message.
 * @param length how many bytes is the message?
 * @returns the calculated hash.
 */
EXPORT_API size_t ncrypto_hash(const void* data, size_t length);

#endif /*SSCE_NC_HASH_H*/