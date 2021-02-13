#include "confscope.hpp"
#include <algorithm>

namespace conflib {

  const ConfScopeable* ConfScope::GetByName(const string_t& name, ObjectType filter) const {
    auto it = std::find_if(m_Childs.cbegin(), m_Childs.cend(),
      [&name, filter](const ConfScopeable* child) { return child->GetObjectType() == filter && child->GetName() == name; });
    return it == m_Childs.cend() ? nullptr : &**it;
  }

  const ConfScopeable* ConfScope::GetByName(const string_t& name) const {
    auto it = std::find_if(m_Childs.cbegin(), m_Childs.cend(),
      [&name](const ConfScopeable* child) { return child->GetName() == name; });
    return it == m_Childs.cend() ? nullptr : &**it;
  }

  ConfScopeable* ConfScope::Clone(string_t newName) const {
    auto cloned = static_cast<ConfScope*>(ConfScopeable::Clone(std::move(newName)));
    cloned->m_Childs = m_Childs;
    cloned->m_Bases = m_Bases;
    cloned->m_Parent = m_Parent;
    return cloned;
  }
}