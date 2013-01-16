/*
 * Copyright 2010-2013 Esrille Inc.
 *
 * One-at-a-Time Hash by Bob Jenkins:
 *
 * cf. http://burtleburtle.net/bob/hash/doobs.html
 *
 * The code given here are all public domain.
 *
 *     ub4 one_at_a_time(char *key, ub4 len)
 *     {
 *         ub4 hash, i;
 *         for (hash=0, i=0; i<len; ++i)
 *         {
 *             hash += key[i];
 *             hash += (hash << 10);
 *             hash ^= (hash >> 6);
 *         }
 *         hash += (hash << 3);
 *         hash ^= (hash >> 11);
 *         hash += (hash << 15);
 *         return (hash & mask);
 *     }
 *
 * This is similar to the rotating hash, but it actually mixes the internal
 * state. It takes 9n+9 instructions and produces a full 4-byte result.
 * Preliminary analysis suggests there are no funnels.
 *
 * This hash was not in the original Dr. Dobb's article. I implemented it to
 * fill a set of requirements posed by Colin Plumb. Colin ended up using an
 * even simpler (and weaker) hash that was sufficient for his purpose.
 */

#ifndef ES_ONE_AT_A_TIME_H_INCLUDED
#define ES_ONE_AT_A_TIME_H_INCLUDED

#include <cstdint>

namespace one_at_a_time {

constexpr std::uint32_t mix(std::uint32_t hash)
{
    return (hash + (hash << 10)) ^ ((hash + (hash << 10)) >> 6);
}

constexpr std::uint32_t subpostprocess(std::uint32_t hash)
{
    return (hash + (hash << 3)) ^ ((hash + (hash << 3)) >> 11);
}

constexpr std::uint32_t postprocess(std::uint32_t hash)
{
    return subpostprocess(hash) + (subpostprocess(hash) << 15);
}

template <typename T>
constexpr std::uint32_t combine(std::uint32_t hash, const T* s)
{
    return *s ? combine(mix(hash + *s), s + 1) : hash;
}

template <typename T>
constexpr std::uint32_t hash(const T* s)
{
    return postprocess(combine(0, s));
}

} // one_at_a_time

#endif  // ES_ONE_AT_A_TIME_H_INCLUDED
