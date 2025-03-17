#pragma once

#include <QByteArray>
#include <QIODevice>

#include "TinData.hpp"





namespace TinDataLoader
{

/** Thrown as an exception in case of loading errors. */
class Error:
	public std::runtime_error
{
	/** The line number where the error was encountered. */
	int mLineNum;

public:

	Error(const char * aMessage, int aLineNum):
		std::runtime_error(aMessage),
		mLineNum(aLineNum)
	{
	}
};





/** Returns true if the data in the probe is likely to be in TIN format. */
bool doesProbeMatch(const QByteArray & aProbe);

/** Loads complete data from TIN-formatted data source.
Throws an Error on any problem. */
TinData loadFrom(QIODevice & aDataSource);

};  // namespace TinDataLoader
