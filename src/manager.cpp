/*****************************************************************************
*
* manager.cpp
*
* By Jari Korkala 12/12/2015
*
*****************************************************************************/

#include "manager.h"

using namespace CEngine;

Manager::Manager()
{
}

Manager::~Manager()
{
}

const EngineResult::value Manager::AddPath(const Ceng::StringUtf8 &assetPath)
{
	Ceng_MakeOSPath(assetPath, this->assetPath);

	return EngineResult::ok;
}

void Manager::GetFileInfo(const Ceng::StringUtf8 &filename, Ceng::StringUtf8 &finalPath, 
	Ceng::StringUtf8 &title,Ceng::StringUtf8 &extension) const
{
	auto iter = filename.FindLast('.', filename.ConstBeginIterator());

	title = filename.SubString(filename.ConstBeginIterator(), iter);

	++iter;

	extension = filename.SubString(iter, filename.ConstEndIterator());

	finalPath = assetPath + filename;
}
