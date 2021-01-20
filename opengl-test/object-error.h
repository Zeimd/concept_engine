/*****************************************************************************
*
* object-error.h
*
* By Jari Korkala 7/6/2015
*
*****************************************************************************/

#ifndef CENGINE_OBJECT_ERROR_H
#define CENGINE_OBJECT_ERROR_H

namespace ObjectError
{
	enum value
	{
		ok,
		fail,
		file_not_found,
		io_error,
		invalid_param,
	};
}


#endif
