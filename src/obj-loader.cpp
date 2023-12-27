/*****************************************************************************
*
* obj-loader.cpp
*
* Created By Jari Korkala 29/10/2015
*
* Loads mesh from Waveform Object text file.
*
*****************************************************************************/

#define _CRT_SECURE_NO_DEPRECATE

#include <fstream>

#include <ceng/lib/timerlib.h>

#include "mesh.h"
#include "material-manager.h"

#include "mesh-manager.h"
#include "mesh-action.h"
#include "math-vector.h"

using namespace CEngine;

struct Vertex
{
	Ceng::INT32 position;
	Ceng::INT32 uvw;
	Ceng::INT32 lmapUV;
	Ceng::INT32 normal;
	Ceng::INT32 tangent;

	Ceng::INT32 unpack;

	const bool operator == (const Vertex &other) const
	{
		if (position != other.position) return false;

		if (uvw != other.uvw) return false;

		if (lmapUV != other.lmapUV) return false;

		if (normal != other.normal) return false;

		return true;
	}
};

template<>
struct std::hash<Vertex>
{
	std::size_t operator() (const Vertex &param) const
	{
		const unsigned int seed = 0xB0F57EE3;

		std::size_t result;

		Ceng::UINT32 temp[] = 
		{ 
			Ceng::UINT32(param.position),
			Ceng::UINT32(param.uvw),
			Ceng::UINT32(param.normal) 
		};

		MurmurHash3_x86_32(temp, 3*sizeof(Ceng::UINT32), seed, &result);

		return result;
	}
};


struct Face
{
	Vertex vertex[3];
	Ceng::UINT32 material;
};

struct DrawCall
{
	/**
	* Index buffer element to start drawing from.
	*/
	Ceng::UINT32 firstIndex;

	/**
	* Number of continuous indices to use.
	*/
	Ceng::UINT32 indexCount;

	/**
	* Vertex buffer to enable for this set.
	*/
	Ceng::UINT32 vertexBufferId;

	/**
	* Index buffer to enable for this set.
	*/
	Ceng::UINT32 indexBufferId;
};

struct MaterialGroup
{
	Ceng::StringUtf8 name;
	std::vector<Face> faces;

	std::vector<DrawCall> calls;

	std::shared_ptr<Material> material;
};



struct Unpack
{
	std::vector<Vertex> vertex;

	std::vector<Vec3> tangent;
};

struct ProtoMesh
{
	std::vector<Vec3> position;
	std::vector<Vec3> normal;

	std::vector<Vec3> tangent;

	Ceng::UINT32 texCoords;

	// Texture coordinates in separate vectors to minimize storage space for 1d and 2d
	// textures
	std::vector<Vec3> uvw;

	std::vector<MaterialGroup> materialGroups;
};

const Vec3 Normalize(const Vec3 &source)
{
	Ceng::FLOAT32 factor = 1.0f / sqrt(source.x*source.x +
		source.y*source.y + source.z*source.z);

	Vec3 temp;

	temp.x = factor * source.x;
	temp.y = factor * source.y;
	temp.z = factor * source.z;

	return temp;
}

int ScanVec3(const char *source, std::vector<Vec3> &out)
{
	Vec3 temp;

	int scanResult =  sscanf(source, "%f %f %f", &temp.x, &temp.y, &temp.z);

	if (scanResult < 3)
	{
		return -1;
	}

	out.push_back(temp);

	return 0;
}

int ScanVec2(const char *source, std::vector<Vec3> &out)
{
	Vec3 temp;

	temp.z = 0.0f;

	int scanResult = sscanf(source, "%f %f", &temp.x, &temp.y);

	if (scanResult < 2)
	{
		return -1;
	}

	out.push_back(temp);

	return 0;
}


const Ceng::INT32 FindMatchingVertex(const Unpack &unpack, const ProtoMesh &mesh, const Vertex &vertex);

const std::vector<std::string> Split(const std::string &source, const char separator);

const ObjectError::value MeshManager::LoadMesh_OBJ
(const Ceng::StringUtf8 &filename, const Ceng::StringUtf8 &materialFile,const Ceng::StringUtf8 &objectName,
	std::shared_ptr<Mesh> &out)
{
	std::ifstream fileReader;

	fileReader.open(filename.ToCString(), std::ios::in);
	if (!fileReader.good())
	{
		return ObjectError::file_not_found;
	}

	Ceng::Log::Print("OBJ loader:");

	Ceng::StringUtf8 text;

	text = "file = ";
	text += filename;
	text += '\n';

	Ceng::Log::Print(text);

	std::string line,word;

	std::stringstream wordStream;

	Ceng::UINT32 currentMaterial = 0;

	Ceng::UINT32 foundObject = 0;

	ProtoMesh mesh;

	Ceng::FLOAT64 start = Ceng_HighPrecisionTimer();

	Ceng::UINT32 faceCount = 0;

	Ceng::BOOL texCoord = true;

	while(!fileReader.eof())
	{
		std::getline(fileReader,line);
		if (!fileReader.good())
		{
			if (fileReader.bad())
			{
				return ObjectError::io_error;
			}
		}

		if (line.length() == 0)
		{
			continue;
		}

		// Skip comments
		if (line[0] == '#')
		{
			continue;
		}

		if (line.length() < 2)
		{
			continue;
		}

		int scanResult;
		if (line[0] == 'o')
		{
			int scanResult = strcmp(&line[2], objectName.ToCString());

			if (scanResult == 0)
			{
				if (foundObject == 1)
				{
					break;
				}
				foundObject = 1;
			}
			continue;
		}

		if (foundObject == 0) continue;

		if (line.length() >= 2)
		{
			if (line[0] == 'v' && line[1] == 'n')
			{
				scanResult = ScanVec3(&line[2], mesh.normal);

				if (scanResult != 0)
				{
					break;
				}
				continue;
			}
			else if (line[0] == 'v' && line[1] == 't')
			{
				scanResult = ScanVec2(&line[2], mesh.uvw);

				if (scanResult == -1)
				{
					break;
				}

				mesh.texCoords = scanResult;
				continue;
			}

		}

		if (line[0] == 'v')
		{
			scanResult = ScanVec3(&line[2], mesh.position);

			if (scanResult != 0)
			{
				break;
			}
			continue;
		}
		else if (line[0] == 'f')
		{
			Face face;

			face.material = currentMaterial;

			Ceng::INT32 v[3], n[3], texUV[3],lmapUV[3];

			scanResult = sscanf(&line[2], "%d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d", 
				&v[0], &texUV[0], &lmapUV[0],&n[0],
				&v[1], &texUV[1], &lmapUV[1],&n[1],
				&v[2], &texUV[2], &lmapUV[2],&n[2]);

			if (scanResult < 12)
			{
				scanResult = sscanf(&line[2], "%d/%d/%d %d/%d/%d %d/%d/%d", 
					&v[0], &texUV[0], &n[0],
					&v[1], &texUV[1], &n[1], 
					&v[2], &texUV[2], &n[2]);

				lmapUV[0] = 0;
				lmapUV[1] = 0;
				lmapUV[2] = 0;
			}

			if (scanResult < 9)
			{
				texCoord = false;
				scanResult = sscanf(&line[2], "%d//%d %d//%d %d//%d", 
					&v[0], &n[0],
					&v[1], &n[1],
					&v[2], &n[2]);

				texUV[0] = 0;
				texUV[1] = 0;
				texUV[2] = 0;

				if (scanResult < 6)
				{
					continue;
				}
			}

			Ceng::BOOL invalid = false;

			for (Ceng::UINT32 vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
			{
				face.vertex[vertexIndex].position = v[vertexIndex] - 1;
				face.vertex[vertexIndex].normal = n[vertexIndex] - 1;
				face.vertex[vertexIndex].uvw = texUV[vertexIndex] - 1;
				face.vertex[vertexIndex].lmapUV = lmapUV[vertexIndex] - 1;

				if (size_t(face.vertex[vertexIndex].position) >= mesh.position.size())
				{
					invalid = true;
					break;
				}

				if (size_t(face.vertex[vertexIndex].normal) >= mesh.normal.size())
				{
					invalid = true;
					break;
				}

				if (texCoord)
				{
					if (size_t(face.vertex[vertexIndex].uvw) >= mesh.uvw.size())
					{
						invalid = true;
						break;
					}

					if (face.vertex[vertexIndex].lmapUV >= 0 &&
					    size_t(face.vertex[vertexIndex].lmapUV) >= mesh.uvw.size())
					{
						invalid = true;
						break;
					}
				}
			}

			if (invalid) continue;

			mesh.materialGroups[currentMaterial].faces.push_back(face);
			++faceCount;
			continue;
		}

		if (line.length() < 6) continue;

		if (strncmp(&line[0], "usemtl",6) == 0)
		{
			Ceng::UINT32 k = 0;
			for (k = 0; k < mesh.materialGroups.size(); ++k)
			{
				scanResult = strcmp(&line[7], mesh.materialGroups[k].name.ToCString());

				if (scanResult == 0)
				{
					break;
				}
			}

			if (k == mesh.materialGroups.size())
			{
				// Found new material name
				MaterialGroup group;
				group.name = line.substr(7);
				mesh.materialGroups.push_back(std::move(group));
			}
			currentMaterial = k;
			continue;
		}
	};

	Ceng::FLOAT64 end = Ceng_HighPrecisionTimer();

	text = "parsing time = ";
	text += end - start;
	text += '\n';

	Ceng::Log::Print(text);

	/////////////////////////////////////////////////////////////////////////////
	// This part is common for all loaders

	// Calculate tangent basis

	start = Ceng_HighPrecisionTimer();

	if (texCoord)
	{
		for (auto &group : mesh.materialGroups)
		{
			for (auto &face : group.faces)
			{
				Vec3 &vertex0 = mesh.position[face.vertex[0].position];
				Vec3 &vertex1 = mesh.position[face.vertex[1].position];
				Vec3 &vertex2 = mesh.position[face.vertex[2].position];

				Ceng::FLOAT32 &u0 = mesh.uvw[face.vertex[0].uvw].x;
				Ceng::FLOAT32 &u1 = mesh.uvw[face.vertex[1].uvw].x;
				Ceng::FLOAT32 &u2 = mesh.uvw[face.vertex[2].uvw].x;

				Ceng::FLOAT32 &v0 = mesh.uvw[face.vertex[0].uvw].y;
				Ceng::FLOAT32 &v1 = mesh.uvw[face.vertex[1].uvw].y;
				Ceng::FLOAT32 &v2 = mesh.uvw[face.vertex[2].uvw].y;

				// Edge deltas
				Vec3 deltaPos1;
				deltaPos1.x = vertex1.x - vertex0.x;
				deltaPos1.y = vertex1.y - vertex0.y;
				deltaPos1.z = vertex1.z - vertex0.z;

				Vec3 deltaPos2;
				deltaPos2.x = vertex2.x - vertex0.x;
				deltaPos2.y = vertex2.y - vertex0.y;
				deltaPos2.z = vertex2.z - vertex0.z;

				// UV delta

				TexelF2 deltaUV1, deltaUV2;

				deltaUV1.u = u1 - u0;
				deltaUV1.v = v1 - v0;

				deltaUV2.u = u2 - u0;
				deltaUV2.v = v2 - v0;

				Vec3 tangent;

				Ceng::FLOAT32 r = 1.0f / (deltaUV1.u * deltaUV2.v - deltaUV1.v * deltaUV2.u);

				tangent.x = r * (deltaPos1.x * deltaUV2.v - deltaPos2.x * deltaUV1.v);
				tangent.y = r * (deltaPos1.y * deltaUV2.v - deltaPos2.y * deltaUV1.v);
				tangent.z = r * (deltaPos1.z * deltaUV2.v - deltaPos2.z * deltaUV1.v);

				/*
				// Associated binormal
				binormal.x = r * (deltaPos2.x * deltaUV1.u - deltaPos1.x * deltaUV2.u);
				binormal.y = r * (deltaPos2.y * deltaUV1.u - deltaPos1.y * deltaUV2.u);
				binormal.z = r * (deltaPos2.z * deltaUV1.u - deltaPos1.z * deltaUV2.u);
				*/

				mesh.tangent.push_back(tangent);
		
				Ceng::INT32 index = mesh.tangent.size() - 1;

				face.vertex[0].tangent = index;
				face.vertex[1].tangent = index;
				face.vertex[2].tangent = index;
			}
		}
	}

	end = Ceng_HighPrecisionTimer();

	text = "tangent basis generation : ";
	text += end - start;
	text += " s\n";

	Ceng::Log::Print(text);

	// Convert to single index form

	std::unordered_map<Vertex, Ceng::UINT32> cache;

	Unpack unpack;

	start = Ceng_HighPrecisionTimer();

	for (auto &group : mesh.materialGroups)
	{
		for (auto &face : group.faces)
		{
			for (auto &vertex : face.vertex)
			{
				auto iter = cache.find(vertex);

				Ceng::INT32 index;

				if (iter == cache.end())
				{
					unpack.vertex.push_back(vertex);

					if (texCoord)
					{
						unpack.tangent.push_back(mesh.tangent[vertex.tangent]);
					}

					vertex.unpack = unpack.vertex.size() - 1;

					cache[vertex] = vertex.unpack;
				}
				else
				{
					index = (*iter).second;

					// Combine tangent spaces

					if (texCoord)
					{
						unpack.tangent[index].x += mesh.tangent[vertex.tangent].x;
						unpack.tangent[index].y += mesh.tangent[vertex.tangent].y;
						unpack.tangent[index].z += mesh.tangent[vertex.tangent].z;
					}

					vertex.unpack = index;
				}
			}
		}
	}

	end = Ceng_HighPrecisionTimer();

	text = "unpack time : ";
	text += end - start;
	text += " s\n";

	Ceng::Log::Print(text);

	start = Ceng_HighPrecisionTimer();

	for (Ceng::UINT32 k = 0; k < unpack.tangent.size(); ++k)
	{
		// Normalize tangent space

		unpack.tangent[k] = Normalize(unpack.tangent[k]);
	
		// Orthogonalize tangent in respect to normal

		Vec3 &normal = mesh.normal[unpack.vertex[k].normal];

		Ceng::FLOAT32 dot = unpack.tangent[k].x*normal.x +
			unpack.tangent[k].y*normal.y + unpack.tangent[k].z*normal.z;

		unpack.tangent[k].x = unpack.tangent[k].x - dot * normal.x;
		unpack.tangent[k].y = unpack.tangent[k].y - dot * normal.y;
		unpack.tangent[k].z = unpack.tangent[k].z - dot * normal.z;
	}

	end = Ceng_HighPrecisionTimer();

	text = "tangent space normalize : ";
	text += end - start;
	text += " s\n";

	Ceng::Log::Print(text);
	

	// Configure streams

	Ceng::INT32 positionItem = FindVertexVar("position");
	Ceng::INT32 normalItem = FindVertexVar("normal");
	
	Ceng::INT32 tangentItem = FindVertexVar("tangent");

	Ceng::INT32 uvItem = FindVertexVar("textureUV");

	Ceng::INT32 lmapItem = FindVertexVar("lightmapUV");

	Ceng::UINT32 vertexSize = vertexDecl[0].stride;

	std::vector<std::vector<Ceng::UINT32>> vertexData;
	std::vector<std::vector<Ceng::UINT32>> indexData;

	vertexData.push_back(std::vector<Ceng::UINT32>());
	indexData.push_back(std::vector<Ceng::UINT32>());

	std::vector<Ceng::UINT32> *vertexBuffer = &vertexData[0];

	std::vector<Ceng::UINT32> *indexBuffer = &indexData[0];

	DrawCall currentCall;

	currentCall.vertexBufferId = 0;
	currentCall.indexBufferId = 0;

	currentCall.firstIndex = 0;
	currentCall.indexCount = 0;

	// Convert to multiple index and vertex buffers

	std::unordered_map<Ceng::UINT32, Ceng::UINT32> vertexCache;

	start = Ceng_HighPrecisionTimer();

	for (auto &group : mesh.materialGroups)
	{
		for (auto &face : group.faces)
		{
			if (vertexBuffer->size() + 3 > vBufferMax)
			{
				// Push current call because vertex buffer has to be changed
				
				group.calls.push_back(currentCall);
				currentCall.firstIndex += currentCall.indexCount;
				currentCall.indexCount = 0;

				vertexData.push_back(std::vector<Ceng::UINT32>());

				vertexBuffer = &vertexData.back();

				++currentCall.vertexBufferId;

				vertexCache.clear();
			}

			if (indexBuffer->size() + 3 > iBufferMax)
			{
				// Push current call because index buffer has to be changed
				group.calls.push_back(currentCall);

				currentCall.firstIndex = 0;
				currentCall.indexCount = 0;

				indexData.push_back(std::vector<Ceng::UINT32>());

				indexBuffer = &indexData.back();

				++currentCall.indexBufferId;
			}

			for (auto &vertex : face.vertex)
			{
				auto iter = vertexCache.find(vertex.unpack);

				if (iter == vertexCache.end())
				{
					vertexBuffer->push_back(vertex.unpack);
					indexBuffer->push_back(vertexBuffer->size() - 1);

					vertexCache[vertex.unpack] = indexBuffer->back();
				}
				else
				{
					Ceng::INT32 index = (*iter).second;
					indexBuffer->push_back(index);
				}
			}

			currentCall.indexCount += 3;
		}

		// Always stop draw call group when material changes

		group.calls.push_back(currentCall);

		currentCall.firstIndex += currentCall.indexCount;
		currentCall.indexCount = 0;
	}

	end = Ceng_HighPrecisionTimer();

	text = "buffer allocation : ";
	text += end - start;
	text += " s\n";

	Ceng::Log::Print(text);

	std::shared_ptr<Mesh> finalMesh = std::make_shared<Mesh>();

	// Construct index buffers

	Ceng::CRESULT cresult;

	start = Ceng_HighPrecisionTimer();

	for (auto &data : indexData)
	{
		// Construct initial data
		std::vector<Ceng::UINT8> indexByte;
		std::vector<Ceng::UINT16> indexWord;

		void *initialData = &data[0];

		switch (maxIndexBytes)
		{
		case 1:
			for (auto &index : data)
			{
				indexByte.push_back(index);
			}
			initialData = &indexByte[0];
			break;
		case 2:
			for (auto &index : data)
			{
				indexWord.push_back(index);
			}
			initialData = &indexWord[0];
			break;
		}

		Ceng::IndexBuffer *indexBuffer;

		cresult = renderDevice->CreateIndexBuffer(maxIndexBytes, data.size(), 
			Ceng::BufferUsage::gpu_read_only, initialData, &indexBuffer);

		if (cresult != Ceng::CE_OK)
		{
			return ObjectError::fail;
		}

		finalMesh->indexBuffers.push_back(indexBuffer);
	}

	end = Ceng_HighPrecisionTimer();

	text = "index buffer generation : ";
	text += end - start;
	text += " s\n";

	Ceng::Log::Print(text);

	// Construct vertex buffers

	std::vector<Ceng::VertexBuffer*> vertexBuffers;

	start = Ceng_HighPrecisionTimer();

	for (auto &data : vertexData)
	{
		std::vector<Ceng::UINT8> initialData(data.size()*vertexSize);

		Ceng::UINT32 pos = 0;

		for (auto &vertex : data)
		{
			Vec3 &position = mesh.position[unpack.vertex[vertex].position];
			Vec3 &normal = mesh.normal[unpack.vertex[vertex].normal];

			memcpy(&initialData[pos + vertexDecl[positionItem].offset], &position, sizeof(Vec3));

			memcpy(&initialData[pos + vertexDecl[normalItem].offset], &normal, sizeof(Vec3));

			if (texCoord)
			{
				memcpy(&initialData[pos + vertexDecl[tangentItem].offset], &unpack.tangent[vertex], sizeof(Vec3));

				Ceng::FLOAT32 &u = mesh.uvw[unpack.vertex[vertex].uvw].x;
				Ceng::FLOAT32 &v = mesh.uvw[unpack.vertex[vertex].uvw].y;

				memcpy(&initialData[pos + vertexDecl[uvItem].offset], &u, sizeof(Ceng::FLOAT32));
				memcpy(&initialData[pos + vertexDecl[uvItem].offset + 4], &v, sizeof(Ceng::FLOAT32));

				if (unpack.vertex[vertex].lmapUV >= 0)
				{
					u = mesh.uvw[unpack.vertex[vertex].lmapUV].x;
					v = mesh.uvw[unpack.vertex[vertex].lmapUV].y;

					memcpy(&initialData[pos + vertexDecl[lmapItem].offset], &u, sizeof(Ceng::FLOAT32));
					memcpy(&initialData[pos + vertexDecl[lmapItem].offset + 4], &v, sizeof(Ceng::FLOAT32));
				}
			}

			pos += vertexSize;
		}

		Ceng::VertexBuffer *vertexBuffer;

		cresult = renderDevice->CreateVertexBuffer(vertexSize,data.size(), Ceng::BufferUsage::gpu_read_only, &vertexBuffer);
		if (cresult != Ceng::CE_OK)
		{
			return ObjectError::fail;
		}

		cresult = vertexBuffer->LoadData(&initialData[0]);
		if (cresult != Ceng::CE_OK)
		{
			return ObjectError::fail;
		}

		finalMesh->vertexBuffers.push_back(vertexBuffer);
	}

	end = Ceng_HighPrecisionTimer();

	text = "vertex buffer generation : ";
	text += end - start;
	text += " s\n";

	Ceng::Log::Print(text);

	// Load material file

	EngineResult::value eresult;

	Ceng::StringUtf8 materialLog;

	eresult = materialManager->LoadMaterialFile(materialFile, materialLog);

	// Load materials

	for (auto &group : mesh.materialGroups)
	{
		eresult = materialManager->GetMaterial(materialFile,group.name, group.material);

		if (eresult != EngineResult::ok)
		{
			Ceng::Log::Print(Ceng::StringUtf8("Failed to load material : ") + group.name);
		}
	}

	// Generate draw commands

	start = Ceng_HighPrecisionTimer();

	finalMesh->commands.push_back(std::make_unique<VertexFormatChange>(vertexFormat));
	finalMesh->shadowCommands.push_back(std::make_unique<VertexFormatChange>(vertexFormat));

	Ceng::INT32 currentVB = -1;
	Ceng::INT32 currentIB = -1;

	for (auto &group : mesh.materialGroups)
	{
		if (group.material == nullptr) continue;

		finalMesh->commands.push_back(std::make_unique<MaterialChange>(group.material));

		for (auto &call : group.calls)
		{
			if (call.indexBufferId != currentIB)
			{
				currentIB = call.indexBufferId;

				finalMesh->commands.push_back(
					std::make_unique<IndexBufferChange>(finalMesh->indexBuffers[call.indexBufferId]));
				
				finalMesh->shadowCommands.push_back(
					std::make_unique<IndexBufferChange>(finalMesh->indexBuffers[call.indexBufferId]));
			}

			if (call.vertexBufferId != currentVB)
			{
				currentVB = call.vertexBufferId;

				finalMesh->commands.push_back(
					std::make_unique<VertexBufferChange>(finalMesh->vertexBuffers[call.vertexBufferId],vertexSize));
				
				finalMesh->shadowCommands.push_back(
					std::make_unique<VertexBufferChange>(finalMesh->vertexBuffers[call.vertexBufferId], vertexSize));
			}

			finalMesh->commands.push_back(std::make_unique<DrawCommand>(Ceng::PRIMITIVE_TYPE::TRIANGLE_LIST,
				call.firstIndex, call.indexCount));

			finalMesh->shadowCommands.push_back(std::make_unique<DrawCommand>(Ceng::PRIMITIVE_TYPE::TRIANGLE_LIST,
				call.firstIndex, call.indexCount));
		}
	}

	end = Ceng_HighPrecisionTimer();

	text = "draw command generation : ";
	text += end - start;
	text += " s\n";

	Ceng::Log::Print(text);

	out = finalMesh;

	return ObjectError::ok;
}

const Ceng::INT32 FindMatchingVertex(const Unpack &unpack,const ProtoMesh &mesh,const Vertex &vertex)
{
	for (Ceng::UINT32 k = 0; k < unpack.vertex.size();++k)
	{
		if (unpack.vertex[k].position != vertex.position) continue;

		if (unpack.vertex[k].normal != vertex.normal) continue;

		if (unpack.vertex[k].uvw != vertex.uvw) continue;

		if (unpack.vertex[k].lmapUV != vertex.lmapUV) continue;

		return k;
	}

	return -1;
}

const std::vector<std::string> Split(const std::string &source, const char separator)
{
	std::vector<std::string> output;

	auto iter = source.begin();

	while (iter != source.end())
	{
		std::size_t start = std::distance(source.begin(), iter);
		std::size_t end = source.find(separator, start);

		output.push_back(source.substr(start, end-start));

		std::size_t dist = std::distance(iter, source.end());

		if (dist < (end - start + 1))
		{
			break;
		}
		std::advance(iter, end - start+1);
	}

	return output;
}