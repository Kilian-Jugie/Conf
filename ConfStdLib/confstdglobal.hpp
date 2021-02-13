#pragma once
#include <string>
#include <concepts>

/*
 * Configuration macros :
 * CONF_STD_NO_UNICODE : remove unicode support from confstd
 *
 * Feature macros          | Since version
 * CONF_STD_CHAR_SUPPORT   | 0.0.1
 * CONF_STD_VERLIB_SUPPORT | 0.0.1
 * 
 * Exported macros         | Description
 * CONF_STD_VER            | Version of confstd lib
 * CONF_EMPTY              | Empty
 * CONF_DEFER_N            | Defer expression N times
 * CONF_EVAL[_N]           | Eval expression N times or 64 if not precised
 * CONF_STR                | Stringify expression
 * CONF_CAT                | Concatenate tokens
 * CONF_STD_VERLIB_INT     | Convert version as int (for comparisons)
 * CONF_STD_VERLIB_DOT     | Convert version as dot separated expression
 * CONF_STD_VERLIB_STR     | Convert version as human redeable string
 */

/* Support for unicode may be implemented in a future release depending
 * of the needs to implement a such technology due to its performance cost
 * and the work to inject into for nearly no real advantages for now
 */
#define CONF_STD_NO_UNICODE

namespace confstd {
/* Preprocessor helpers */
#define CONF_EMPTY()
#define CONF_DEFER_1(...) __VA_ARGS__ CONF_EMPTY ()
#define CONF_DEFER_2(...) __VA_ARGS__ CONF_EMPTY CONF_EMPTY ()()
#define CONF_DEFER_3(...) __VA_ARGS__ CONF_EMPTY CONF_EMPTY CONF_EMPTY ()()()

#define CONF_EVAL_1(...) __VA_ARGS__
#define CONF_EVAL_2(...) CONF_EVAL_1(CONF_EVAL_1(__VA_ARGS__))
#define CONF_EVAL_4(...) CONF_EVAL_2(CONF_EVAL_2(__VA_ARGS__))
#define CONF_EVAL_8(...) CONF_EVAL_4(CONF_EVAL_4(__VA_ARGS__))
#define CONF_EVAL_16(...) CONF_EVAL_8(CONF_EVAL_8(__VA_ARGS__))
#define CONF_EVAL_32(...) CONF_EVAL_16(CONF_EVAL_16(__VA_ARGS__))
#define CONF_EVAL_64(...) CONF_EVAL_32(CONF_EVAL_32(__VA_ARGS__))
#define CONF_EVAL(...) CONF_EVAL_64(__VA_ARGS__)

#define _CONF_SKIP_2(_1,_2,...)__VA_ARGS__
#define _CONF_SKIP(_, ...) __VA_ARGS__
#define _CONF_SEC(_, sec, ...) sec
#define _CONF_FIRST(first, ...) first

#define _CONF_CAT(x, y) x##y
#define _CONF_STR(x) #x

#define CONF_STR(x) _CONF_STR(x)
#define CONF_CAT(x, y) _CONF_CAT(x, y)

#define _CONF_STD_VERLIB_INT(maj, min, rev) CONF_CAT(maj, CONF_CAT(min, rev))
#define _CONF_STD_VERLIB_DOT(maj, min, rev) CONF_CAT(maj, CONF_CAT(., CONF_CAT(min, CONF_CAT(., rev))))
#define _CONF_STD_VERLIB_STR(maj, min, rev) CONF_STR(_CONF_STD_VERLIB_DOT(maj, min, rev))

#define CONF_STD_VERLIB_INT(...) CONF_EVAL(_CONF_STD_VERLIB_INT(__VA_ARGS__))
#define CONF_STR_VERLIB_DOT(...) CONF_EVAL(_CONF_STD_VERLIB_DOT(__VA_ARGS__))
#define CONF_STD_VERLIB_STR(...) CONF_EVAL(_CONF_STD_VERLIB_STR(__VA_ARGS__))

// constini and consteval not yet implemented in msvc
#define __conf_constini constexpr
#define __conf_consteval constexpr

#ifndef CONF_STD_VER
#define CONF_STD_VER 0,0,1
#endif // !CONF_STD_VER

#if defined(UNICODE) && !defined(CONF_STD_NO_UNICODE)
#define CONF_STD_UNICODE
#endif

#ifdef CONF_STD_UNICODE
  using char_t = wchar_t;
#else
  using char_t = char;
#endif

  using string_t = std::basic_string<char_t>;
  using string_view_t = std::basic_string_view<char_t>;

  std::string wstr_str(const std::wstring& source);
  std::wstring str_wtr(const std::string& source);

  // This must be not defined to help debugging thanks to link symbols
  // If compiler tries to call this, a specialization is missing
  // Warning: Here, to_string is basically a litteral or wiseness converter
  // so if a specific class needs its own specialization, it must
  // be done in an adapted header !
  template<typename _T>__forceinline string_t to_string(_T elem);

  template<typename _T>concept StdStringConstructible = requires(_T v) {
    { std::string{v} };
  };

  template<typename _T>concept StdWStringConstructible = requires(_T v) {
    { std::wstring{v} };
  };

  template<typename _T>concept StringConstructible = requires(_T v) {
    { string_t{v} };
  };

#ifdef CONF_STD_UNICODE
  template<typename _T>concept StringConvertible = requires(_T v) {
    { std::to_wstring(v) } -> std::same_as<string_t>;
  };

  template<StringConvertible _T>__forceinline string_t to_string(_T elem) {
    return std::to_wstring(elem);
  }

  template<StdStringConstructible _T>__forceinline string_t to_string(_T&& ws) {
    return str_wstr(std::forward<_T>(ws));
  }
#else
  template<typename _T>concept StringConvertible = requires(_T v) {
    { std::to_string(v) } -> std::same_as<string_t>;
  };

  template<StringConvertible _T>__forceinline string_t to_string(_T elem) {
    return std::to_string(elem);
  }

  template<StdWStringConstructible _T>__forceinline string_t to_string(_T&& ws) {
    return wstr_str(std::forward<_T>(ws));
  }
#endif

  template<>__forceinline string_t to_string(const char_t* ch) {
    return string_t{ch};
  }

  // Is NRVO working on parameters and parameter of this kind of function ?
  template<>__forceinline string_t to_string(string_t ch) {
    return ch;
  }

  template<>__forceinline string_t to_string(string_view_t&& ch) {
    return string_t{std::move(ch)};
  }

  template<>__forceinline string_t to_string(string_view_t ch) {
    return string_t{ch};
  }

#define CONF_STD_CHAR_SUPPORT
#define CONF_STD_VERLIB_SUPPORT
}