#pragma once

/*
 *		          /!\ WARNING /!\
 * This library is HIGHLY experimental and its use
 * should be MODERATE at least until C++20 is fully
 * correctly implemented in compiler.
 */

#include <concepts>
#include <type_traits>

namespace confstd {
	namespace concepts {
		template<typename _T>concept ExplicitCharType = std::disjunction_v<
			std::is_same<_T, char>, std::is_same<_T, wchar_t>,
			std::is_same<_T, char8_t>, std::is_same<_T, char16_t>,
			std::is_same<_T, char32_t>>;

		template<typename _T>concept ExplicitSizeType = std::is_same_v<_T, std::size_t>;

		template<typename _T >concept ExplicitBoolType = std::is_same_v<_T, bool>;

		template<typename _T>concept CharType = ExplicitCharType<std::decay_t<_T>>;
		template<typename _T>concept SizeType = ExplicitSizeType<std::decay_t<_T>>;
		template<typename _T>concept BoolType = ExplicitBoolType<std::decay_t<_T>>;

		template<typename _T, typename _S>concept SameType = std::is_same_v<_T, _S>;

		template<typename _T>concept Iterator = std::_Is_iterator_v<_T>;

		template<typename _T, typename _F, typename _S>concept PairBase = requires(_T a) {
			{ a.first } -> SameType<_F>;
			{ a.second } -> SameType<_S>;
		};

		template<typename _T>concept Pair = requires(_T) {
			typename _T::first_type;
			typename _T::second_type;
		} && PairBase< _T, typename _T::first_type, typename _T::second_type>;

		template<typename _T, typename _IT = int>concept OperatorIndexable = requires(_T a) { { a[_IT()] }; };
		template<typename _T, typename _IT = int>concept AtIndexable = requires(_T a) { { a.at(_IT()) }; };
		template<typename _T, typename _IT = int>concept AnyIndexable = OperatorIndexable<_T, _IT> || AtIndexable<_T, _IT>;
		template<typename _T, typename _IT = int>concept StdIndexable = OperatorIndexable<_T, _IT> && AtIndexable<_T, _IT>;

		template<typename _T>concept WithTypeValueType = requires(_T) { typename _T::value_type; };
		template<typename _T>concept WithTypeIterator = requires(_T) { typename _T::iterator; };
		template<typename _T>concept WithTypeConstIterator = requires(_T) { typename _T::const_iterator; };
		template<typename _T>concept WithIterators = WithTypeIterator<_T> && WithTypeConstIterator<_T>;
		template<typename _T>concept WithTypeReference = requires(_T) { typename _T::reference;	};
		template<typename _T>concept WithTypeConstReference = requires(_T) { typename _T::const_reference;	};
		template<typename _T>concept WithReferences = WithTypeReference<_T> && WithTypeConstReference<_T>;
		template<typename _T>concept WithTypeSizeType = requires(_T) { typename _T::size_type; };
		template<typename _T>concept WithTypeKeyType = requires(_T) { typename _T::key_type; };

		namespace containers {
			template<typename _T>concept WithOperatorCopy = requires(_T a) { { a.operator=(_T()) }; };
			template<typename _T>concept WithAssign = WithTypeValueType<_T> && WithIterators<_T> &&
				requires(_T a, size_t i, typename _T::value_type v, typename _T::const_iterator it, std::initializer_list<typename _T::value_type> il) {
				a.assign(i, v);
				a.assign(it, it);
				a.assign(il);
			};
			template<typename _T>concept WithAt = WithReferences<_T> && WithTypeSizeType<_T> &&
				requires(_T a, const _T ca, typename _T::size_type p) {
				{ a.at(p) } -> SameType<typename _T::reference>;
				{ ca.at(p) } -> SameType<typename _T::const_reference>;
			};
			template<typename _T>concept WithOperatorIndex = WithReferences<_T> && WithTypeSizeType<_T> &&
				requires(_T a, const _T ca, typename _T::size_type p) {
				{ a[p] } -> SameType<typename _T::reference>;
				{ ca[p] } -> SameType<typename _T::const_reference>;
			};
			template<typename _T>concept WithData = WithTypeValueType<_T> && requires(_T a, const _T ca) {
				{ a.data() } noexcept -> SameType<typename _T::value_type*>;
				{ ca.data() } noexcept -> SameType<const typename _T::value_type*>;
			};
			template<typename _T>concept WithFront = WithReferences<_T> && requires(_T a, const _T ca) {
				{ a.front() } -> SameType<typename _T::_reference>;
				{ ca.front() } -> SameType<typename _T::const_reference>;
			};
			template<typename _T>concept WithBack = WithReferences<_T> && requires(_T a, const _T ca) {
				{ a.back() } -> SameType<typename _T::reference>;
				{ ca.back() } -> SameType<typename _T::const_reference>;
			};
			template<typename _T>concept WithEmpty = requires(const _T a) { 
				{ a.empty() } noexcept -> SameType<bool>; 
			};
			template<typename _T>concept WithSize = WithTypeSizeType<_T> && requires(_T a) { 
				{ a.size() } noexcept -> SameType<typename _T::size_type>; 
			};
			template<typename _T>concept WithMaxSize = WithTypeSizeType<_T> && requires(_T a) {
				{ a.max_size() } noexcept -> SameType<typename _T::size_type>; 
			};
			template<typename _T>concept WithResize = WithTypeSizeType<_T> && WithTypeValueType<_T> &&
				requires(_T a, typename _T::size_type s, const typename _T::value_type& v) { //why not _T::const_reference ? 
				a.resize(s);
				a.resize(s, v);
			};
			template<typename _T>concept WithCapacity = WithTypeValueType<_T> && requires(const _T ca) {
				{ ca.capacity() } noexcept -> SameType<typename _T::size_type>;
			};
			template<typename _T>concept WithReserve = WithTypeSizeType<_T> && 
				requires(_T a, typename _T::size_type s) { 
				a.reserve(s);
			};
			template<typename _T>concept WithShrinkToFit = requires(_T a) { a.shrink_to_fit(); };
			template<typename _T>concept WithClear = requires(_T a) { { a.clear() } noexcept; };
			template<typename _T>concept WithInsert = WithIterators<_T> && WithTypeValueType<_T> &&
				WithTypeSizeType<_T> &&  requires(_T a, typename _T::const_iterator cit,
					typename _T::iterator it, const typename _T::value_type& cref, 
					typename _T::value_type&& rv, typename _T::size_type p, Iterator auto oit, 
					std::initializer_list<typename _T::value_type> il) {
				{ a.insert(cit, cref) } -> SameType<typename _T::iterator>;
				{ a.insert(cit, rv) } -> SameType<typename _T::iterator>;
				{ a.insert(cit, p, cref) } -> SameType<typename _T::iterator>;
				{ a.insert(cit, oit, oit) } -> SameType<typename _T::iterator>;
				{ a.insert(cit, il) } -> SameType<typename _T::iterator>;
			};
			template<typename _T>concept WithInsertOrAssign = WithTypeKeyType<_T> && WithIterators<_T> &&
				requires(_T a, auto&& obj, typename _T::key_type& cref, typename _T::key_type&& rv,
					typename _T::const_iterator cit) {
				{ a.insert_or_assign(cref, obj) } -> Pair<typename _T::iterator, bool>; 
				{ a.insert_or_assign(rv, obj) } -> Pair<typename _T::iterator, bool>;
				{ a.insert_or_assign(cit, cref, obj) } -> SameType<typename _T::iterator>;
				{ a.insert_or_assign(cit, rv, obj) } -> SameType<typename _T::iterator>;
			};
			template<typename _T>concept WithEmplace = requires(_T a) { { a.emplace }; };
			template<typename _T>concept WithEmplaceHint = requires(_T a) { { a.emplace_hint }; };
			template<typename _T>concept WithTryEmplace = requires(_T a) { { a.try_emplace }; };
			template<typename _T>concept WithErase = WithIterators<_T> && requires(_T a, 
				typename _T::iterator it, typename _T::const_iterator cit) { 
					{ a.erase(it) } -> SameType<typename _T::iterator>;
					{ a.erase(cit) } -> SameType<typename _T::iterator>;
					{ a.erase(it, it) } -> SameType<typename _T::iterator>;
					{ a.erase(cit, cit) } -> SameType<typename _T::iterator>;
			};
			template<typename _T>concept WithPushFront = requires(_T a) { { a.push_front }; };
			template<typename _T>concept WithEmplaceFront = requires(_T a) { { a.emplace_front }; };
			template<typename _T>concept WithPopFront = requires(_T a) { { a.pop_front }; };
			template<typename _T>concept WithPushBack = WithTypeKeyType<_T> && 
				requires(_T a, const typename _T::key_type& cref, typename _T::key_type&& rvref) {
					{ a.push_back(cref) };
					{ a.push_back(rvref) };
			};
			template<typename _T>concept WithEmplaceBack = requires(_T a) { { a.emplace_back }; };
			template<typename _T>concept WithPopBack = requires(_T a) { { a.pop_back }; };
			template<typename _T>concept WithSwap = requires(_T a) { { a.swap }; };
			template<typename _T>concept WithMerge = requires(_T a) { { a.merge }; };
			template<typename _T>concept WithExtract = requires(_T a) { { a.extract }; };
			template<typename _T>concept WithSplice = requires(_T a) { { a.splice }; };
			template<typename _T>concept WithRemove = requires(_T a) { { a.remove }; };
			template<typename _T>concept WithRemoveIf = requires(_T a) { { a.remove_if }; };
			template<typename _T>concept WithReverse = requires(_T a) { { a.reverse }; };
			template<typename _T>concept WithUnique = requires(_T a) { { a.unique }; };
			template<typename _T>concept WithSort = requires(_T a) { { a.sort }; };
			template<typename _T>concept WithCount = requires(_T a) { { a.count }; };
			template<typename _T>concept WithFind = requires(_T a) { { a.find }; };
			template<typename _T>concept WithContains = requires(_T a) { { a.contains }; };
			template<typename _T>concept WithLowerBound = requires(_T a) { { a.lower_bound }; };
			template<typename _T>concept WithUpperBound = requires(_T a) { { a.upper_bound }; };
			template<typename _T>concept WithEqualRange = requires(_T a) { { a.equal_range }; };
			template<typename _T>concept WithKeyComp = requires(_T a) { { a.key_comp }; };
			template<typename _T>concept WithValueComp = requires(_T a) { { a.value_comp }; };
			template<typename _T>concept WithHashFunction = requires(_T a) { { a.hash_function }; };
			template<typename _T>concept WithKeyEq = requires(_T a) { { a.key_eq }; };

			namespace stdvector {

			}

			//TODO: generic
			template<typename _T>concept StringViewable = StdGenericContainer<_T> && requires(_T a) { { a[size_t()] } -> CharType; };
			template<typename _T>concept StringWritable = StdGenericContainer<_T> && requires(_T a) { { a[size_t()] = _T::value_type() }; };

			template<typename _T>concept StdVectorCompatible = WithOperatorCopy<_T> && WithSize<_T>
				&& WithPushBack<_T> && WithOperatorIndex<_T> && WithErase<_T>;
			template<typename _T>concept ArrayCompatible = WithOperatorIndex<_T>;
			template<typename _T>concept StdStringCompatible = StringViewable<_T> && StringWritable<_T>;

			template<typename _T>concept StdContainer = AnyIndexable<_T, size_t> && containers::WithSize<_T>;
			template<typename _T>concept StdGenericContainer = StdContainer<_T> && WithTypeValueType<_T>;

			template<typename _T>concept Iterable = WithIterators<_T> &&requires(_T a) {
				{ a.begin() } -> SameType<typename _T::iterator>;
				{ a.end() } -> SameType<typename _T::iterator>;
				{ a.cbegin() } -> SameType<typename _T::const_iterator>;
				{ a.cend() } -> SameType<typename _T::const_iterator>;
			};

			template<typename _T>concept StdIterableGenericContainer = StdGenericContainer<_T> && Iterable<_T>;
		}

		using containers::StdVectorCompatible;
		using containers::ArrayCompatible;
		using containers::StdStringCompatible;
		using containers::StdIterableGenericContainer;
		using containers::Iterable;

		template<typename _T>concept PusheableBack = WithTypeValueType<_T> && requires(_T a) { { a.push_back(_T::value_type()) }; };
		template<typename _T>concept PusheableFront = WithTypeValueType<_T> && requires(_T a) { { a.push_front(_T::value_type()) }; };

		template<typename _T, typename _Ret, typename... _Args>
		concept Callable = requires(_T a, _Args... args) {
			{ a(args...) } -> SameType<_Ret>;
		};
		
	}
}