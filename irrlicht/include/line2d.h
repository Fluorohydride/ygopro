// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_LINE_2D_H_INCLUDED__
#define __IRR_LINE_2D_H_INCLUDED__

#include "irrTypes.h"
#include "vector2d.h"

namespace irr
{
namespace core
{

//! 2D line between two points with intersection methods.
template <class T>
class line2d
{
	public:
		//! Default constructor for line going from (0,0) to (1,1).
		line2d() : start(0,0), end(1,1) {}
		//! Constructor for line between the two points.
		line2d(T xa, T ya, T xb, T yb) : start(xa, ya), end(xb, yb) {}
		//! Constructor for line between the two points given as vectors.
		line2d(const vector2d<T>& start, const vector2d<T>& end) : start(start), end(end) {}
		//! Copy constructor.
		line2d(const line2d<T>& other) : start(other.start), end(other.end) {}

		// operators

		line2d<T> operator+(const vector2d<T>& point) const { return line2d<T>(start + point, end + point); }
		line2d<T>& operator+=(const vector2d<T>& point) { start += point; end += point; return *this; }

		line2d<T> operator-(const vector2d<T>& point) const { return line2d<T>(start - point, end - point); }
		line2d<T>& operator-=(const vector2d<T>& point) { start -= point; end -= point; return *this; }

		bool operator==(const line2d<T>& other) const
		{ return (start==other.start && end==other.end) || (end==other.start && start==other.end);}
		bool operator!=(const line2d<T>& other) const
		{ return !(start==other.start && end==other.end) || (end==other.start && start==other.end);}

		// functions
		//! Set this line to new line going through the two points.
		void setLine(const T& xa, const T& ya, const T& xb, const T& yb){start.set(xa, ya); end.set(xb, yb);}
		//! Set this line to new line going through the two points.
		void setLine(const vector2d<T>& nstart, const vector2d<T>& nend){start.set(nstart); end.set(nend);}
		//! Set this line to new line given as parameter.
		void setLine(const line2d<T>& line){start.set(line.start); end.set(line.end);}

		//! Get length of line
		/** \return Length of the line. */
		T getLength() const { return start.getDistanceFrom(end); }

		//! Get squared length of the line
		/** \return Squared length of line. */
		T getLengthSQ() const { return start.getDistanceFromSQ(end); }

		//! Get middle of the line
		/** \return center of the line. */
		vector2d<T> getMiddle() const
		{
			return (start + end)/(T)2;
		}

		//! Get the vector of the line.
		/** \return The vector of the line. */
		vector2d<T> getVector() const { return vector2d<T>(end.X - start.X, end.Y - start.Y); }

		//! Tests if this line intersects with another line.
		/** \param l: Other line to test intersection with.
		\param checkOnlySegments: Default is to check intersection between the begin and endpoints.
		When set to false the function will check for the first intersection point when extending the lines.
		\param out: If there is an intersection, the location of the
		intersection will be stored in this vector.
		\return True if there is an intersection, false if not. */
		bool intersectWith(const line2d<T>& l, vector2d<T>& out, bool checkOnlySegments=true) const
		{
			// Uses the method given at:
			// http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
			const f32 commonDenominator = (f32)(l.end.Y - l.start.Y)*(end.X - start.X) -
											(l.end.X - l.start.X)*(end.Y - start.Y);

			const f32 numeratorA = (f32)(l.end.X - l.start.X)*(start.Y - l.start.Y) -
											(l.end.Y - l.start.Y)*(start.X -l.start.X);

			const f32 numeratorB = (f32)(end.X - start.X)*(start.Y - l.start.Y) -
											(end.Y - start.Y)*(start.X -l.start.X);

			if(equals(commonDenominator, 0.f))
			{
				// The lines are either coincident or parallel
				// if both numerators are 0, the lines are coincident
				if(equals(numeratorA, 0.f) && equals(numeratorB, 0.f))
				{
					// Try and find a common endpoint
					if(l.start == start || l.end == start)
						out = start;
					else if(l.end == end || l.start == end)
						out = end;
					// now check if the two segments are disjunct
					else if (l.start.X>start.X && l.end.X>start.X && l.start.X>end.X && l.end.X>end.X)
						return false;
					else if (l.start.Y>start.Y && l.end.Y>start.Y && l.start.Y>end.Y && l.end.Y>end.Y)
						return false;
					else if (l.start.X<start.X && l.end.X<start.X && l.start.X<end.X && l.end.X<end.X)
						return false;
					else if (l.start.Y<start.Y && l.end.Y<start.Y && l.start.Y<end.Y && l.end.Y<end.Y)
						return false;
					// else the lines are overlapping to some extent
					else
					{
						// find the points which are not contributing to the
						// common part
						vector2d<T> maxp;
						vector2d<T> minp;
						if ((start.X>l.start.X && start.X>l.end.X && start.X>end.X) || (start.Y>l.start.Y && start.Y>l.end.Y && start.Y>end.Y))
							maxp=start;
						else if ((end.X>l.start.X && end.X>l.end.X && end.X>start.X) || (end.Y>l.start.Y && end.Y>l.end.Y && end.Y>start.Y))
							maxp=end;
						else if ((l.start.X>start.X && l.start.X>l.end.X && l.start.X>end.X) || (l.start.Y>start.Y && l.start.Y>l.end.Y && l.start.Y>end.Y))
							maxp=l.start;
						else
							maxp=l.end;
						if (maxp != start && ((start.X<l.start.X && start.X<l.end.X && start.X<end.X) || (start.Y<l.start.Y && start.Y<l.end.Y && start.Y<end.Y)))
							minp=start;
						else if (maxp != end && ((end.X<l.start.X && end.X<l.end.X && end.X<start.X) || (end.Y<l.start.Y && end.Y<l.end.Y && end.Y<start.Y)))
							minp=end;
						else if (maxp != l.start && ((l.start.X<start.X && l.start.X<l.end.X && l.start.X<end.X) || (l.start.Y<start.Y && l.start.Y<l.end.Y && l.start.Y<end.Y)))
							minp=l.start;
						else
							minp=l.end;

						// one line is contained in the other. Pick the center
						// of the remaining points, which overlap for sure
						out = core::vector2d<T>();
						if (start != maxp && start != minp)
							out += start;
						if (end != maxp && end != minp)
							out += end;
						if (l.start != maxp && l.start != minp)
							out += l.start;
						if (l.end != maxp && l.end != minp)
							out += l.end;
						out.X = (T)(out.X/2);
						out.Y = (T)(out.Y/2);
					}

					return true; // coincident
				}

				return false; // parallel
			}

			// Get the point of intersection on this line, checking that
			// it is within the line segment.
			const f32 uA = numeratorA / commonDenominator;
			if(checkOnlySegments && (uA < 0.f || uA > 1.f) )
				return false; // Outside the line segment

			const f32 uB = numeratorB / commonDenominator;
			if(checkOnlySegments && (uB < 0.f || uB > 1.f))
				return false; // Outside the line segment

			// Calculate the intersection point.
			out.X = (T)(start.X + uA * (end.X - start.X));
			out.Y = (T)(start.Y + uA * (end.Y - start.Y));
			return true;
		}

		//! Get unit vector of the line.
		/** \return Unit vector of this line. */
		vector2d<T> getUnitVector() const
		{
			T len = (T)(1.0 / getLength());
			return vector2d<T>((end.X - start.X) * len, (end.Y - start.Y) * len);
		}

		//! Get angle between this line and given line.
		/** \param l Other line for test.
		\return Angle in degrees. */
		f64 getAngleWith(const line2d<T>& l) const
		{
			vector2d<T> vect = getVector();
			vector2d<T> vect2 = l.getVector();
			return vect.getAngleWith(vect2);
		}

		//! Tells us if the given point lies to the left, right, or on the line.
		/** \return 0 if the point is on the line
		<0 if to the left, or >0 if to the right. */
		T getPointOrientation(const vector2d<T>& point) const
		{
			return ( (end.X - start.X) * (point.Y - start.Y) -
					(point.X - start.X) * (end.Y - start.Y) );
		}

		//! Check if the given point is a member of the line
		/** \return True if point is between start and end, else false. */
		bool isPointOnLine(const vector2d<T>& point) const
		{
			T d = getPointOrientation(point);
			return (d == 0 && point.isBetweenPoints(start, end));
		}

		//! Check if the given point is between start and end of the line.
		/** Assumes that the point is already somewhere on the line. */
		bool isPointBetweenStartAndEnd(const vector2d<T>& point) const
		{
			return point.isBetweenPoints(start, end);
		}

		//! Get the closest point on this line to a point
		/** \param checkOnlySegments: Default (true) is to return a point on the line-segment (between begin and end) of the line.
		When set to false the function will check for the first the closest point on the the line even when outside the segment. */
		vector2d<T> getClosestPoint(const vector2d<T>& point, bool checkOnlySegments=true) const
		{
			vector2d<f64> c((f64)(point.X-start.X), (f64)(point.Y- start.Y));
			vector2d<f64> v((f64)(end.X-start.X), (f64)(end.Y-start.Y));
			f64 d = v.getLength();
			if ( d == 0 )	// can't tell much when the line is just a single point
				return start;
			v /= d;
			f64 t = v.dotProduct(c);

			if ( checkOnlySegments )
			{
				if (t < 0) return vector2d<T>((T)start.X, (T)start.Y);
				if (t > d) return vector2d<T>((T)end.X, (T)end.Y);
			}

			v *= t;
			return vector2d<T>((T)(start.X + v.X), (T)(start.Y + v.Y));
		}

		//! Start point of the line.
		vector2d<T> start;
		//! End point of the line.
		vector2d<T> end;
};

	// partial specialization to optimize <f32> lines (avoiding casts)
	template <>
	inline vector2df line2d<irr::f32>::getClosestPoint(const vector2df& point, bool checkOnlySegments) const
	{
		vector2df c = point - start;
		vector2df v = end - start;
		f32 d = (f32)v.getLength();
		if ( d == 0 )	// can't tell much when the line is just a single point
			return start;
		v /= d;
		f32 t = v.dotProduct(c);

		if ( checkOnlySegments )
		{
			if (t < 0) return start;
			if (t > d) return end;
		}

		v *= t;
		return start + v;
	}


	//! Typedef for an f32 line.
	typedef line2d<f32> line2df;
	//! Typedef for an integer line.
	typedef line2d<s32> line2di;

} // end namespace core
} // end namespace irr

#endif

