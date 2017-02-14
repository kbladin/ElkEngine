#include "elk/core/texture_unit.h"

namespace elk { namespace core {

bool TextureUnit::_initialized = false;
unsigned int TextureUnit::_totalActive = 0;
GLint TextureUnit::_maxTexUnits = 0;
std::vector<bool> TextureUnit::_busyUnits = std::vector<bool>();

TextureUnit::TextureUnit()
    : _number(0)
    , _glEnum(0)
    , _assigned(false)
{
    if (!_initialized) {
        initialize();
    }
}

TextureUnit::~TextureUnit() {
    if (_assigned) {
        _busyUnits.at(_number) = false;
        --_totalActive;
    }
}

void TextureUnit::activate() {
    if (!_assigned) {
        assignUnit();
    }
    glActiveTexture(_glEnum);
}

GLint TextureUnit::glEnum() {
    if (!_assigned) {
        assignUnit();
    }
    return _glEnum;
}

GLint TextureUnit::unitNumber() {
    if (!_assigned) {
        assignUnit();
    }
    return _number;
}

TextureUnit::operator GLint() {
    return unitNumber();
}

void TextureUnit::setZeroUnit() {
    glActiveTexture(GL_TEXTURE0);
}

int TextureUnit::numberActiveUnits() {
    return _totalActive;
}

void TextureUnit::assignUnit() {
    if (_totalActive >= _maxTexUnits)
        printf("No more texture units available!\n");

    _assigned = true;

    for (size_t i = 0; i < _maxTexUnits; ++i) {
        if (!_busyUnits[i]) {
            _number = static_cast<GLint>(i);
            _glEnum = GL_TEXTURE0 + _number;
            _busyUnits[i] = true;
            ++_totalActive;
            break;
        }
    }
}

void TextureUnit::initialize()
{
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_maxTexUnits);
    _busyUnits = std::vector<bool>(_maxTexUnits, false);
    _initialized = true;
}

} }
