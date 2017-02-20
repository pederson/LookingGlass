#ifndef _OWNEDCOLOR_H
#define _OWNEDCOLOR_H

#include "DrawableColor.hpp"

namespace LookingGlass{

/** @class OwnedColor
 *  @brief class for color in LookingGlass
 *
 *  an rgba color where the data is
 *	owned by the object
 *
 */
class OwnedColor : public DrawableColor{
public:
	OwnedColor()
	: m_r(1), m_g(1), m_b(1), m_a(1) {};

	OwnedColor(double cr, double cg, double cb, double ca)
	: m_r(cr), m_g(cg), m_b(cb), m_a(ca) {};

	double r() const {return m_r;};
	double g() const {return m_g;};
	double b() const {return m_b;};
	double a() const {return m_a;};

protected:
	double m_r;
	double m_g;
	double m_b;
	double m_a;
};
}

#endif