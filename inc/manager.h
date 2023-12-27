/*****************************************************************************
*
* manager.h
*
* By Jari Korkala 12/12/2015
*
*****************************************************************************/

#ifndef CENGINE_MANAGER_H
#define CENGINE_MANAGER_H

#include <ceng.h>

#include "engine-result.h"

namespace CEngine
{
	class Manager
	{
	protected:
		Ceng::StringUtf8 assetPath;

	public:
		Manager();

		virtual ~Manager();

		const EngineResult::value AddPath(const Ceng::StringUtf8 &assetPath);

		void GetFileInfo(const Ceng::StringUtf8 &filename, Ceng::StringUtf8 &finalPath, 
			Ceng::StringUtf8 &title,Ceng::StringUtf8 &extension) const;
	};
}

#endif