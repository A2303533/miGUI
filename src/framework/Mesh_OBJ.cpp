/*
  Copyright (C) 2022 Basov Artyom
  The authors can be contacted at <artembasov@outlook.com>
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
	 notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
	 notice, this list of conditions and the following disclaimer in
	 the documentation and/or other materials provided with the
	 distribution.
  3. The names of the authors may not be used to endorse or promote
	 products derived from this software without specific prior
	 written permission.
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// windows.h include winsock.h that can conflict with filesystem
//#define WIN32_LEAN_AND_MEAN // add in project parameters

#include "framework/mgf.h"
#include <filesystem>
#include "String.h"
#include "Array.h"
#include "Mesh.h"
#include "PolygonCreator.h"
#include "MeshBuilder.h"
#include "Material.h"
#include "Log.h"

#include <unordered_map>

using namespace mgf;

enum class OBJFaceType
{
	p,
	pu,
	pun,
	pn
};

struct OBJSimpleArr
{
	OBJSimpleArr() {
		sz = 0;
	}

	int data[0x100];
	unsigned int sz;

	void push_back(int v) { data[sz++] = v; }
	unsigned int size() { return sz; }
	void reset() { sz = 0; }
};

struct OBJFace
{
	OBJFace() {
		ft = OBJFaceType::pun;
	}

	OBJSimpleArr p, u, n;
	OBJFaceType ft;

	void reset()
	{
		ft = OBJFaceType::pun;
		p.reset();
		u.reset();
		n.reset();
	}
};

bool OBJStringEqual(const StringW& str, const char* c_Str)
{
	auto c_str_len = std::strlen(c_Str);
	if (c_str_len != str.size())
		return false;

	for (size_t i = 0, sz = str.size(); i < sz; ++i)
	{
		if (str[i] != (StringW::value_type)c_Str[i])
			return false;
	}

	return true;
}

unsigned char* OBJNextLine(unsigned char* ptr);
unsigned char* OBJSkipSpaces(unsigned char* ptr);
unsigned char* OBJReadVec2(unsigned char* ptr, v2f& vec2);
unsigned char* OBJReadFloat(unsigned char* ptr, float& value);
unsigned char* OBJReadVec3(unsigned char* ptr, v3f& vec3);
unsigned char* OBJReadFace(unsigned char* ptr, OBJFace& f, char* s);
unsigned char* OBJReadWord(unsigned char* ptr, StringW& str);
unsigned char* OBJReadLastWord(unsigned char* ptr, StringW& str);

void OBJMTL(
	Array<Material*>& materials,
	const char* obj_fileName,
	const char* mtl_fileName
)
{
	auto relPath = std::filesystem::relative(obj_fileName);

	{
		StringW mtlPath = relPath.c_str();
		for (size_t i = 0, sz = relPath.native().size(); i < sz; ++i)
		{
			auto c = mtlPath.pop_back_return();
			if (c == L'/' || c == L'\\')
			{
				relPath += c;
				break;
			}
		}
	}

	StringW mtlPath = relPath.c_str();
	mtlPath += mtl_fileName;

	if (std::filesystem::exists(mtlPath.data()))
	{
		auto mbStr = mtlPath.to_utf8();

		FILE* file = fopen(mbStr.data(), "rb");
		auto file_size = (size_t)std::filesystem::file_size(mbStr.data());

		std::vector<unsigned char> file_byte_array((unsigned int)file_size + 2);
		unsigned char* ptr = file_byte_array.data();

		fread(ptr, 1, file_size, file);
		fclose(file);

		ptr[(unsigned int)file_size] = ' ';
		ptr[(unsigned int)file_size + 1] = 0;

		Material* curMaterial = 0;

		while (*ptr)
		{
			switch (*ptr)
			{
			case '#':
			case 'd':
			case 'T':
			case 'i':
			case 'K':
				ptr = OBJNextLine(ptr);
				break;
			case 'n':
			{
				StringW mtlWord;
				ptr = OBJReadWord(ptr, mtlWord);

				if (OBJStringEqual(mtlWord, "newmtl"))
				{
					curMaterial = new Material;
					materials.push_back(curMaterial);

					ptr = OBJReadWord(ptr, curMaterial->m_name);
				}
			}break;
			case 'N':
			{
				StringW word;
				ptr = OBJReadWord(ptr, word);
				if (OBJStringEqual(word, "Ns"))
				{
					ptr = OBJSkipSpaces(ptr);
					ptr = OBJReadFloat(ptr, curMaterial->m_specularExponent);
				}
				else if (OBJStringEqual(word, "Ni"))
				{
					ptr = OBJSkipSpaces(ptr);
					ptr = OBJReadFloat(ptr, curMaterial->m_refraction);
				}
				else
					ptr = OBJNextLine(ptr);

			}break;
			case 'm':
			{
				StringW word;
				ptr = OBJReadWord(ptr, word);
				if (OBJStringEqual(word, "map_Kd"))
				{
					ptr = OBJReadLastWord(ptr, word);

					StringW mapPath = word;

					if (!std::filesystem::exists(mapPath.data()))
					{
						mapPath = relPath.c_str();
						mapPath += word.data();
					}

					curMaterial->m_maps[0].m_filename = mapPath;
				}
				else
					ptr = OBJNextLine(ptr);
			}break;
			default:
				++ptr;
				break;
			}
		}
	}
}

Material* OBJGetMaterial(
	Array<Material*>& materials,
	const StringW& name
)
{
	for (uint32_t i = 0; i < materials.m_size; ++i)
	{
		if (materials.m_data[i]->m_name == name)
			return materials.m_data[i];
	}
	return 0;
}

void mgf::Mesh_OBJ(const char* fn, MeshLoader* loader)
{
	auto mbStr = fn;

	FILE* file = fopen(mbStr, "rb");
	if (!file)
	{
		mgf::LogWriteError("%s: can't open file [%s]", MGF_FUNCTION, fn);
		return;
	}

	auto file_size = (size_t)std::filesystem::file_size(mbStr);
	if (!file_size)
	{
		mgf::LogWriteError("%s: file size [%s] is 0", MGF_FUNCTION, fn);
		return;
	}

	std::vector<unsigned char> file_byte_array((unsigned int)file_size + 2);

	unsigned char* ptr = file_byte_array.data();
	fread(ptr, 1, file_size, file);
	fclose(file);

	ptr[(unsigned int)file_size] = ' ';
	ptr[(unsigned int)file_size + 1] = 0;

	bool groupBegin = false;
	bool isModel = false;
	bool grpFound = false;

	v2f tcoords;
	v3f pos;
	v3f norm;

	std::vector<v3f> position;
	std::vector<v2f> uv;
	std::vector<v3f> normal;

	position.reserve(0xffff);
	uv.reserve(0xffff);
	normal.reserve(0xffff);

	position.reserve(0xffff);
	uv.reserve(0xffff);
	normal.reserve(0xffff);

	//std::string name_word;
	StringW tmp_word;
	StringW curr_word;
	StringW prev_word;

	OBJFace f;
	char s[0xff];

	int last_counter[3] = { 0,0,0 };

	std::unordered_map<std::string, unsigned int> map;
	std::string hash;

	mgf::PolygonCreator polygonCreator;
	mgf::PolygonCreator triangulationPolygonCreator;
	mgf::MeshBuilder meshBuilder;

	Array<Material*> obj_materials;
	Material* currMaterial = 0;

	while (*ptr)
	{
		switch (*ptr)
		{
		case 'm'://mtllib
		{
			StringW word;
			ptr = OBJReadWord(ptr, word);

			if (OBJStringEqual(word, "mtllib"))
			{
				ptr = OBJReadWord(ptr, word);
				//wprintf(L"MTL: %s\n", mtlWord.data());
				StringA stra;
				stra = word.data();
				OBJMTL(obj_materials, mbStr, stra.data());
			}
		}break;
		case 'u'://usemtl
		{
			StringW word;
			ptr = OBJReadWord(ptr, word);

			if (OBJStringEqual(word, "usemtl"))
			{
				ptr = OBJReadWord(ptr, word);
				currMaterial = OBJGetMaterial(obj_materials, word);
			}
		}break;
		case 's':
		case 'l':
		case 'c'://curv
		case 'p'://parm
		case 'd'://deg
		case '#':
		case 'e'://end
			ptr = OBJNextLine(ptr);
			break;
		case 'v':
		{
			++ptr;
			if (groupBegin)
				groupBegin = false;
			switch (*ptr)
			{
			case 't':
				ptr = OBJReadVec2(++ptr, tcoords);
				uv.push_back(tcoords);
				++last_counter[1];
				break;
			case 'n':
				ptr = OBJReadVec3(++ptr, norm);
				normal.push_back(norm);
				++last_counter[2];
				break;
			default:
				ptr = OBJReadVec3(ptr, pos);
				position.push_back(pos);
				++last_counter[0];
				break;
			}
		}break;
		case 'f':
		{
			isModel = true;
			f.reset();
			ptr = OBJReadFace(++ptr, f, s);

			polygonCreator.Clear();
			if (!meshBuilder.m_isBegin)
				meshBuilder.Begin();

			bool weld = false;
			bool genNormals = true;

			for (size_t sz2 = f.p.size(), i2 = 0; i2 < sz2; ++i2)
			{
				auto index = i2;
				auto pos_index = f.p.data[index];


				if (pos_index < 0)
				{
					// если индекс отрицательный то он указывает на позицию относительно последнего элемента
					// -1 = последний элемент
					pos_index = last_counter[0] + pos_index + 1;
				}

				{
					hash.clear();
					hash += pos_index;

					// это я не помню зачем сделал
					// когда дойду до control вершин, станет ясно зачем это здесь
					auto it = map.find(hash);
					if (it == map.end())
						map[hash] = pos_index;
					else
						weld = true;
				}

				auto v = position[pos_index];

				v3f pcPos, pcNorm;
				v2f pcUV;

				pcPos = v;

				if (f.ft == OBJFaceType::pu || f.ft == OBJFaceType::pun)
				{
					auto uv_index = f.u.data[index];

					if (uv_index < 0)
						uv_index = last_counter[1] + uv_index + 1;

					auto u = uv[uv_index];
					pcUV.x = u.x;
					pcUV.y = 1.f - u.y;
				}

				if (f.ft == OBJFaceType::pn || f.ft == OBJFaceType::pun)
				{
					auto nor_index = f.n.data[index];

					if (nor_index < 0)
						nor_index = last_counter[2] + nor_index + 1;

					auto n = normal[nor_index];
					pcNorm = n;
					genNormals = false;
				}
				polygonCreator.Add(pcPos, weld, false, pcNorm, pcUV);
			}

			uint32_t triCount = polygonCreator.Size() - 2;
			auto _positions = polygonCreator.GetPositions();
			auto _normals = polygonCreator.GetNormals();
			auto _tcoords = polygonCreator.GetTCoords();
			for (uint32_t i = 0; i < triCount; ++i)
			{
				triangulationPolygonCreator.Clear();
				triangulationPolygonCreator.Add(_positions[0].m_first, weld, false, _normals[0], _tcoords[0]);
				triangulationPolygonCreator.Add(_positions[i + 1].m_first, weld, false, _normals[i + 1], _tcoords[i + 1]);
				triangulationPolygonCreator.Add(_positions[i + 2].m_first, weld, false, _normals[i + 2], _tcoords[i + 2]);

				meshBuilder.AddPolygon(&triangulationPolygonCreator, genNormals);
			}

			/*if (g_ImportOBJ_triangulate && importerHelper.m_polygonCreator.Size() > 3)
			{
				u32 triCount = importerHelper.m_polygonCreator.Size() - 2;
				auto _positions = importerHelper.m_polygonCreator.GetPositions();
				auto _normals = importerHelper.m_polygonCreator.GetNormals();
				auto _tcoords = importerHelper.m_polygonCreator.GetTCoords();
				for (u32 i = 0; i < triCount; ++i)
				{
					triangulationPolygonCreator.Clear();
					triangulationPolygonCreator.Add(_positions[0].m_first, weld, false, _normals[0], _tcoords[0]);
					triangulationPolygonCreator.Add(_positions[i + 1].m_first, weld, false, _normals[i + 1], _tcoords[i + 1]);
					triangulationPolygonCreator.Add(_positions[i + 2].m_first, weld, false, _normals[i + 2], _tcoords[i + 2]);

					importerHelper.m_meshBuilder->AddPolygon(&triangulationPolygonCreator, genNormals);
				}
			}
			else
			{
				importerHelper.m_meshBuilder->AddPolygon(&importerHelper.m_polygonCreator, genNormals);
			}*/
		}break;
		case 'o':
		case 'g':
		{
			if (!groupBegin)
				groupBegin = true;
			else
			{
				ptr = OBJNextLine(ptr);
				break;
			}

			ptr = OBJReadWord(++ptr, tmp_word);
			if (tmp_word.size())
			{
				prev_word = curr_word;
				curr_word = tmp_word;
			}

			if (grpFound)
			{
				if (meshBuilder.m_isBegin)
				{
					meshBuilder.End();
					/*Material* m = 0;
					if (currMaterial)
					{
						m = g_sdk->CreateMaterial(currMaterial->m_name.data());
						if (currMaterial->m_map_diffuse.size())
							m->m_maps[m->mapSlot_Diffuse].m_texturePath = currMaterial->m_map_diffuse;
					}*/

					loader->OnLoad(&meshBuilder, currMaterial);
					//g_sdk->CreateSceneObjectFromHelper(&importerHelper, prev_word.data(), m);

					meshBuilder.Clear();
					meshBuilder.Begin();
				}
			}
			grpFound = true;
		}break;
		default:
			++ptr;
			break;
		}
	}

	if (meshBuilder.m_isBegin)
	{
		meshBuilder.End();

		/*Material* m = 0;
		if (currMaterial)
		{
			m = g_sdk->CreateMaterial(currMaterial->m_name.data());
			if (currMaterial->m_map_diffuse.size())
				m->m_maps[m->mapSlot_Diffuse].m_texturePath = currMaterial->m_map_diffuse;
		}*/

		loader->OnLoad(&meshBuilder, currMaterial);
		//g_sdk->CreateSceneObjectFromHelper(&importerHelper, curr_word.data(), m);
	}


	for (uint32_t i = 0; i < obj_materials.m_size; ++i)
	{
		delete obj_materials.m_data[i];
	}
}

unsigned char* OBJNextLine(unsigned char* ptr)
{
	while (*ptr)
	{
		if (*ptr == '\n')
		{
			ptr++;
			return ptr;
		}
		ptr++;
	}
	return ptr;
}

unsigned char* OBJReadVec2(unsigned char* ptr, v2f& vec2)
{
	ptr = OBJSkipSpaces(ptr);
	float x, y;
	if (*ptr == '\n')
	{
		ptr++;
	}
	else
	{
		ptr = OBJReadFloat(ptr, x);
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadFloat(ptr, y);
		ptr = OBJNextLine(ptr);
		vec2.x = x;
		vec2.y = y;
	}
	return ptr;
}

unsigned char* OBJSkipSpaces(unsigned char* ptr)
{
	while (*ptr)
	{
		if (*ptr != '\t' && *ptr != ' ')
			break;
		ptr++;
	}
	return ptr;
}

unsigned char* OBJReadFloat(unsigned char* ptr, float& value)
{
	char str[32u];
	memset(str, 0, 32);
	char* p = &str[0u];
	while (*ptr) {
		if (!isdigit(*ptr) && (*ptr != '-') && (*ptr != '+')
			&& (*ptr != 'e') && (*ptr != 'E') && (*ptr != '.')) break;
		*p = *ptr;
		++p;
		++ptr;
	}
	value = (float)atof(str);
	return ptr;
}

unsigned char* OBJReadVec3(unsigned char* ptr, v3f& vec3) {
	ptr = OBJSkipSpaces(ptr);
	float x, y, z;
	if (*ptr == '\n') {
		ptr++;
	}
	else {
		ptr = OBJReadFloat(ptr, x);
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadFloat(ptr, y);
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadFloat(ptr, z);
		ptr = OBJNextLine(ptr);
		vec3.x = x;
		vec3.y = y;
		vec3.z = z;
	}
	return ptr;
}

unsigned char* OBJSkipSpace(unsigned char* ptr) {
	while (*ptr) {
		if (*ptr != ' ' && *ptr != '\t') break;
		ptr++;
	}
	return ptr;
}

unsigned char* OBJGetInt(unsigned char* p, int& i)
{
	char str[8u];
	memset(str, 0, 8);
	char* pi = &str[0u];

	while (*p)
	{
		/*if( *p == '-' )
		{
		++p;
		continue;
		}*/

		if (!isdigit(*p) && *p != '-') break;


		*pi = *p;
		++pi;
		++p;
	}
	i = atoi(str);
	return p;
}

unsigned char* OBJReadFace(unsigned char* ptr, OBJFace& f, char* s) {
	ptr = OBJSkipSpaces(ptr);
	if (*ptr == '\n')
	{
		ptr++;
	}
	else
	{
		while (true)
		{
			int p = 1;
			int u = 1;
			int n = 1;

			ptr = OBJGetInt(ptr, p);

			if (*ptr == '/')
			{
				ptr++;
				if (*ptr == '/')
				{
					ptr++;
					f.ft = OBJFaceType::pn;
					ptr = OBJGetInt(ptr, n);
				}
				else
				{
					ptr = OBJGetInt(ptr, u);
					if (*ptr == '/')
					{
						ptr++;
						f.ft = OBJFaceType::pun;
						ptr = OBJGetInt(ptr, n);
					}
					else
					{
						f.ft = OBJFaceType::pu;
					}
				}
			}
			else
			{
				f.ft = OBJFaceType::p;
			}
			f.n.push_back(n - 1);
			f.u.push_back(u - 1);
			f.p.push_back(p - 1);
			ptr = OBJSkipSpace(ptr);

			if (*ptr == '\r')
				break;
			else if (*ptr == '\n')
				break;
		}
	}
	return ptr;
}

unsigned char* OBJReadWord(unsigned char* ptr, StringW& str)
{
	ptr = OBJSkipSpaces(ptr);
	str.clear();
	while (*ptr)
	{
		if (isspace(*ptr))
			break;
		str += (wchar_t)*ptr;
		ptr++;
	}
	return ptr;
}

unsigned char* OBJReadLastWord(unsigned char* ptr, StringW& str) {
	while (true)
	{
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadWord(ptr, str);

		ptr = OBJSkipSpaces(ptr);

		if (*ptr == '\r' || *ptr == '\n')
		{
			break;
		}

		if (*ptr == 0)
			break;
	}
	return ptr;
}