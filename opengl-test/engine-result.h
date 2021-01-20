/*****************************************************************************
*
* engine-result.h
*
* By Jari Korkala 1/11/2015
*
*****************************************************************************/

#ifndef CENGINE_RESULT_H
#define CENGINE_RESULT_H

namespace CEngine
{
	namespace EngineResult
	{
		enum value
		{
			ok,
			fail,
			out_of_memory,
			out_of_video_memory,
			invalid_param,
			io_error,
			unexpected_eof,
			unknown_file_format,
			file_not_found,
			not_supported,
			null_ptr,
			texture_type_mismatch,
			invalid_texture_type,
		};
	}
}

#endif
