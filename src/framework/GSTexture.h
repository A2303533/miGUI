#pragma once
#ifndef __MGF_GSTEXTURE_H_
#define __MGF_GSTEXTURE_H_

namespace mgf
{
	class GSTextureInfo
	{
	public:
		GSTextureInfo()
			:
			m_textureType(Type_Texture2D),
			m_filter(Filter_PPP),
			m_addressMode(AddressMode_Wrap),
			m_comparisonFunc(GSTextureInfo::ComparisonFunc_Always),
			m_anisotropicLevel(1),
			m_flags(0),
			m_width(0),
			m_height(0),
			m_imagePtr(0),
			m_imagePathPtr(0)
		{}
		~GSTextureInfo() {}

		void SetImage(Image* img)
		{
			m_imagePtr = img->GetMGImage();
			m_width = img->GetWidth();
			m_height = img->GetHeight();
		}

		enum Type
		{
			Type_Texture2D,
			Type_RTT
		};

		enum ComparisonFunc
		{
			ComparisonFunc_Never,
			ComparisonFunc_Less,
			ComparisonFunc_Equal,
			ComparisonFunc_LessEqual,
			ComparisonFunc_Greater,
			ComparisonFunc_NotEqual,
			ComparisonFunc_GreaterEqual,
			ComparisonFunc_Always
		};

		enum AddressMode
		{
			AddressMode_Wrap,
			AddressMode_Mirror,
			AddressMode_Clamp,
			AddressMode_Border,
			AddressMode_MirrorOnce
		};

		enum Filter
		{
			// min mag mip / point linear
			Filter_PPP,
			Filter_PPL,
			Filter_PLP,
			Filter_PLL,
			Filter_LPP,
			Filter_LPL,
			Filter_LLP,
			Filter_LLL,
			Filter_ANISOTROPIC,
			// comparison
			Filter_CMP_PPP,
			Filter_CMP_PPL,
			Filter_CMP_PLP,
			Filter_CMP_PLL,
			Filter_CMP_LPP,
			Filter_CMP_LPL,
			Filter_CMP_LLP,
			Filter_CMP_LLL,
			Filter_CMP_ANISOTROPIC,
		};

		Type m_textureType;
		Filter m_filter;
		AddressMode m_addressMode;
		ComparisonFunc m_comparisonFunc;
		int32_t m_anisotropicLevel;

		uint32_t m_flags;
		enum
		{
			flag_useMipmaps = 0x1,

			// don't use
			flag_fromCache = 0x2,
		};

		bool IsUseMipMaps()
		{
			return (m_flags & flag_useMipmaps) == flag_useMipmaps;
		}

		void UseMipMaps(bool set)
		{
			if (set)
			{
				m_flags |= flag_useMipmaps;
			}
			else
			{
				if (m_flags & flag_useMipmaps)
					m_flags ^= flag_useMipmaps;
			}
		}

		uint32_t m_width;
		uint32_t m_height;

		mgImage* m_imagePtr;

		// if use miGetTexture then this ptr will have address to CacheNode<miGPUTexture>.m_path.data()
		// and then you can use Unload/Reload
		const wchar_t* m_imagePathPtr;
	};

	class GSTexture : public mgf::BaseClass
	{
	protected:
		GSTextureInfo m_textureInfo;
	public:
		GSTexture() {}
		virtual ~GSTexture() {}

		GSTextureInfo* GetTextureInfo() { return &m_textureInfo; }
		uint32_t GetWidth() { return m_textureInfo.m_width; }
		uint32_t GetHeight() { return m_textureInfo.m_height; }

		v2f GetUV(uint32_t x, uint32_t y)
		{
			v2f uv;

			v2f tsz((float)m_textureInfo.m_width, (float)m_textureInfo.m_height);

			uv.x = x * 1.f / (float)tsz.x;
			uv.y = y * 1.f / (float)tsz.y;

			return uv;
		}
	};

}
#endif
