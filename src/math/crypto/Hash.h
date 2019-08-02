#ifndef SSCE_NC_HASH_H
#define SSCE_NC_HASH_H
/**
 * @file
 * @brief Methods about finding the maximum or minimum from a set of integers.
 */

#include <Macros.h>

#include <stddef.h>
#include <stdint.h>

#if __SIZE_WIDTH__==64
    /**
     * Calculates a non cryptographic hash for some data.
     * The used hash algorithm depends on cpu architecture.
     * Note that the returned hash may differ based on cpu architecture.
     * 
     * @param data pointer to message.
     * @param length how many bytes is the message?
     * @returns the calculated hash.
     */
    #define ncrypto_native_hash(data, length) ncrypto_spooky64(data, length, 0)
#elif __SIZE_WIDTH__==32
    /**
     * Calculates a non cryptographic hash for some data.
     * The used hash algorithm depends on cpu architecture.
     * Note that the returned hash may differ based on cpu architecture.
     * 
     * @param data pointer to message.
     * @param length how many bytes is the message?
     * @returns the calculated hash.
     */
    #define ncrypto_native_hash(data, length) ncrypto_xxhash32(data, length, 0)
#else
    #warning No native hash function for this processor!
#endif


#ifdef INT128_SUPPORTED
    /**
     * SpookyHash implementation.
     * Only available on >=64bit platforms.
     * 
     * @param data pointer to message.
     * @param length how many bytes is the message?
     * @returns the calculated hash.
     */
    EXPORT_API uint128_t ncrypto_spooky128(const void* data, size_t length, uint128_t seed) MARK_NONNULL_ARGS(1);
#endif

/**
 * SpookyHash implementation.
 * This only returns the lower 64 bits of the hash.
 * 
 * @param data pointer to message.
 * @param length how many bytes is the message?
 * @returns the calculated hash.
 */
EXPORT_API uint64_t ncrypto_spooky64(const void* data, size_t length, uint64_t seed) MARK_NONNULL_ARGS(1);

/**
 * XXHash32 implementation.
 * 
 * @param data pointer to message.
 * @param length how many bytes is the message?
 * @returns the calculated hash.
 */
EXPORT_API uint32_t ncrypto_xxhash32(const void* data, size_t length, uint32_t seed) MARK_NONNULL_ARGS(1);

#endif /*SSCE_NC_HASH_H*/