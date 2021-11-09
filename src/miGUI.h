#include "mixer.lib.h"
#include "mixer.lib.mesh.h"
#include "mixer.lib.gpuTexture.h"
#include "mixer.lib.gpuMesh.h"
#include "mixer.gui.h"
#include "io/mixer.file.h"

#include "mixer.gui.contextImpl.h"
#include "mixer.gui.element.textImpl.h"

#include "miLib.h"

#include <filesystem>

extern miLib* g_lib;

extern "C"
{
	MI_API miGUIContext* MI_C_DECL miGUICreateContext(miWindow* w)
	{
		miGUIContextImpl* c = miCreate<miGUIContextImpl>();
		c->m_window = w;
		return c;
	}

	MI_API void MI_C_DECL miGUIDestroyContext(miGUIContext* c)
	{
		assert(c);
		miDestroy(c);
	}

	MI_API miGUIFont* MI_C_DECL miGUILoadFont(const wchar_t* fileName)
	{
		std::filesystem::path p = fileName;
		if (!std::filesystem::exists(p))
		{
			miLogWriteWarning("Font file [%s] not exist\n", fileName);
			MI_PRINT_FAILED;
			return nullptr;
		}

		std::filesystem::path parentPath = p.parent_path();

		miFile file;
		file.open(fileName, "rb");

		if (!file.isOpen())
		{
			miLogWriteWarning("Can't open file [%s]\n", fileName);
			MI_PRINT_FAILED;
			return nullptr;
		}

		if (!file.isUTF16LE())
		{
			miLogWriteWarning("Font file [%s] is not UTF16-LE\n", fileName);
			MI_PRINT_FAILED;
			return nullptr;
		}

		miString str;
		file.readWordFromUTF16LE(str);

		int num_of_textures = util::to_int(str.data(), (int)str.size());
		if (num_of_textures < 1)
		{
			miLogWriteWarning("Bad font file [%s]\n", fileName);
			MI_PRINT_FAILED;
			return nullptr;
		}

		std::vector<miString> texture_name_array;
		texture_name_array.reserve(num_of_textures);
		for (int i = 0; i < num_of_textures; ++i)
		{
			file.get_lineUTF16LE(str);

			miString wstr;
			wstr = parentPath.c_str();
			wstr += L"\\";
			wstr += str;

			texture_name_array.emplace_back(wstr);
		}

		// skip comment
		// надо переделать. комментарий по сути обязателен.
		file.get_lineUTF16LE(str);

		struct CharInfo
		{
			CharInfo()
			{
				underhang = 0;
				overhang = 0;
				textureSlot = 0;
			}

			wchar_t symbol;
			v4f rect;
			float underhang;
			float overhang;
			int   textureSlot;
		};

		std::vector<CharInfo> char_info_array;
		while (!file.isEOF())
		{
			CharInfo ci;

			bool is_new_line = false;
			int integer = 0;

			// symbol
			file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
			if (str.size()) ci.symbol = str[0];
			if (is_new_line) continue;

			// leftTopX
			file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
			if (str.size()) ci.rect.x = (float)util::to_int(str.data(), (int)str.size());
			if (is_new_line) continue;


			// leftTopY
			file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
			if (str.size()) ci.rect.y = (float)util::to_int(str.data(), (int)str.size());
			if (is_new_line) continue;

			// rightBottomX
			file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
			if (str.size()) ci.rect.z = (float)util::to_int(str.data(), (int)str.size());
			if (is_new_line) continue;

			// rightBottomY
			file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
			if (str.size()) ci.rect.w = (float)util::to_int(str.data(), (int)str.size());
			if (is_new_line) continue;

			// underhang
			file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
			if (str.size()) ci.underhang = (float)util::to_int(str.data(), (int)str.size());
			if (is_new_line) continue;

			// overhang
			file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
			if (str.size()) ci.overhang = (float)util::to_int(str.data(), (int)str.size());
			if (is_new_line) continue;

			// textureId
			file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
			if (str.size()) ci.textureSlot = util::to_int(str.data(), (int)str.size());

			char_info_array.emplace_back(ci);
		}


		if (char_info_array.size())
		{
			miGUIFont* newFont = miCreate<miGUIFont>();

			for (auto& tf : texture_name_array)
			{
				auto t = miGetTexture(tf.data());
				newFont->m_textures.push_back(t);
			}

			v2f tsz((f32)newFont->m_textures[0]->GetWidth(), (f32)newFont->m_textures[0]->GetHeight());

			f32 mulX = 1.f / (f32)tsz.x;
			f32 mulY = 1.f / (f32)tsz.y;

			for (auto& ci : char_info_array)
			{
				auto glyph = newFont->GetGlyph(ci.symbol);
				glyph->symbol = ci.symbol;

				/*glyph->lb.set(ci.rect.x * uvPerPixel_x, (ci.rect.w) * uvPerPixel_y);
				glyph->lt.set(ci.rect.x * uvPerPixel_x, (ci.rect.y) * uvPerPixel_y);
				glyph->rb.set((ci.rect.z) * uvPerPixel_x, (ci.rect.w) * uvPerPixel_y);
				glyph->rt.set((ci.rect.z) * uvPerPixel_x, (ci.rect.y) * uvPerPixel_y);*/

				glyph->lt.x = ci.rect.x * mulX;
				glyph->lt.y = ci.rect.y * mulY;
				glyph->rb.x = (ci.rect.z + 0.f) * mulX;
				glyph->rb.y = (ci.rect.w + 0.f) * mulY;

				glyph->width = ci.rect.z - ci.rect.x;
				glyph->height = ci.rect.w - ci.rect.y;

				if (glyph->height > newFont->m_maxHeight)
					newFont->m_maxHeight = glyph->height;

				glyph->overhang = ci.overhang;
				glyph->underhang = ci.underhang;
				glyph->textureID = ci.textureSlot;

				//	printf("[%c] [%f]\n", ci.symbol, glyph->height);
			}

			g_lib->m_fonts.push_back(newFont);

			return newFont;
		}
		return nullptr;
	}
}


void miLib::GUIDestroyFont(miGUIFont* f)
{
	assert(f);
	miDestroy(f);
}

