
#include <limits.h>
#include <stdint.h>

namespace bitop {

/* Return the number of bits of the data type.
 *
 * The return value is sizeof() * CHAR_BIT. On most platforms today, CHAR_BIT == 8.
 * CHAR_BIT could be other values if this is compiled on older mainframe platforms where
 * a 'char' is less or more than 8 bits.
 *
 * @return Number of bits in data type T
 */
template <typename T=unsigned int> static inline constexpr unsigned int type_bits(void) {
    return (unsigned int)sizeof(T) * (unsigned int)CHAR_BIT;
}

/* Return data type T with all bits 0
 *
 * The memory storage of such a value should show ALL bits set to 0.
 * 
 * On CPUs today that use signed 2's complement integers, it is expected that: allzero() - (T)1u == allones()
 * Subtracting 1 from all zeros should result in all ones.
 *
 * @return Type T with zero bits
 */
template <typename T=unsigned int> static inline constexpr T allzero(void) {
    return (T)0;
}

/* Return data type T with all bits 1
 *
 * The memory storage of such a value should show ALL bits set to 1.
 * 
 * On CPUs today that use signed 2's complement integers, it is expected that: allones() + (T)1u == allzero()
 * Adding 1 to all ones should result in all zeros.
 * 
 * @return Type T with set bits
 */
template <typename T=unsigned int> static inline constexpr T allones(void) {
    return (T)( ~((T)0) );
}


/* private recursion function */
template <typename T=unsigned int> static inline constexpr unsigned int _bitlength_recursion(const T v,const unsigned int bits) {
    return (v != allzero()) ? _bitlength_recursion(v >> (T)1u,bits + 1u) : bits;
}

/* Return minimum number of bits required to represent value 'v' in data type 'T'
 *
 * This function is declared constexpr so that the compiler can evaluate a constant value at compile
 * time and insert the result as constant into the code. Since constexpr cannot use if(), while() and
 * so on, recursion is used instead.
 *
 * @return Number of bits needed
 */
template <typename T=unsigned int,const T v> static inline constexpr unsigned int bitlength(void) {
    return _bitlength_recursion<T>(v,0);
}

template <const unsigned int v> static inline constexpr unsigned int bitlength(void) {
    return bitlength<unsigned int,v>();
}

/* non-constant version for runtime use */
template <typename T=unsigned int> static inline unsigned int bitlength(T v) {
    unsigned int c = 0;

    while ((v & 0xFFUL) == 0xFFUL) {
        v >>= (T)8UL;
        c += (T)8;
    }
    while (v != allzero<T>()) {
        v >>= (T)1UL;
        c++;
    }

    return c;
}


/* private recursion function */
template <typename T=unsigned int> static inline constexpr unsigned int _bitseqlength_recursionlsb(const T v,const unsigned int bits) {
    return (v & 1u) ? _bitseqlength_recursionlsb<T>(v >> (T)1u,bits + 1u) : bits;
}

/* private common function */
template <typename T=unsigned int> static inline void _bitseqlengthlsb_1(unsigned int &c,T &v) {
    while ((v & 0xFFUL) == 0xFFUL) {
        v >>= (T)8UL;
        c += (T)8;
    }
    while (v & 1u) {
        v >>= (T)1UL;
        c++;
    }
}

/* Return number of sequential 1 bits counting from LSB in value 'v' of type 'T'
 *
 * THis counts bits from the LSB upward until a zero bit is encountered.
 *
 * A constexpr version is provided for use at compile time and a non-constexpr
 * version is provided for use at runtime.
 *
 * @return Number of bits
 */
template <typename T=unsigned int,const T v> static inline constexpr unsigned int bitseqlengthlsb(void) {
    return _bitseqlength_recursionlsb(v,0);
}

template <const unsigned int v> static inline constexpr unsigned int bitseqlengthlsb(void) {
    return bitseqlengthlsb<unsigned int,v>();
}

template <typename T=unsigned int> static inline unsigned int bitseqlengthlsb(T v) {
    unsigned int c = 0;
    _bitseqlengthlsb_1(/*&*/c,/*&*/v);
    return c;
}


/* Return binary mask of bit 'a'
 *
 * The returned bitmask should be usable to mask off bit 'a' and only bit 'a'.
 *
 * The function is constexpr to enable compile-time evaluation.
 *
 * @return Bitmask
 */
template <const unsigned int a,typename T=unsigned int> static inline constexpr T bit2mask(void) {
    static_assert(a < type_bits<T>(), "bit2mask constexpr bit count too large for data type");
    return (T)1U << (T)a;
}

template <typename T=unsigned int> static inline constexpr T bit2mask(const unsigned int a) {
    return (T)1U << (T)a;
}


/* Return binary mask of MSB in type 'T'
 *
 * The returned bitmask should be usable to mask off the most significant bit of data type 'T'
 *
 * @return Bitmask
 */
template <typename T=unsigned int> static inline constexpr T type_msb_mask(void) {
    return bit2mask<T>(type_bits<T>() - 1u);
}


/* Return binary mask of 'a' LSB bits starting at bit offset 'offset' in type 'T'
 *
 * The returned bitmask should be usable to mask off 'a' bits starting from the bottom (LSB) at bit 'offset'.
 *
 * Examples:    bitcount2masklsb<1,0>() = 0x00000001U           a=1 offset=0        bits 0 to 0
 *              bitcount2masklsb<4,0>() = 0x0000000FU           a=4 offset=0        bits 3 to 0
 *              bitcount2masklsb<16,0>() = 0x0000FFFFU          a=16 offset=0       bits 15 to 0
 *              bitcount2masklsb<4,4>() = 0x000000F0U           a=4 offset=4        bits 7 to 4
 *
 * @return Bitmask
 */
template <const unsigned int a,const unsigned int offset,typename T=unsigned int> static inline constexpr T bitcount2masklsb(void) {
    /* NTS: special case for a == type_bits because shifting the size of a register OR LARGER is undefined.
     *      On Intel x86 processors, with 32-bit integers, x >> 32 == x >> 0 because only the low 5 bits are used 
     *      a % type_bits<T>() is there to keep a < type_bits<T> in case your C++11 compiler likes to trigger
     *      all static_assert<> clauses even if the ternary would not choose that path. */
    static_assert(a <= type_bits<T>(), "bitcount2masklsb constexpr bit count too large for data type");
    static_assert(offset < type_bits<T>(), "bitcount2masklsb constexpr offset count too large for data type");
    static_assert((a+offset) <= type_bits<T>(), "bitcount2masklsb constexpr bit count + offset count too large for data type");
    return ((a < type_bits<T>()) ? (bit2mask<a % type_bits<T>(),T>() - (T)1u) : allones<T>()) << (T)offset;
}

template <const unsigned int a,typename T=unsigned int> static inline constexpr T bitcount2masklsb(void) {
    return bitcount2masklsb<a,0u,T>();
}

template <typename T=unsigned int> static inline constexpr T bitcount2masklsb(const unsigned int a,const unsigned int offset=0) {
    /* NTS: special case for a == type_bits because shifting the size of a register OR LARGER is undefined.
     *      On Intel x86 processors, with 32-bit integers, x >> 32 == x >> 0 because only the low 5 bits are used */
    return ((a < type_bits<T>()) ? (bit2mask<T>(a) - (T)1u) : allones<T>()) << (T)offset;
}


/* Return binary mask of 'a' MSB bits starting at bit offset 'offset' in type 'T'
 *
 * The returned bitmask should be usable to mask off 'a' bits starting from the top (MSB) at bit 'offset'.
 *
 * Examples:    bitcount2maskmsb<1,0>() = 0x80000000U           a=1 offset=0        bits 31 to 31
 *              bitcount2maskmsb<4,0>() = 0xF0000000U           a=4 offset=0        bits 31 to 28
 *              bitcount2maskmsb<16,0>() = 0xFFFF0000U          a=16 offset=0       bits 31 to 16
 *              bitcount2maskmsb<4,4>() = 0x0F000000U           a=4 offset=4        bits 27 to 24
 *
 * @return Bitmask
 */
template <const unsigned int a,const unsigned int offset,typename T=unsigned int> static inline constexpr T bitcount2maskmsb(void) {
    /* NTS: special case for a == type_bits because shifting the size of a register OR LARGER is undefined.
     *      On Intel x86 processors, with 32-bit integers, x >> 32 == x >> 0 because only the low 5 bits are used 
     *      a % type_bits<T>() is there to keep a < type_bits<T> in case your C++11 compiler likes to trigger
     *      all static_assert<> clauses even if the ternary would not choose that path. */
    static_assert(a <= type_bits<T>(), "bitcount2maskmsb constexpr bit count too large for data type");
    static_assert(offset < type_bits<T>(), "bitcount2maskmsb constexpr offset count too large for data type");
    static_assert((a+offset) <= type_bits<T>(), "bitcount2maskmsb constexpr bit count + offset count too large for data type");
    return ((a != (T)0) ? ((T)(allones<T>() << (T)(type_bits<T>() - a)) >> (T)offset) : allzero<T>());
}

template <const unsigned int a,typename T=unsigned int> static inline constexpr T bitcount2maskmsb(void) {
    return bitcount2maskmsb<a,0u,T>();
}

template <typename T=unsigned int> static inline constexpr T bitcount2maskmsb(const unsigned int a,const unsigned int offset=0) {
    /* NTS: special case for a == type_bits because shifting the size of a register OR LARGER is undefined.
     *      On Intel x86 processors, with 32-bit integers, x >> 32 == x >> 0 because only the low 5 bits are used */
    return ((a != (T)0) ? ((T)(allones<T>() << (T)(type_bits<T>() - a)) >> (T)offset) : allzero<T>());
}


/* Indicate whether 'a' is a power of 2.
 *
 * This code will NOT work correctly if a == 0, the result is to be considered undefined.
 * Note that in real mathematics, there is no value x for which 2 to the power of 'x' equals zero.
 * But you might be able to plug infinity into x to get really close to zero.
 *
 * @return Boolean true if 'a' is a power of 2 */
template <typename T=unsigned int> static inline constexpr bool ispowerof2(const unsigned int a) {
    return (a & (a-(T)1u)) == 0;
}


/* Return log2(v), or the bit position of the highest '1' bit of value 'v'
 *
 * A value of '0' will return allones<unsigned int>() to indicate an invalid value.
 *
 * The constexpr templated version will trigger a static_assert if v == 0.
 *
 * log2(2^31) == 31
 * log2(2^30) == 30
 * log2(2^16) == 16                             2^16 == 65536
 * log2(2^4) == 4                               2^4 == 16
 * log2(2^1) == 1                               2^1 == 2
 * log2(2^0) == 0                               2^0 == 1
 * log2(0) == ~0u (UINT_MAX or allones()) 
 *
 * @return Bitmask
 */

/* private recursion function */
template <typename T=unsigned int> static inline constexpr unsigned int _log2_recursion(const T v,const unsigned int bits) {
    /* NTS: The additional check against v == allzero() is needed to avoid an infinite recursion loop */
    return (v != allzero<T>()) ?
        (((v & type_msb_mask<T>()) == allzero<T>()) ? _log2_recursion(v << (T)1u,bits - 1u) : bits) :
        allones<T>();
}

template <typename T=unsigned int,const T v> static inline constexpr unsigned int log2(void) {
    return _log2_recursion<T>(v,type_bits<T>() - 1u);
}

template <const unsigned int v> static inline constexpr unsigned int log2(void) {
    return log2<unsigned int,v>();
}

/* non-constant version for runtime use */
template <typename T=unsigned int> static inline unsigned int log2(T v) {
    if (v != allzero<T>()) {
        unsigned int c = type_bits<T>() - 1u;

        while (!(v & type_msb_mask<T>())) {
            v <<= (T)1UL;
            c--;
        }

        return c;
    }

    return ~0u;
}

void self_test(void);

}

