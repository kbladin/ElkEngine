#pragma once

#include <gl/glew.h>

#include <vector>

namespace sge { namespace core {

class TextureUnit {
public:
    
    TextureUnit();
	~TextureUnit();

    void activate();

    GLint glEnum();
    GLint unitNumber();

    operator GLint();

    static void setZeroUnit();

    static int numberActiveUnits();

private:
    void assignUnit();
    static void initialize();

    GLint _number;
    GLint _glEnum;
    bool _assigned;
    static bool _initialized;
    static unsigned int _totalActive;
    static unsigned int _maxTexUnits;
    static std::vector<bool> _busyUnits;
};

} }