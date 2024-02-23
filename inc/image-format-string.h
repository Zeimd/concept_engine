#ifndef CENG_IMAGE_FORMAT_STRING_H
#define CENG_IMAGE_FORMAT_STRING_H

#include <ceng/datatypes/ce-string.h>
#include <ceng/enums/image-formats.h>

namespace CEngine
{
	const Ceng::String ImageFormatToString(const Ceng::IMAGE_FORMAT::value format);
}

#endif