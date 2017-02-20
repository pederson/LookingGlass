#ifndef _DRAWABLECOLOR_H
#define _DRAWABLECOLOR_H

namespace LookingGlass{

/** @class DrawableColor
 *  @brief base class for color in LookingGlass
 *
 *  an RGBA point used for rendering
 *
 */
class DrawableColor{
public:
	virtual double r() const = 0;
	virtual double g() const = 0;
	virtual double b() const = 0;
	virtual double a() const = 0;
};
}

#endif