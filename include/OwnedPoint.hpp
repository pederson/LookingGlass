#ifndef _OWNEDPOINT_H
#define _OWNEDPOINT_H

#include "DrawablePoint.hpp"

namespace LookingGlass{

/** @class OwnedPoint
 *  @brief class for point in LookingGlass
 *
 *  a 3D point where the data is
 *	owned by the object
 *
 */
class OwnedPoint : public DrawablePoint{
public:
	OwnedPoint()
	: m_x(0), m_y(0), m_z(0) {};

	OwnedPoint(double px, double py, double pz)
	: m_x(px), m_y(py), m_z(pz) {};

	double x() const {return m_x;};
	double y() const {return m_y;};
	double z() const {return m_z;};

protected:
	double m_x;
	double m_y;
	double m_z;
};
}

#endif