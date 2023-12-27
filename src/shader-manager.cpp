/*****************************************************************************
*
* shader-manager.cpp
*
* By Jari Korkala 1/11/2015
*
*****************************************************************************/

#include <fstream>

#include <ceng/lib/liblog.h>

#include "shader-manager.h"
#include "shader.h"
#include "shader-program.h"

using namespace CEngine;

ShaderManager::ShaderManager()
{
}

ShaderManager::ShaderManager(Ceng::RenderDevice *renderDevice)
	: renderDevice(renderDevice)
{
}

ShaderManager::~ShaderManager()
{
}

const std::shared_ptr<Shader> ShaderManager::FindShader(const ShaderType::value type, const Ceng::StringUtf8 &file,
	const std::vector<Ceng::StringUtf8> *flags)
{
	auto iter = shaderList.begin();

	while (iter != shaderList.end())
	{
		if ((*iter)->type == type && (*iter)->name == file)
		{
			if (flags != nullptr)
			{
				// Match shader only if it uses exactly the same flags
				if ((*iter)->flags.size() == flags->size())
				{
					Ceng::UINT32 counter = 0;

					for (auto &flag : *flags)
					{
						auto flagIter = (*iter)->flags.find(flag);

						if (flagIter == (*iter)->flags.end())
						{
							break;
						}
						++counter;
					}

					if (counter == flags->size())
					{
						return *iter;
					}
				}
			}
			else if ((*iter)->flags.size() == 0)
			{
				return *iter;
			}
		}
		++iter;
	}

	return nullptr;
}

const EngineResult::value ShaderManager::LoadShader(const ShaderType::value type, const Ceng::StringUtf8 &file,
	const std::vector<Ceng::StringUtf8> *flags,std::shared_ptr<Shader> &output,Ceng::StringUtf8 &log)
{
	Ceng::StringUtf8 filename;

	Ceng_GetFilename(file, filename);

	auto find = FindShader(type, filename,flags);

	if (find != nullptr)
	{
		output = find;
		return EngineResult::ok;
	}

	Ceng::StringUtf8 shaderString = "vertex";

	if (type == ShaderType::pixel)
	{
		shaderString = "pixel";
	}

	// List for files that have already been included
	std::vector<Ceng::StringUtf8> excludeList;

	excludeList.push_back(filename);

	Ceng::StringUtf8 readPath;
	
	Ceng_ApplyRelativePath(assetPath, file, readPath);

	std::vector<std::ifstream> fileStack;

	std::ifstream fileReader;

	fileReader.open(readPath.ToCString(), std::ios_base::in);
	if (!fileReader.good())
	{
		log += "Error loading ";
		log += shaderString;
		log += " shader : ";
		log += readPath;
		log += " : ";

		char buffer[128];

		errno_t res = strerror_s(buffer, 128, errno);
		
		if (res == 0)
		{
			log += buffer;
		}
		else
		{
			log += "No mapping for error code: " + errno;
		}
		
		log += '\n';

		return EngineResult::file_not_found;
	}

	if (fileReader.eof())
	{
		return EngineResult::fail;
	}

	fileStack.push_back(std::move(fileReader));

	std::ifstream *reader = &fileStack.back();

	std::string line;

	std::vector<Ceng::StringUtf8> lineStack;

	Ceng::StringUtf8 shaderText;
	Ceng::StringUtf8 lineText;

	// Prefix flags

	do
	{
		try
		{
			std::getline(*reader, line);
		}
		catch (std::bad_alloc&)
		{
			return EngineResult::out_of_memory;
		}

		if (!reader->eof() && !reader->good())
		{
			log += "Error loading ";
			log += shaderString;
			log += " shader : ";
			log += " : io error\n";
			return EngineResult::io_error;
		}

		// Test for include directive
		lineText = line;

		auto iter = lineText.FindFirstNotOf(" \t", lineText.ConstBeginIterator());;

		Ceng::BOOL includeFlag = false;

		if (*iter == '#')
		{
			auto start = ++iter;
			auto end = lineText.FindFirstOf(" \t\"<", start);

			if (lineText.SubString(start, end) == "include")
			{
				start = end;

				if (*end == ' ' || *end == '\t')
				{
					start = lineText.FindFirstNotOf(" \t",start);

					start = lineText.FindFirstOf("\"<", start);
				}

				Ceng::CHAR32 separator = Ceng::CHAR32(*start);

				++start;

				end = lineText.FindFirstOf("\">", start);

				Ceng::BOOL syntaxError = false;

				if (separator == '\"' && *end != '\"')
				{
					syntaxError = true;

					log += "Error loading ";
					log += shaderString;
					log += " shader : ";
					log += file;
					log += "include statement syntax error\n";
				}
				else if (separator == '<' && *end != '>')
				{
					syntaxError = true;
	
					log += "Error loading ";
					log += shaderString;
					log += " shader : ";
					log += file;
					log += "include statement syntax error\n";
				}

				if (!syntaxError)
				{
					Ceng::StringUtf8 includeName = lineText.SubString(start, end);

					Ceng_ApplyRelativePath(assetPath, includeName, readPath);

					Ceng_GetFilename(includeName, filename);

					// Check that the file hasn't been already included
					Ceng::UINT32 excluded = false;

					for (Ceng::UINT32 k = 0; k < excludeList.size(); ++k)
					{
						if (excludeList[k] == filename)
						{
							excluded = true;
							break;
						}
					}

					if (!excluded)
					{
						excludeList.push_back(filename);

						fileReader.open(readPath.ToCString(), std::ios_base::in);
						if (!fileReader.good())
						{
							log += "Error loading ";
							log += shaderString;
							log += " shader : ";
							log += includeName;
							log += " : file not found\n";

							return EngineResult::file_not_found;
						}

						fileStack.push_back(std::move(fileReader));

						reader = &fileStack.back();

						includeFlag = true;
					}
				}
			}
		}
		
		if (!includeFlag)
		{
			try
			{
				shaderText += lineText;
				shaderText += '\n';
			}
			catch (std::bad_alloc&)
			{
				return EngineResult::out_of_memory;
			}
		}

		if (reader->eof())
		{
			fileStack.pop_back();

			if (fileStack.size() == 0)
			{
				break;
			}

			reader = &fileStack.back();
		}

	} while(1);

	// Find version directive

	auto iter = shaderText.FindFirst("#version", shaderText.ConstBeginIterator());

	iter = shaderText.FindFirst('\n', iter);

	++iter;

	// Add shader flags

	Ceng::StringUtf8 flagText;

	if (flags != nullptr)
	{
		for (auto &flag : *flags)
		{
			flagText += "#define ";
			flagText += flag;
			flagText += '\n';
		}
	}

	shaderText.Insert(iter.RawPosition(), flagText);

	//Ceng::Log::Print(shaderText);

	Ceng::CRESULT cresult;

	output = nullptr;

	std::shared_ptr<Shader> data;

	switch (type)
	{
	case ShaderType::vertex:

		if (1)
		{
			Ceng::VertexShader *vshader;

			cresult = renderDevice->CreateVertexShader(shaderText, &vshader);

			if (cresult != Ceng::CE_OK)
			{
				log += "Error loading ";
				log += shaderString;
				log += " shader : ";

				switch (cresult)
				{
				case Ceng::CE_ERR_OUT_OF_MEMORY:
					return EngineResult::out_of_memory;
					break;
				case Ceng::CE_ERR_FAIL:
					log += " : failed to create shader object\n";
					return EngineResult::fail;
					break;
				case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
					log += " : out of memory\n";
					return EngineResult::out_of_video_memory;
					break;
				}

				return EngineResult::fail;

				return EngineResult::fail;
			}

			VertexShader *vs;
			
			try
			{
				vs = new VertexShader();
			}
			catch (std::bad_alloc&)
			{
				vshader->Release();
				return EngineResult::out_of_memory;
			}

			vs->type = type;
			vs->name = file;
			vs->vshader = vshader;

			try
			{
				data = std::shared_ptr<Shader>(vs);
			}
			catch (std::bad_alloc&)
			{
				vshader->Release();
				return EngineResult::out_of_memory;
			}
		}

		break;
	case ShaderType::pixel:

		if(1)
		{
			Ceng::PixelShader *pshader;

			cresult = renderDevice->CreatePixelShader(shaderText, &pshader);

			if (cresult != Ceng::CE_OK)
			{
				log += "Error loading ";
				log += shaderString;
				log += " shader : ";

				switch (cresult)
				{
				case Ceng::CE_ERR_OUT_OF_MEMORY:
					return EngineResult::out_of_memory;
					break;
				case Ceng::CE_ERR_FAIL:
					log += " : failed to create shader object\n";
					return EngineResult::fail;
					break;
				case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
					log += " : out of memory\n";
					return EngineResult::out_of_video_memory;
					break;
				}

				return EngineResult::fail;
			}

			PixelShader *ps;
			
			try
			{
				ps = new PixelShader();
			}
			catch (std::bad_alloc&)
			{
				pshader->Release();
				return EngineResult::out_of_memory;
			}

			ps->type = type;
			ps->name = file;
			ps->pshader = pshader;

			try
			{
				data = std::shared_ptr<Shader>(ps);
			}
			catch (std::bad_alloc&)
			{
				pshader->Release();
				return EngineResult::out_of_memory;
			}
		}

		break;
	}

	try
	{
		data->AddFlags(flags);
		shaderList.push_back(data);
	}
	catch (std::bad_alloc&)
	{
		return EngineResult::out_of_memory;
	}

	output = data;

	return EngineResult::ok;
}

const EngineResult::value ShaderManager::CreateProgramFromFile(const Ceng::StringUtf8 &vshaderFile,
	const Ceng::StringUtf8 &pshaderFile,
	std::shared_ptr<CEngine::ShaderProgram> &program)
{
	return CreateProgramFromFile(vshaderFile, nullptr, pshaderFile, nullptr, program);
}

const EngineResult::value ShaderManager::CreateProgramFromFile(const Ceng::StringUtf8 &vshaderFile,
	const std::vector<Ceng::StringUtf8> *vsFlags,
	const Ceng::StringUtf8 &pshaderFile, const std::vector<Ceng::StringUtf8> *psFlags,
	std::shared_ptr<CEngine::ShaderProgram> &program)
{
	Ceng::StringUtf8 log;
	Ceng::CRESULT cresult;

	program = nullptr;

	EngineResult::value eresult;

	std::shared_ptr<Shader> vshader;

	eresult = LoadShader(ShaderType::vertex, vshaderFile,vsFlags,vshader,log);
	if (eresult == EngineResult::out_of_memory)
	{
		return eresult;
	}

	Ceng::BOOL vsError = true;

	if (vshader != nullptr)
	{
		Ceng::VertexShader *vertexShader = vshader->AsVertexShader();

		if (vertexShader != nullptr)
		{
			if (!vertexShader->Compiled())
			{
				log += "Error compiling vertex shader : ";
				log += vshaderFile;
				log += '\n';

				Ceng::StringUtf8 *shaderLog;

				cresult = vertexShader->GetLog(&shaderLog);

				log += *shaderLog;

				delete shaderLog;
			}
			else
			{
				vsError = false;
			}
		}
	}

	std::shared_ptr<Shader> pshader;

	eresult = LoadShader(ShaderType::pixel, pshaderFile, psFlags,pshader,log);
	if (eresult == EngineResult::out_of_memory)
	{
		return eresult;
	}

	Ceng::BOOL psError = true;

	if (pshader != nullptr)
	{
		Ceng::PixelShader *pixelShader = pshader->AsPixelShader();

		if (pixelShader != nullptr)
		{
			if (!pixelShader->Compiled())
			{
				log += "Error compiling pixel shader : ";
				log += pshaderFile;
				log += '\n';

				Ceng::StringUtf8 *shaderLog;

				cresult = pixelShader->GetLog(&shaderLog);

				log += *shaderLog;

				delete shaderLog;
			}
			else
			{
				psError = false;
			}
		}
	}

	auto find = FindProgram(vshader, pshader);

	if (find != nullptr)
	{
		program = find;
		return EngineResult::ok;
	}

	Ceng::BOOL build = true;

	if (vsError == true)
	{
		log += "Error linking shader program : ";
		log += vshaderFile;
		log += " , ";
		log += pshaderFile;
		log += " :\n";
		log += "vertex shader is not valid\n";

		build = false;
	}

	if (psError == true)
	{
		log += "Error linking shader program : ";
		log += vshaderFile;
		log += " , ";
		log += pshaderFile;
		log += " :\n";
		log += "pixel shader is not valid\n";

		build = false;
	}

	if (build == false)
	{
		// Store results as an invalid shader program so that error messages won't be repeated

		ShaderProgram *temp = new ShaderProgram();

		temp->vertexShader = vshader;
		temp->pixelShader = pshader;
		temp->program = nullptr;
		temp->managerLog = log;

		program = std::shared_ptr<ShaderProgram>(temp);

		programList.push_back(program);
		return EngineResult::fail;
	}

	Ceng::ShaderProgram *temp_program;

	cresult = renderDevice->CreateShaderProgram(vshader->AsVertexShader(), pshader->AsPixelShader(), &temp_program);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			log += "Error linking shader program : ";
			log += vshaderFile;
			log += " , ";
			log += pshaderFile;
			log += " :\n";
			log += "out of video memory\n";
			break;
		case Ceng::CE_ERR_FAIL:
			log += "Error linking shader program : ";
			log += vshaderFile;
			log += " , ";
			log += pshaderFile;
			log += " :\n";
			log += "failed to create program object\n";
			break;
		case Ceng::CE_ERR_OUT_OF_MEMORY:
			return EngineResult::out_of_memory;
		default:
			break;
		}
	}

	if (temp_program != nullptr)
	{
		if (!temp_program->Linked())
		{
			log += "Error linking shader program : ";
			log += vshaderFile;
			log += " , ";
			log += pshaderFile;
			log += " :\n";

			Ceng::StringUtf8 *shaderLog;

			cresult = temp_program->GetLog(&shaderLog);

			log += *shaderLog;

			delete shaderLog;

			return EngineResult::fail;
		}
	}

	ShaderProgram *temp;

	try
	{
		temp = new ShaderProgram();
	}
	catch (std::bad_alloc&)
	{
		temp_program->Release();
		return EngineResult::out_of_memory;
	}

	temp->vertexShader = vshader;
	temp->pixelShader = pshader;
	temp->program = temp_program;
	temp->managerLog = log;

	try
	{
		program = std::shared_ptr<ShaderProgram>(temp);
	}
	catch (std::bad_alloc&)
	{
		temp_program->Release();
		return EngineResult::out_of_memory;
	}

	try
	{
		programList.push_back(program);
	}
	catch (std::bad_alloc&)
	{
		return EngineResult::out_of_memory;
	}

	return EngineResult::ok;
}

const std::shared_ptr<ShaderProgram> ShaderManager::FindProgram(std::shared_ptr<Shader> &vertexShader, 
	std::shared_ptr<Shader> &pixelShader)
{
	auto iter = programList.begin();

	while (iter != programList.end())
	{
		if ((*iter)->vertexShader == vertexShader && (*iter)->pixelShader == pixelShader)
		{
			return *iter;
		}

		++iter;
	}

	return nullptr;
}

void ShaderManager::PruneShaderList()
{
	auto iter = shaderList.begin();

	while (iter != shaderList.end())
	{
		if ( (*iter).use_count() == 1)
		{
			auto previous = iter;
			++iter;
			shaderList.erase(previous);
		}
	}
}


void ShaderManager::PruneProgramList()
{
	auto iter = programList.begin();

	while (iter != programList.end())
	{
		if ((*iter).use_count() == 1)
		{
			auto previous = iter;
			++iter;
			programList.erase(previous);
		}
	}
}

