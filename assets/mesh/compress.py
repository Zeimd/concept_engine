
import os.path

import ctypes

DDS_COMPRESS_NONE = 0
DDS_COMPRESS_BC1 = 1        # DXT1  
DDS_COMPRESS_BC2 = 2        # DXT3  
DDS_COMPRESS_BC3 = 3        	 # DXT5  
DDS_COMPRESS_BC3N = 4       	 # DXT5n 
DDS_COMPRESS_BC4 = 5        	 # ATI1  
DDS_COMPRESS_BC5 = 6        	 # ATI2 
DDS_COMPRESS_RXGB = 7       	 # DXT5

DDS_MIPMAP_FILTER_DEFAULT = 0
DDS_MIPMAP_FILTER_NEAREST = 1
DDS_MIPMAP_FILTER_BOX = 2
DDS_MIPMAP_FILTER_TRIANGLE = 3
DDS_MIPMAP_FILTER_QUADRATIC = 4
DDS_MIPMAP_FILTER_BSPLINE = 5
DDS_MIPMAP_FILTER_MITCHELL = 6
DDS_MIPMAP_FILTER_LANCZOS = 7
DDS_MIPMAP_FILTER_KAISER = 8

DDS_MIPMAP_WRAP_DEFAULT = 0
DDS_MIPMAP_WRAP_MIRROR = 1
DDS_MIPMAP_WRAP_REPEAT = 2
DDS_MIPMAP_WRAP_CLAMP = 3

dds_plugin_path = "ddsplugin.dll"
dds_plugin = ctypes.CDLL(dds_plugin_path)

call_Compress_Texture2D_DDX = dds_plugin.Compress_Texture2D_DDX
call_Compress_Texture2D_DDX.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.c_int]
call_Compress_Texture2D_DDX.restype = ctypes.c_int

def Compress_Texture2D_DXT(sourceFile,destFile,compression=None,normalMap=0,mipmapFilter=DDS_MIPMAP_FILTER_DEFAULT,mipmapWrap=DDS_MIPMAP_WRAP_DEFAULT):
	# source = ctypes.c_char_p(sourceFile)
	# dest = ctypes.c_char_p(destFile)

	# compression = ctypes.c_int(DDS_COMPRESS_BC1)
	# normalMap = ctypes.c_int(0)
	# mipmapFilter = ctypes.c_int(DDS_MIPMAP_FILTER_DEFAULT)
	# mipmapWrap = ctypes.c_int(DDS_MIPMAP_WRAP_DEFAULT)

	call_Compress_Texture2D_DDX(ctypes.c_char_p(sourceFile),
		ctypes.c_char_p(destFile),ctypes.c_int(compression),ctypes.c_int(normalMap),ctypes.c_int(mipmapFilter),ctypes.c_int(mipmapWrap))

#Compress_Texture2D_DXT("door.png","door.dds",DDS_COMPRESS_BC1)