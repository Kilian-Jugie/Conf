#pragma once
#include "confscopeable.hpp"
#include <vector>

namespace conflib {
	class ConfScope : public ConfScopeable {
	private:
		std::vector<ConfScopeable*> m_Childs;
		std::vector<ConfScope*> m_Bases;
		ConfScope* m_Parent;
	public:
		ConfScope(string_t name, ConfScope* parent) :
			ConfScopeable{std::move(name)}, m_Parent{std::move(parent)} {}

		virtual ObjectType GetObjectType() const override {
			return ObjectType::SCOPE;
		}

		inline const ConfScope& GetParent() const {
			return *m_Parent;
		}

		inline const std::vector<ConfScopeable*>& GetChilds() const {
			return m_Childs;
		}

		inline const std::vector<ConfScope*>& GetBases() const {
			return m_Bases;
		}

		inline void AddChild(ConfScopeable* child) {
			m_Childs.push_back(std::move(child));
		}

		inline void AddBase(ConfScope* child) {
			m_Childs.push_back(std::move(child));
		}

		const ConfScopeable* GetByName(const string_t& name, ObjectType filter) const;
		const ConfScopeable* GetByName(const string_t& name) const;

		virtual ConfScopeable* Clone(string_t newName) const override;
	};
}