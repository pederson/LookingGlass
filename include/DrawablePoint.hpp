#ifndef _DRAWABLEPOINT_H
#define _DRAWABLEPOINT_H

namespace LookingGlass{

/** @class DrawablePoint
 *  @brief base class for point in LookingGlass
 *
 *  a 3D point used for rendering
 *
 */
class DrawablePoint{
public:
	virtual double x() const = 0;
	virtual double y() const = 0;
	virtual double z() const = 0;
};
}

#endif