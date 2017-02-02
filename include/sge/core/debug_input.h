#pragma once

#include <map>
#include <string>
#include <memory>

namespace sge { namespace core {

class DebugInput
{
public:
	DebugInput(){};
	~DebugInput();

	static float& value(std::string key);
private:
	static std::map<std::string, float> _values;
};

} }
