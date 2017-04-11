/*
    BadStaticAssert.h

    A workaround for the Arduino IDE not supporting c++0x `static_assert`.
*/
#ifndef BadStaticAssert_h_
#define BadStaticAssert_h_

#ifndef STATIC_ASSERT
    /* ripped from
     * http://www.pixelbeat.org/programming/gcc/static_assert.html
     */
    #ifndef static_assert
        #define ASSERT_CONCAT_(a, b) a##b
        #define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
        /* These can't be used after statements in c89. */
        #ifdef __COUNTER__
          #define STATIC_ASSERT(e,m) \
            ;enum { ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(int)(!!(e)) }
        #else
          /* This can't be used twice on the same line so ensure if using in
           * header that the headers are not included twice (by wrapping in
           * #ifndef...#endif)
           * Note it doesn't cause an issue when used on same line of separate
           * modules compiled with gcc -combine -fwhole-program.  */
          #define STATIC_ASSERT(e, m) \
            ;enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(int)(!!(e)) }
        #endif
    #else
        #define STATIC_ASSERT(e, m) static_assert(e, m)
    #endif

    /* http://msdn.microsoft.com/en-us/library/ms679289(VS.85).aspx */
    #ifndef C_ASSERT
    #define C_ASSERT(e) STATIC_ASSERT(e)
    #endif

#endif

#endif
