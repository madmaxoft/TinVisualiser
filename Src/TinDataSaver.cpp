#include "TinDataSaver.hpp"

#include <QByteArray>

#include "TinData.hpp"





namespace
{

/** Returns a textual representation of the coord. */
inline QByteArray formatCoord(double aCoord)
{
	auto res = QByteArray::number(aCoord, 'f', 3);
	/*
	while (res.endsWith('0'))
	{
		res.chop(1);
	}
	*/
	return res;
}

}  // anonymous namespace





void TinDataSaver::saveTo(const TinData & aData, QIODevice & aDest)
{
	auto crlf = QByteArray::fromRawData("\r\n", 2);
	auto tab = QByteArray::fromRawData("\t", 1);
	
	// Write the points:
	aDest.write(QByteArray::number(aData.points().size()));
	aDest.write(crlf);
	std::map<TinData::Point *, int> pointIndexMap;  // Maps the Point instance to its index
	int idx = 0;
	for (const auto & pt: aData.points())
	{
		aDest.write(formatCoord(pt->mX));
		aDest.write(tab);
		aDest.write(formatCoord(pt->mY));
		aDest.write(tab);
		aDest.write(formatCoord(pt->mZ));
		aDest.write(crlf);
		pointIndexMap[pt.get()] = idx++;
	}
	
	// Write the constraints:
	aDest.write(QByteArray::number(aData.constraints().size()));
	for (const auto & constraint: aData.constraints())
	{
		aDest.write(QByteArray::number(pointIndexMap[constraint->point1().get()]));
		aDest.write(tab);
		aDest.write(QByteArray::number(pointIndexMap[constraint->point2().get()]));
		aDest.write(crlf);
	}
}
