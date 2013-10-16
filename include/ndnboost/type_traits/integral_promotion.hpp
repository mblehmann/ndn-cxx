// Copyright 2005 Alexander Nasonov.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef FILE_ndnboost_type_traits_integral_promotion_hpp_INCLUDED
#define FILE_ndnboost_type_traits_integral_promotion_hpp_INCLUDED

#include <ndnboost/config.hpp>

#include <ndnboost/mpl/eval_if.hpp>
#include <ndnboost/mpl/identity.hpp>
#include <ndnboost/type_traits/integral_constant.hpp>
#include <ndnboost/type_traits/is_const.hpp>
#include <ndnboost/type_traits/is_enum.hpp>
#include <ndnboost/type_traits/is_volatile.hpp>
#include <ndnboost/type_traits/remove_cv.hpp>

// Should be the last #include
#include <ndnboost/type_traits/detail/type_trait_def.hpp>

namespace ndnboost {

namespace type_traits { namespace detail {

// 4.5/2
template <class T> struct need_promotion : public ndnboost::is_enum<T> {};

// 4.5/1
template<> struct need_promotion<char              > : public true_type {};
template<> struct need_promotion<signed char       > : public true_type {};
template<> struct need_promotion<unsigned char     > : public true_type {};
template<> struct need_promotion<signed short int  > : public true_type {};
template<> struct need_promotion<unsigned short int> : public true_type {};


// Specializations for non-standard types.
// Type is promoted if it's smaller then int.

#define NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE(T) \
    template<> struct need_promotion<T>          \
        : public integral_constant<bool, (sizeof(T) < sizeof(int))> {};

// Same set of integral types as in ndnboost/type_traits/is_integral.hpp.
// Please, keep in sync.
#if (defined(NDNBOOST_MSVC) && (NDNBOOST_MSVC < 1300)) \
    || (defined(NDNBOOST_INTEL_CXX_VERSION) && defined(_MSC_VER) && (NDNBOOST_INTEL_CXX_VERSION <= 600)) \
    || (defined(__BORLANDC__) && (__BORLANDC__ == 0x600) && (_MSC_VER < 1300))
// TODO: common macro for this #if. Or better yet, PP SEQ of non-standard types.
NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE(__int8          )
NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE(unsigned __int8 )
NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE(__int16         )
NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE(unsigned __int16)
NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE(__int32         )
NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE(unsigned __int32)
#ifdef __BORLANDC__
NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE(unsigned __int64)
NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE(         __int64)
#endif
#endif

#if defined(NDNBOOST_HAS_LONG_LONG)
NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE(ndnboost::ulong_long_type)
NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE(ndnboost::long_long_type )
#elif defined(NDNBOOST_HAS_MS_INT64)
NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE(unsigned __int64)
NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE(         __int64)
#endif

#undef NDNBOOST_TT_AUX_PROMOTE_NONSTANDARD_TYPE


#ifndef NDNBOOST_NO_INTRINSIC_WCHAR_T
// 4.5/2
template<> struct need_promotion<wchar_t> : public true_type {};
#endif

// 4.5/3 (integral bit-field) is not supported.

// 4.5/4
template<> struct need_promotion<bool> : public true_type {};


// Get promoted type by index and cv qualifiers.

template<int Index, int IsConst, int IsVolatile> struct promote_from_index;

#define NDNBOOST_TT_AUX_PROMOTE_FROM_INDEX(N,T)                                   \
    template<> struct promote_from_index<N,0,0> { typedef T type; };           \
    template<> struct promote_from_index<N,0,1> { typedef T volatile type; };  \
    template<> struct promote_from_index<N,1,0> { typedef T const type; };     \
    template<> struct promote_from_index<N,1,1> { typedef T const volatile type; };


NDNBOOST_TT_AUX_PROMOTE_FROM_INDEX(1, int          )
NDNBOOST_TT_AUX_PROMOTE_FROM_INDEX(2, unsigned int )
NDNBOOST_TT_AUX_PROMOTE_FROM_INDEX(3, long         )
NDNBOOST_TT_AUX_PROMOTE_FROM_INDEX(4, unsigned long)


// WARNING: integral promotions to non-standard types
// long long and __int64 are not defined by the standard.
// Additional specialisations and overloads shouldn't
// introduce ambiguity, though.

#if defined(NDNBOOST_HAS_LONG_LONG)
NDNBOOST_TT_AUX_PROMOTE_FROM_INDEX(5, ndnboost::long_long_type )
NDNBOOST_TT_AUX_PROMOTE_FROM_INDEX(6, ndnboost::ulong_long_type)
#elif defined(NDNBOOST_HAS_MS_INT64)
NDNBOOST_TT_AUX_PROMOTE_FROM_INDEX(7, __int64         )
NDNBOOST_TT_AUX_PROMOTE_FROM_INDEX(8, unsigned __int64)
#endif

#undef NDNBOOST_TT_AUX_PROMOTE_FROM_INDEX


// Define NDNBOOST_TT_AUX_PROMOTED_INDEX_TESTER:
#if !defined(NDNBOOST_MSVC)

template<int N>
struct sized_type_for_promotion
{
    typedef char (&type)[N];
};

#define NDNBOOST_TT_AUX_PROMOTED_INDEX_TESTER(I,T) \
    sized_type_for_promotion<I>::type promoted_index_tester(T);

#else

#define NDNBOOST_TT_AUX_PROMOTED_INDEX_TESTER(I,T) \
    char (&promoted_index_tester(T))[I];

#endif

NDNBOOST_TT_AUX_PROMOTED_INDEX_TESTER(1, int          )
NDNBOOST_TT_AUX_PROMOTED_INDEX_TESTER(2, unsigned int )
NDNBOOST_TT_AUX_PROMOTED_INDEX_TESTER(3, long         )
NDNBOOST_TT_AUX_PROMOTED_INDEX_TESTER(4, unsigned long)

#if defined(NDNBOOST_HAS_LONG_LONG)
NDNBOOST_TT_AUX_PROMOTED_INDEX_TESTER(5, ndnboost::long_long_type )
NDNBOOST_TT_AUX_PROMOTED_INDEX_TESTER(6, ndnboost::ulong_long_type)
#elif defined(NDNBOOST_HAS_MS_INT64)
NDNBOOST_TT_AUX_PROMOTED_INDEX_TESTER(7, __int64         )
NDNBOOST_TT_AUX_PROMOTED_INDEX_TESTER(8, unsigned __int64)
#endif

#undef NDNBOOST_TT_AUX_PROMOTED_INDEX_TESTER


// Get an index of promoted type for type T.
// Precondition: need_promotion<T>
template<class T>
struct promoted_index
{
    static T testee; // undefined
    NDNBOOST_STATIC_CONSTANT(int, value = sizeof(promoted_index_tester(+testee)) );
    // Unary plus promotes testee                    LOOK HERE ---> ^
};

template<class T>
struct integral_promotion_impl
{
    typedef NDNBOOST_DEDUCED_TYPENAME promote_from_index<
        (ndnboost::type_traits::detail::promoted_index<T>::value)
      , (ndnboost::is_const<T>::value)
      , (ndnboost::is_volatile<T>::value)
      >::type type;
};

template<class T>
struct integral_promotion
  : public ndnboost::mpl::eval_if<
        need_promotion<NDNBOOST_DEDUCED_TYPENAME remove_cv<T>::type>
      , integral_promotion_impl<T>
      , ndnboost::mpl::identity<T>
      >
{
};

} }

NDNBOOST_TT_AUX_TYPE_TRAIT_DEF1(
      integral_promotion
    , T
    , NDNBOOST_DEDUCED_TYPENAME
        ndnboost::type_traits::detail::integral_promotion<T>::type
    )
}

#include <ndnboost/type_traits/detail/type_trait_undef.hpp>

#endif // #ifndef FILE_ndnboost_type_traits_integral_promotion_hpp_INCLUDED

