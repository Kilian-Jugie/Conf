#pragma once
#include "conflibglobal.hpp"
#include <memory>

namespace conflib {
	enum class ObjectType {
		SCOPE,
		TYPE,
		INSTANCE,
		NONE
	};

	class ConfScopeable {
	protected:
		string_t m_Name;
		bool m_IsTemporary;

	public:
		ConfScopeable(string_t name, bool isTemporary = false) :
			m_Name{std::move(name)}, m_IsTemporary{isTemporary} {
		}

		ConfScopeable(const ConfScopeable&) = default;
		ConfScopeable(ConfScopeable&&) = default;

		virtual ObjectType GetObjectType() const = 0;
		virtual ConfScopeable* Clone(string_t newName) const {
			//return new ConfScopeable{std::move(newName), m_IsTemporary};
			return nullptr;
		}


		void SetIsTemporary(bool newVal) {
			m_IsTemporary = newVal;
		}

		bool GetIsTemporary() const {
			return m_IsTemporary;
		}

		const string_t& GetName() const {
			return m_Name;
		}
	};
}