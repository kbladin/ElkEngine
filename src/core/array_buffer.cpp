#include "elk/core/array_buffer.h"

namespace elk { namespace core {

ArrayBuffer::ArrayBuffer(InitData init_data) :
  _init_data(init_data)
{
  glGenBuffers(1, &_id);
  upload();
}

ArrayBuffer::~ArrayBuffer()
{
  glDeleteBuffers(1, &_id);
}

void ArrayBuffer::render()
{
  glDrawArrays(_init_data.render_mode, 0, _init_data.n_elements);
}

void ArrayBuffer::update(InitData init_data)
{
  memcpy(&_init_data, &init_data, sizeof(InitData));
  upload();
}

void ArrayBuffer::upload()
{
  bind();
  glBufferData(
    _init_data.buffer_type,
    _init_data.data_size,
    _init_data.data,
    _init_data.mode);
}

ElementArrayBuffer::ElementArrayBuffer(InitData init_data) :
  ArrayBuffer(init_data)
{

}

void ElementArrayBuffer::render()
{
  glDrawElements(
    _init_data.render_mode, _init_data.n_elements, _init_data.type,
    static_cast<void*>(0));
}

} }
