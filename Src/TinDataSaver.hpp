#pragma once

#include <QIODevice>





// fwd:
class TinData;





namespace TinDataSaver
{

/** Saves the specified TIN data into the specified output device.
Throws a std::runtime_error descendant upon error. */
void saveTo(const TinData & aData, QIODevice & aDest);

};
