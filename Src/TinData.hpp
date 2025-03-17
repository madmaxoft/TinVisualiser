#pragma once

#include <memory>
#include <vector>





/** Represents all the data remembered for a TIN project:
- points
- edges (constraints)
*/
class TinData
{
public:

	// fwd:
	class Point;
	class Edge;
	
	
	
	
	/** Stores info for a single point. */
	class Point
	{
		std::vector<std::weak_ptr<Edge>> mNeighborConstraints;
		
	public:
		// The stored coords are publicly available:
		double mX;
		double mY;
		double mZ;

		Point(double aX, double aY, double aZ):
			mX(aX), mY(aY), mZ(aZ)
		{
		}
		
		/** Adds the specified edge into mNeighborConstraints.
		Asserts that such a constraint doesn't already exist. */
		void addNeighborConstraint(std::shared_ptr<Edge> & aConstraint);
		
		/** Returns the constraint to the specified other point, if there is one in mNeighborConstraints. */
		std::weak_ptr<Edge> constraintTo(const Point * aOther);
	};
	
	
	
	
	
	/** Stores infor for a single edge (constraint). */
	class Edge
	{
		std::weak_ptr<Point> mPoint1;
		std::weak_ptr<Point> mPoint2;
		
	public:
		Edge(std::weak_ptr<Point> aPoint1, std::weak_ptr<Point> aPoint2):
			mPoint1(aPoint1),
			mPoint2(aPoint2)
		{
		}
		
		/** Returns the point other than aPoint.
		Asserts that aPoint is actually in the edge. */
		std::weak_ptr<Point> theOtherPoint(const Point * aPoint);
		
		// Getters:
		std::shared_ptr<Point> point1() const { return mPoint1.lock(); }
		std::shared_ptr<Point> point2() const { return mPoint2.lock(); }
	};
	
	
	
	

	/** Adds a point at the specified coords. */
	std::shared_ptr<Point> addPoint(double x, double y, double z);
	
	/** Adds a constraint between the specified points.
	Ignored if such a constraint already exists. */
	std::shared_ptr<Edge> addConstraint(std::shared_ptr<Point> aPoint1, std::shared_ptr<Point> aPoint2);
	
	/** Adds a constraint between the points specified by their index in mPoints.
	Ignored if such a constraint already exists. */
	std::shared_ptr<Edge> addConstraint(size_t aPoint1Idx, size_t aPoint2Idx);
	
	/** Removes all points and contraints. */
	void clear();
	
	/** Move-merges the points and constraints from aOther into this. */
	void import(TinData && aOther);
	
	// Getters:
	const std::vector<std::shared_ptr<Point>> & points() const { return mPoints; }
	const std::vector<std::shared_ptr<Edge>> & constraints() const { return mConstraints; }
	
		
private:


	/** All the contained points. */
	std::vector<std::shared_ptr<Point>> mPoints;
	
	/** All the contained constraints. */
	std::vector<std::shared_ptr<Edge>> mConstraints;
};
