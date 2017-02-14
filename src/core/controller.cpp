#include "elk/core/controller.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace elk { namespace core {

Controller::Controller() :
	_mouse_x(0),
	_mouse_y(0),
	_mouse_dx(0),
	_mouse_dy(0),
	_scroll_dx(0),
	_scroll_dy(0)
{

}
Controller::~Controller()
{

}

void Controller::mousePosCallback(double x, double y)
{
	_mouse_dx = x - _mouse_x;
	_mouse_dy = y - _mouse_y;
	_mouse_x = x;
	_mouse_y = y;
}

void Controller::mouseButtonCallback(MouseButton button, KeyAction action)
{
	if (action == KeyAction::PRESS)
	{
		_mouse_buttons_pressed.insert(button);
	}
	if (action == KeyAction::RELEASE)
	{
		_mouse_buttons_pressed.erase(button);
	}
}

void Controller::mouseScrollCallback(double dx, double dy)
{
	_scroll_dx = dx;
	_scroll_dy = dy;
}

void Controller::keyCallback(Key key, KeyAction action)
{
	if (action == KeyAction::PRESS)
	{
		_keys_pressed.insert(key);
        if (key == Key::KEY_LEFT_SHIFT || key == Key::KEY_RIGHT_SHIFT)
		{
			_modifiers_pressed.insert(KeyModifier::MOD_SHIFT);
		}
		if (key == Key::KEY_LEFT_CONTROL || key == Key::KEY_RIGHT_CONTROL)
		{
			_modifiers_pressed.insert(KeyModifier::MOD_CONTROL);
		}
		if (key == Key::KEY_LEFT_ALT || key == Key::KEY_RIGHT_ALT)
		{
			_modifiers_pressed.insert(KeyModifier::MOD_SHIFT);
		}
		if (key == Key::KEY_LEFT_SUPER || key == Key::KEY_RIGHT_SUPER)
		{
			_modifiers_pressed.insert(KeyModifier::MOD_SUPER);
		}
	}
	else if (action == KeyAction::RELEASE)
	{
		_keys_pressed.erase(key);
		if (key == Key::KEY_LEFT_SHIFT || key == Key::KEY_RIGHT_SHIFT)
		{
			_modifiers_pressed.erase(KeyModifier::MOD_SHIFT);
		}
		if (key == Key::KEY_LEFT_CONTROL || key == Key::KEY_RIGHT_CONTROL)
		{
			_modifiers_pressed.erase(KeyModifier::MOD_CONTROL);
		}
		if (key == Key::KEY_LEFT_ALT || key == Key::KEY_RIGHT_ALT)
		{
			_modifiers_pressed.erase(KeyModifier::MOD_SHIFT);
		}
		if (key == Key::KEY_LEFT_SUPER || key == Key::KEY_RIGHT_SUPER)
		{
			_modifiers_pressed.erase(KeyModifier::MOD_SUPER);
		}
	}

}

void Controller::windowSizeCallback(int width, int height)
{

}

SphericalController::SphericalController(Object3D& object) :
	Controller(),
	_object(object)
{
	// These should be set according to the transform matrix of object
	_theta = 0;
	_phi = 0;
	_r = 1;
	_center = glm::vec3(0.0f, 0.0f, 0.0f);
}

SphericalController::~SphericalController()
{

}

void SphericalController::step(float dt)
{
	handleInput(dt);
	transformObject();
}

void SphericalController::handleInput(float dt)
{
    if (_modifiers_pressed.count(KeyModifier::MOD_CONTROL))
	{
		_r -= _scroll_dy * _r * sensitivity;
		_r = glm::clamp(_r, 0.01f, 100.0f);
    _shift_dx = -_scroll_dx * sensitivity * _r;
  }
	else if (_modifiers_pressed.count(KeyModifier::MOD_SHIFT))
	{
		_shift_dx = -_scroll_dx * sensitivity * _r;
		_shift_dy = _scroll_dy * sensitivity * _r;
	}
	else
	{
		_theta += _scroll_dx * sensitivity;
		_phi += _scroll_dy * sensitivity;
    _shift_dx = 0;
    _shift_dy = 0;
	}

	if (_keys_pressed.count(Key::KEY_R))
	{
		_center = glm::vec3(0.0f);
	}

	// Reset
	_mouse_dx = 0;
	_mouse_dy = 0;
	_scroll_dx = 0;
	_scroll_dy = 0;
}

void SphericalController::transformObject()
{
	glm::mat4 T = glm::translate(glm::vec3(0.0f, 0.0f, _r));
	glm::mat4 R1 = glm::rotate(glm::mat4(1.0f), _phi, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 R2 = glm::rotate(glm::mat4(1.0f), _theta, glm::vec3(0.0f, 1.0f, 0.0f));
	
	_center += glm::vec3(R2 * R1 * glm::vec4(_shift_dx, _shift_dy, 0.0f, 1.0f));

	glm::mat4 T2 = glm::translate(_center);

	glm::mat4 M = T2 * R2 * R1 * T;

	_object.setTransform(M);
}

} }
