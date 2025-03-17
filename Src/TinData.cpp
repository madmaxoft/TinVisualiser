#include "TinData.hpp"

#include <cassert>





////////////////////////////////////////////////////////////////////////////////
// TinData::Point:

void TinData::Point::addNeighborConstraint(std::shared_ptr<Edge> & aConstraint)
{
	// Check that the constraint doesn't already exist:
	auto pt2 = aConstraint->theOtherPoint(this).lock();
	assert(constraintTo(pt2.get()).lock() == nullptr);
	
	// Add the constraint:
	mNeighborConstraints.push_back(aConstraint);
}





std::weak_ptr<TinData::Edge> TinData::Point::constraintTo(const Point * aOther)
{
	for (const auto & constraint: mNeighborConstraints)
	{
		if (constraint.lock()->theOtherPoint(this).lock().get() == aOther)
		{
			return constraint;
		}
	}
	return {};
}





////////////////////////////////////////////////////////////////////////////////
// TinData::Edge:

std::weak_ptr<TinData::Point> TinData::Edge::theOtherPoint(const Point * aPoint)
{
	if (mPoint1.lock().get() == aPoint)
	{
		return mPoint2;
	}
	assert(mPoint2.lock().get() == aPoint);
	return mPoint1;
}





////////////////////////////////////////////////////////////////////////////////
// TinData:

std::shared_ptr<TinData::Point> TinData::addPoint(double x, double y, double z)
{
	auto pt = std::make_shared<Point>(x, y, z);
	mPoints.push_back(pt);
	return pt;
}





std::shared_ptr<TinData::Edge> TinData::addConstraint(
	std::shared_ptr<TinData::Point> aPoint1,
	std::shared_ptr<TinData::Point> aPoint2
)
{
	// Check if constraint already exists:
	auto constraint = aPoint1->constraintTo(aPoint2.get()).lock();
	if (constraint != nullptr)
	{
		return constraint;
	}
	assert(aPoint2->constraintTo(aPoint1.get()).lock() == nullptr);
	
	// Add the constraint:
	constraint = std::make_shared<Edge>(aPoint1, aPoint2);
	aPoint1->addNeighborConstraint(constraint);
	aPoint2->addNeighborConstraint(constraint);
	return constraint;
}





std::shared_ptr<TinData::Edge> TinData::addConstraint(size_t aPoint1Idx, size_t aPoint2Idx)
{
	return addConstraint(mPoints[aPoint1Idx], mPoints[aPoint2Idx]);
}





void TinData::clear()
{
	mConstraints.clear();
	mPoints.clear();
}





void TinData::import(TinData && aOther)
{
	mPoints.insert(mPoints.end(), aOther.mPoints.begin(), aOther.mPoints.end());
	mConstraints.insert(mConstraints.begin(), aOther.mConstraints.begin(), aOther.mConstraints.end());
}
