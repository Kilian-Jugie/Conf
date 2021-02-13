#pragma once
//#include "confstdconcepts.hpp"
#include "confstdglobal.hpp"
#include "confstdconcepts.hpp"
#include <vector>
#include <string>

namespace confstd {
	/// <summary>
	/// Returns the last element of C-style array
	/// </summary>
	/// <typeparam name="_Ty">The type of the array</typeparam>
	/// <param name="c">The array from where to find the element</param>
	/// <returns>A reference to the last element of the array</returns>
	template<typename _Ty, size_t N>constexpr _Ty& last(_Ty(&c)[N]) {
		static_assert(N > 0);
		return c[N - 1];
	}

	/// <summary>
	/// Returns the last element of C++ iterable container
	/// </summary>
	/// <typeparam name="_C">The type of the container</typeparam>
	/// <param name="cont">The container from where to find the element</param>
	/// <returns>A reference to the last element of the container</returns>
	template<concepts::containers::Iterable _C>
	constexpr typename _C::value_type& last(_C& cont) {
		return *(cont.end()-1);
	}

	/// <summary>
	/// Returns the first element of C-style array
	/// </summary>
	/// <typeparam name="_Ty">The type of the array</typeparam>
	/// <param name="c">The array from where to find the element</param>
	/// <returns>A reference to the first element of the array</returns>
	template<typename _Ty, size_t N>constexpr _Ty& first(_Ty(&c)[N]) {
		static_assert(N > 0);
		return c[0];
	}

	/// <summary>
	/// Returns the first element of C++ iterable container
	/// </summary>
	/// <typeparam name="_C">The type of the container</typeparam>
	/// <param name="cont">The container from where to find the element</param>
	/// <returns>A reference to the first element of the container</returns>
	template<concepts::containers::Iterable _C>
	constexpr typename _C::value_type& first(_C& cont) {
		return *cont.begin();
	}

	/// <summary>
	/// Split string on each separator in an array containing copy of splitted parts.
	/// If keepChar directly preceed the separator, the separator is kept in an
	/// independant part.
	/// </summary>
	/// <param name="str">The string to split</param>
	/// <param name="sperators">The separators where to split</param>
	/// <param name="keepChar">Character to signal we want to keep separator</param>
	/// <returns>An array of copied splitted parts</returns>
	std::vector<string_t> split(string_view_t str, const string_t& sperators, char_t keepChar = '^');
	template<concepts::Callable<std::tuple<bool, bool>, const char_t&> _FncTy> 
	std::vector<string_t> split(string_view_t str, const _FncTy& fnc) {
		std::vector<string_t> ret{""};
		for(const auto& it : str) {
			const auto&& [sep,keep] = fnc(str);
			if(sep && !last(ret).empty())
				ret.push_back("");
			if(keep) {
				last(ret) += it;
				ret.push_back("");
			}
		}
		return ret;
	}
	
	/// <summary>
	/// Split array by elems which follows a predicate as read only array of 
	/// iterators of splitted parts. Original array is not modified and only 
	/// position of parts is copied.
	/// </summary>
	/// <remarks>
	/// 1 - cont is not perfect forwarded because the aim of this function is
	///			to create references to a persistent memory even after the call.
	/// 2 - 
	/// <param name="cont">The container to split</param>
	/// <param name="fnc">The predicate where true = split</param>
	/// <returns>An array of iterators of begin and end of each splitted parts</returns>
	template<concepts::Iterable _ConTy, concepts::Callable<
		bool, const typename _ConTy::const_iterator&> _FncTy>
		decltype(auto) it_split(const _ConTy& cont, _FncTy&& fnc) {
		const auto end{cont.cend()};
		std::vector<std::tuple<typename _ConTy::const_iterator,
			typename _ConTy::const_iterator>> ret{{cont.cbegin(), end}};
		for(auto it{cont.cbegin()}; it != end; ++it) {
			if(fnc(it)) {
				std::get<1>(last(ret)) = it++;
				ret.push_back({it, end});
			}
		}
		return ret;
	}

	/// <summary>
	/// Do same thing as it_split but construct each iterator pair as
	/// a defined type which can construct from 2 iterators
	/// </summary>
	/// <typeparam name="_AsTy">The type to what construct</typeparam>
	/// <param name="cont">The container to split</param>
	/// <param name="fnc">The predicate where true = split</param>
	/// <returns>An array of newly constructed objects from splitted iterators</returns>
	template<typename _AsTy, concepts::Iterable _ConTy,
		concepts::Callable<bool, const typename _ConTy::const_iterator&> _FncTy>
		decltype(auto) it_split_as(const _ConTy& cont, _FncTy&& fnc)
		requires requires(const typename _ConTy::const_iterator&a, 
			const typename _ConTy::const_iterator&b) { _AsTy{a, b};	}
	{
		const auto end{cont.cend()};
		auto lbeg{cont.cbegin()};
		std::vector<_AsTy> ret;
		for(auto it{cont.cbegin()}; it != end; ++it) {
			if(fnc(it)) {
				ret.push_back(_AsTy{lbeg, it});
				lbeg = ++it;
			}
		}
		ret.push_back(_AsTy{lbeg, end});
		return ret;
	}

	/// <summary>
	/// Remove leading and trailing spaces
	/// </summary>
	/// <param name="str">The string to trim</param>
	void trim(string_t& str);

	/// <summary>
	/// Convert tabs in unique space and compress multiple spaces as one
	/// </summary>
	/// <param name="str">The string to press</param>
	void press(string_t& str);

	/// <summary>
	/// Returns the length of a C-style array
	/// </summary>
	/// <typeparam name="_T">The type of the array</typeparam>
	/// <param name="">The array</param>
	/// <returns>The length of the array</returns>
	template<typename _T, std::size_t N>
	__conf_consteval std::size_t length(_T(&)[N]) {
		return N;
	}

	void strreplace(string_t& src, string_view_t what, string_view_t by);
	
	template<concepts::Callable<bool, std::size_t> _FncTy>
	void strreplace(string_t& src, string_view_t what, string_view_t by, _FncTy _predicate) {
		std::size_t pos{0U};
		while((pos = src.find(what, pos)) != string_t::npos) {
			if(_predicate(pos)) {
				src.replace(pos, what.size(), by);
				pos += by.size();
			}
			else
				pos += what.size();
		}
	}
}