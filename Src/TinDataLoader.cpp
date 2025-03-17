#include "TinDataLoader.hpp"





bool TinDataLoader::doesProbeMatch(const QByteArray & aProbe)
{
	// Check that the probe contains only allowed characters:
	for (const auto & b: aProbe)
	{
		switch (b)
		{
			case 9:
			case 10:
			case 13:
			case 32:
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '.':
			case '-':
			{
				break;
			}
			default:
			{
				return false;
			}
		}
	}

	// Check that the first line is a reasonable integer:
	auto idxNewline = aProbe.indexOf('\n');
	if (idxNewline < 0)
	{
		return false;
	}
	bool isOK = false;
	auto numPoints = aProbe.left(idxNewline).toInt(&isOK);
	if (!isOK || (numPoints < 0) || (numPoints > 1000000))
	{
		return false;
	}

	return true;
}





TinData TinDataLoader::loadFrom(QIODevice & aDataSource)
{
	// Load the points:
	TinData res;
	bool isOK = false;
	auto numPoints = aDataSource.readLine().toInt(&isOK);
	if (!isOK)
	{
		throw TinDataLoader::Error("Cannot read the point count", 1);
	}
	for (int i = 0; i < numPoints; ++i)
	{
		auto line = aDataSource.readLine().split('\t');
		if (line.size() == 1)
		{
			line = line[0].split(' ');
		}
		if (line.size() != 3)
		{
			throw TinDataLoader::Error("Line doesn't contain 3 coords separated by tab or space", i + 2);
		}
		double coords[3];
		for (int j = 0; j < 3; ++j)
		{
			coords[j] = line[j].toDouble(&isOK);
			if (!isOK)
			{
				throw TinDataLoader::Error("Cannot parse coord", i + 2);
			}
		}
		res.addPoint(coords[0], coords[1], coords[2]);
	}

	// Load the constraints:
	auto lineNum = numPoints + 2;
	auto numConstraintsLine = aDataSource.readLine().trimmed();
	if (numConstraintsLine.isEmpty())
	{
		// Constraints not present in the file, return just the points:
		return res;
	}
	auto numConstraints = numConstraintsLine.toInt(&isOK);
	if (!isOK)
	{
		throw TinDataLoader::Error("Cannot read the constraint count", lineNum);
	}
	++lineNum;
	for (int i = 0; i < numConstraints; ++i)
	{
		auto line = aDataSource.readLine().split('\t');
		if (line.size() == 1)
		{
			line = line[0].split(' ');
		}
		if (line.size() != 2)
		{
			throw TinDataLoader::Error("Line doesn't contain 2 indices separated by tab or space", lineNum + i);
		}
		unsigned long indices[2];
		for (int j = 0; j < 2; ++j)
		{
			indices[j] = line[j].toULong(&isOK);
			if (!isOK)
			{
				throw TinDataLoader::Error("Cannot parse constraint's point index", lineNum + i);
			}
		}
		res.addConstraint(indices[0], indices[1]);
	}
	return res;
}
