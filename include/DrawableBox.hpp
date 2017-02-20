#ifndef _DRAWABLEBOX_H
#define _DRAWABLEBOX_H

#include "DrawablePoint.hpp"

#include <iostream>
#include <memory>

namespace LookingGlass{

/** @class DrawableBox
 *  @brief base class for box in LookingGlass
 *
 *  a 3D box used for rendering
 *
 */
class DrawableBox{
public:
	DrawableBox() {};

	DrawableBox(std::shared_ptr<DrawablePoint> l, std::shared_ptr<DrawablePoint> h)
	: lo(l), hi(h) {};

	
	std::shared_ptr<DrawablePoint> lo;
	std::shared_ptr<DrawablePoint> hi;
};
}

#endif