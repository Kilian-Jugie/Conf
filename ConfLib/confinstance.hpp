#pragma once
#include "confscopeable.hpp"
#include <vector>

namespace conflib {
	class ConfInstance : public ConfScopeable {
	protected:
		std::vector<std::shared_ptr<ConfInstance>> m_SubInstances;
	};
}