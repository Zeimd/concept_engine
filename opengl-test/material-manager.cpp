/*****************************************************************************
*
* material-manager.cpp
*
* By Jari Korkala 27/5/2016
*
*****************************************************************************/

#include <fstream>

#include <ceng.h>

#include "material-manager.h"
#include "material-param.h"
#include "material.h"

#include "texture-options.h"
#include "texture-manager.h"

#include "shader-manager.h"
#include "shader-program.h"

using namespace CEngine;

void MoveFields(json &dest, json &source)
{
	for (auto iter = source.begin(); iter != source.end(); ++iter)
	{
		dest[iter.key()] = iter.value();
	}
}

MaterialManager::MaterialManager() : textureManager(nullptr),shaderManager(nullptr)
{
}

MaterialManager::MaterialManager(TextureManager *texManager, ShaderManager *shaderManager)
	: textureManager(texManager),shaderManager(shaderManager)
{
	MaterialFile temp;

	json texture;

	texture["file"] = "missing material data";
	texture["sRGB"] = true;
	texture["wrapMode"] = "repeat";

	temp.textureData["default"] = texture;

	Ceng::Log::Print(temp.textureData.dump());

	// Create default material

	json data;

	data["vs"] = "gbuffer.vs";
	data["fs"] = "gbuffer.fs";

	json flags;

	//flags.push_back("UNIFORM_COLOR");
	flags.push_back("MATERIAL_UNIFORM");
	flags.push_back("NO_NORMAL_MAP");

	data["fs_flags"] = flags;

	json color;

	color.push_back(0.5f);
	color.push_back(0.5f);
	color.push_back(0.5f);

	json texdef;

	texdef.push_back("tex2d");
	texdef.push_back("default");

	data["diffuseTex"] = texdef;

	//data["diffuseColor"] = color;
	
	data["materialRoughness"] = 0.5;
	data["materialMetallic"] = 0.04;

	Ceng::Log::Print(data.dump());

	CreateMaterial(temp, "__default__", data, defaultMaterial);
}

MaterialManager::~MaterialManager()
{
}

const EngineResult::value MaterialManager::LoadMaterialFile(const Ceng::StringUtf8 &file, Ceng::StringUtf8 &log)
{
	Ceng::StringUtf8 filename;

	Ceng_GetFilename(file, filename);

	Ceng::StringUtf8 readPath;

	Ceng_ApplyRelativePath(assetPath, file, readPath);

	auto iter = files.find(readPath);

	if (iter != files.end())
	{
		return EngineResult::ok;
	}

	std::ifstream fileReader;

	fileReader.open(readPath.ToCString(), std::ios_base::in);
	if (!fileReader.good())
	{
		log += "Error loading material file : ";
		log += file;
		log += " : file not found\n";

		return EngineResult::file_not_found;
	}

	if (fileReader.eof())
	{
		return EngineResult::fail;
	}

	json materialFile;

	try
	{
		materialFile = json(fileReader);
	}
	catch (std::exception &e)
	{
		log += "Error parsing material file : ";
		log += file;
		log += " : ";
		log += e.what();
		log += "\n";
		Ceng::Log::Print(log);
		return EngineResult::fail;
	}

	files[readPath] = MaterialFile();

	MaterialFile &dest = files[readPath];

	// Move data to global structure

	for (auto iter = materialFile.begin(); iter != materialFile.end();++iter)
	{
		if (iter.key() == "textures")
		{
			MoveFields(dest.textureData,iter.value());
		}
		else if (iter.key() == "materials")
		{
			MoveFields(dest.materialData,iter.value());
		}
	}

	return EngineResult::ok;
}

const EngineResult::value FindField(json &object, const std::string &name, json::iterator &out)
{
	out = object.find(name);

	if (out == object.end())
	{
		return EngineResult::fail;
	}

	return EngineResult::ok;
}


const EngineResult::value FindField(json &object,const std::string &name, const json::value_t type,json::iterator &out)
{
	/*
	std::string text = "FindField : ";
	text += name;
	text += '\n';

	Ceng::Log::Print(text);

	text = "Object : \n";
	text += object.dump();
	text += '\n';

	Ceng::Log::Print(text);
	*/

	out = object.find(name);

	if (out == object.end())
	{
//		Ceng::Log::Print("FindField : not found : " + name);
		return EngineResult::fail;
	}

	if ((*out).type() != type)
	{
//		Ceng::Log::Print("FindField : type mismatch");
		return EngineResult::not_supported;
	}

	return EngineResult::ok;
}

const EngineResult::value MaterialManager::CreateMaterial(MaterialFile &materialFile,
	const Ceng::StringUtf8 &name,json &material, std::shared_ptr<Material> &out)
{
	Ceng::StringUtf8 vsName;
	Ceng::StringUtf8 fsName;

	std::vector<Ceng::StringUtf8> vs_flags;
	std::vector<Ceng::StringUtf8> fs_flags;

	std::vector<json::iterator> params;

	std::string text;

	Ceng::StringUtf8 debugText;

	debugText = "\nmaterial : ";
	debugText += name;

	Ceng::Log::Print(debugText);

	EngineResult::value result;

	for (auto iter = material.begin(); iter != material.end(); ++iter)
	{
		if (iter.key() == "vs" && iter.value().is_string())
		{
			std::string temp = iter.value();
			vsName = temp;
		}
		else if (iter.key() == "fs" && iter.value().is_string())
		{
			std::string temp = iter.value();
			fsName = temp;
		}
		else if (iter.key() == "vs_flags" && (iter.value().is_array() || iter.value().is_string()))
		{
			for (auto nameIter = iter.value().begin(); nameIter != iter.value().end(); ++nameIter)
			{
				std::string temp = nameIter.value();
				vs_flags.push_back(temp);
			}
		}
		else if (iter.key() == "fs_flags" && (iter.value().is_array() || iter.value().is_string()))
		{
			for (auto nameIter = iter.value().begin(); nameIter != iter.value().end(); ++nameIter)
			{
				std::string temp = nameIter.value();
				fs_flags.push_back(temp);
			}
		}
		else
		{
			// Shader param

			text = iter.key();
			text += " : ";

			text += iter.value().dump();


			Ceng::Log::Print(text);

			if (iter.value().is_array())
			{
				json &array = iter.value();

				if (array.size() == 2 && array[0].is_string() && array[1].is_string())
				{

					text = "texture type : ";

					std::string name = array[0];

					text += name;
					text += '\n';

					text += "texture name : ";

					std::string ref = array[1];

					text += ref;

					Ceng::Log::Print(text);

					// texture
					json::iterator texIter;

					result = FindField(materialFile.textureData, array[1], json::value_t::object, texIter);
					if (result == EngineResult::ok)
					{
						result = SelectTexture(array[0], texIter);
					}
				}
				else
				{
					// array uniform
				}
			}
			else if (iter.value().is_number())
			{
				// uniform value
			}
			else
			{
				// invalid param
			}

			params.push_back(iter);
		}
	}

	// Create shader program

	std::shared_ptr<CEngine::ShaderProgram> program;

	result = shaderManager->CreateProgramFromFile(vsName, &vs_flags, fsName, &fs_flags, program);

	Ceng::StringUtf8 log = program->managerLog;

	if (result != EngineResult::ok)
	{
		Ceng::Log::Print(log);
		return result;
	}

	// Ensure that engine uniforms are available

	Ceng::ShaderProgram *shaderProgram = program->GetProgram();

	Ceng::ShaderConstant *vsObjectRotation;
	Ceng::ShaderConstant *vsFullTransform;

	Ceng::CRESULT cresult;

	cresult = shaderProgram->GetConstant("fullTransform", &vsFullTransform);

	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Error : shader program is missing engine uniform \"fullTransform\"");
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("eyeObjectRotation", &vsObjectRotation);

	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Error : shader program is missing engine uniform \"eyeObjectRotation\"");
		return EngineResult::fail;
	}

	// Create uniforms

	Ceng::UINT32 textureUnit = 0;

	std::vector<std::unique_ptr<MaterialParam>> finalParams;

	for (auto &x : params)
	{
		Ceng::StringUtf8 text;

		//text += x.key();
		//text += '\n';

		//text += x.value().dump();

		//Ceng::Log::Print(text);

		Ceng::ShaderConstant *uniform;

		Ceng::CRESULT cresult;

		std::string uniformName = x.key();

		cresult = program->GetProgram()->GetConstant(uniformName.c_str(), &uniform);

		if (cresult != Ceng::CE_OK)
		{
			continue;
		}


		if (x.value().is_array())
		{
			json &array = x.value();

			if (array.size() == 2 && array[0].is_string() && array[1].is_string())
			{
				// texture

				std::string name = array[1].get<std::string>();

				json::iterator texIter;
				result = FindField(materialFile.textureData, name, json::value_t::object, texIter);
				if (result != EngineResult::ok)
				{
					continue;
				}

				std::string temp = texIter.value()["file"];

				std::shared_ptr<CEngine::Texture> texture;

				CEngine::TextureOptions options;

				options.usage = Ceng::BufferUsage::gpu_read_only;
				options.cpuAccessFlags = 0;

				json::iterator sRGB;
				result = FindField(texIter.value(), "sRGB", json::value_t::boolean, sRGB);

				options.sRGB = true;
				if (result == EngineResult::ok)
				{
					options.sRGB = bool(sRGB.value());
				}

				options.mipLevels = 0;
				options.options = Ceng::BufferOptions::generate_mip_maps;
				options.bindFlags = Ceng::BufferBinding::shader_resource;

				result = textureManager->LoadTexture2D(temp, options, texture);

				Ceng::ShaderResourceViewDesc diffuseViewDesc;

				diffuseViewDesc.format = Ceng::IMAGE_FORMAT::C32_ABGR;
				diffuseViewDesc.dimensions = Ceng::BufferType::texture_2d;
				diffuseViewDesc.tex2d.baseMipLevel = 0;
				diffuseViewDesc.tex2d.maxMipLevel = -1;

				Ceng::ShaderResourceView *texView;

				cresult = texture->AsTexture2D()->GetShaderViewTex2D(diffuseViewDesc, &texView);

				finalParams.push_back(std::make_unique<MaterialTexture>(textureUnit, uniform, texture, texView));

				++textureUnit;
			}
			else
			{
				json::value_t type = json::value_t::number_integer;

				for (auto &item : array)
				{
					if (item.type() == json::value_t::number_float)
					{
						type = json::value_t::number_float;
					}
					else if (item.type() != json::value_t::number_integer)
					{
						type = item.type();
					}
				}

				if (type == json::value_t::number_integer)
				{
					switch (array.size())
					{
					case 1:
						finalParams.push_back(std::make_unique<MaterialInt>(Ceng::INT32(array[0]), uniform));
						break;
					default:
						break;
					}
				}
				else if (type == json::value_t::number_float)
				{
					Ceng::FLOAT32 temp[4];

					switch (array.size())
					{
					case 1:
						temp[0] = array[0];
						finalParams.push_back(std::make_unique<MaterialFloat>(temp, uniform));
						break;
					case 2:
						temp[0] = array[0];
						temp[1] = array[1];
						finalParams.push_back(std::make_unique<MaterialFloat2>(temp, uniform));
						break;
					case 3:
						temp[0] = array[0];
						temp[1] = array[1];
						temp[2] = array[2];
						finalParams.push_back(std::make_unique<MaterialFloat3>(temp, uniform));
						break;
					case 4:
						temp[0] = array[0];
						temp[1] = array[1];
						temp[2] = array[2];
						temp[3] = array[3];
						finalParams.push_back(std::make_unique<MaterialFloat4>(temp, uniform));
						break;
					default:
						break;
					}
				}
			}
		}
		else if (x.value().is_number_float())
		{
			// uniform float

			finalParams.push_back(std::make_unique<MaterialFloat>(Ceng::FLOAT32(x.value()), uniform));
		}
		else if (x.value().is_number_integer())
		{
			// uniform integer

			finalParams.push_back(std::make_unique<MaterialInt>(Ceng::INT32(x.value()), uniform));
		}
		else
		{
			// invalid param
		}

	}

	Material *out_material = new Material(program, std::move(finalParams), vsFullTransform, vsObjectRotation);

	out = std::shared_ptr<Material>(out_material);

	return EngineResult::ok;
}

const EngineResult::value MaterialManager::GetMaterial(const Ceng::StringUtf8 &file,
	const Ceng::StringUtf8 &name, std::shared_ptr<Material> &out)
{
	Ceng::StringUtf8 filename;

	Ceng_GetFilename(file, filename);

	Ceng::StringUtf8 readPath;

	Ceng_ApplyRelativePath(assetPath, file, readPath);

	if (auto iter = files.find(readPath) == files.end())
	{
		out = defaultMaterial;
		return EngineResult::ok;
	}

	MaterialFile &materialFile = files[readPath];

	// Check for existing material
	auto materialIter = materialFile.materials.find(name);

	if (materialIter != materialFile.materials.end())
	{
		out = (*materialIter).second;
		return EngineResult::ok;
	}

	json::iterator iter;

	EngineResult::value result;

	result = FindField(materialFile.materialData, name.ToCString(), json::value_t::object, iter);
	if (result != EngineResult::ok)
	{
		Ceng::StringUtf8 text;

		text = "Warning : Material not found : \"";
		text += name;
		text += "\"\nUsing signal material";

		Ceng::Log::Print(text);

		out = defaultMaterial;
		return EngineResult::ok;
	}

	json &material = iter.value();

	result = CreateMaterial(materialFile, name, material, out);

	if (result != EngineResult::ok)
	{
		out = defaultMaterial;
	}

	materialFile.materials[name] = out;

	return EngineResult::ok;
}

const EngineResult::value MaterialManager::SelectTexture(json &type,json::iterator &texIter)
{
	json &texture = texIter.value();

	EngineResult::value result;

	CEngine::TextureType::value typeValue;

	if (type == "tex2d")
	{
		typeValue = CEngine::TextureType::d2;
	}

	Ceng::BOOL sRGB = false;

	json::iterator iter;
	result = FindField(texture, "sRGB", json::value_t::boolean,iter);
	if (result == EngineResult::ok)
	{
		sRGB = bool(*iter);
	}

	Ceng::StringUtf8 debugText;
	debugText = "sRGB : ";
	debugText += bool(sRGB);

	json::iterator files;

	result = FindField(texture, "file", files);
	if (result != EngineResult::ok)
	{
		return result;
	}

	debugText += "\nfiles : ";
	debugText += files->dump();

	Ceng::Log::Print(debugText);

	/*
	std::string text = files.value().dump();

	Ceng::Log::Print(text);
	*/

	Ceng::IMAGE_FORMAT::value format;

	/*
	json validFile;

	for (auto &x : *files)
	{
		result = textureManager->PeekAndValidateFile(x, typeValue, sRGB, format);
		if (result == EngineResult::ok)
		{
			validFile = x;
		}
	}

	// Replace all potential files with the first supported texture file
	*files = validFile;
	*/

	if (files->is_string())
	{
		// Correct file has been selected
		return EngineResult::ok;
	}

	json temp = (*files)[0];

	*files = temp;

	return EngineResult::ok;
}


