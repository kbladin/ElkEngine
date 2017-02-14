#include "elk/core/debug_input.h"

namespace elk { namespace core {

std::map<std::string, float> DebugInput::_values;

DebugInput::~DebugInput()
{}

float& DebugInput::value(std::string key)
{
  if (!_values[key])
  {
    _values.insert({key, 0.0f});
  }
  return _values[key];
}

} }
