#include "confstdparse.hpp"

namespace confstd {
	std::vector<string_t> confstd::split(string_view_t str, const string_t& sperators, char_t keepChar) {
		std::vector<string_t> ret{};
		ret.push_back({});
		const auto sbeg{sperators.cbegin()}; const auto send{sperators.cend()};
		const auto beg{str.cbegin()};  const auto end{str.cend()};
		for(auto it{beg}; it != end; ++it) {
			if(const auto spit = std::find(sbeg, send, *it); spit != send) {
				if(!ret[ret.size() - 1].empty())
					ret.push_back({});
				if(spit != sbeg && *(spit - 1) == keepChar) {
					ret[ret.size() - 1].append(1, *it);
					ret.push_back({});
				}
			}
			else
				ret[ret.size() - 1].append(1, *it);
		}
		if(ret[ret.size() - 1].empty()) ret.erase(ret.end() - 1);
		return ret;
	}

	void trim(string_t& str) {
		string_t::const_iterator b{str.cbegin()};
		while(isspace(*(b++)));
		str.erase(str.cbegin(), b-1);
		if(str.empty()) return;
		string_t::const_iterator e{str.cend()};
		while(isspace(*(--e)));
		str.erase(e+1, str.cend());
	}

	// For simplicity we assume function is already trimed !
	// TODO: optimization ?
	void press(string_t& str) {
		std::size_t si = 0;
		// Iterators are invalidated so index mandatory
		for(std::size_t i{0U}; i < str.size(); ++i) {
			if(str[i] == '\t') str[i] = ' ';
			if(isspace(str[i])) {
				if(si == 0)
					si = i;
			}
			else{
				if(si == 1) 
					si = 0; 
				else if(si > 0) {
					str.erase(str.cbegin() + si, str.cbegin() + i - 1);
					i -= i - si;
					si = 0;
				}
			}
		}
	}

	void strreplace(string_t& src, string_view_t what, string_view_t by) {
		std::size_t pos{0U};
		while((pos = src.find(what, pos)) != string_t::npos) {
			src.replace(pos, what.size(), by);
			pos += by.size();
		}
	}
}