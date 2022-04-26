#pragma once
#ifndef __MGF_GSTEXTURE_H_
#define __MGF_GSTEXTURE_H_

namespace mgf
{
	enum class GSTextureType : uint32_t
	{
		Texture2D,
		RTT
	};

	enum class GSTextureComparisonFunc : uint32_t
	{
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always
	};

	enum class GSTextureAddressMode : uint32_t
	{
		Wrap,
		Mirror,
		Clamp,
		Border,
		MirrorOnce
	};

	enum class GSTextureFilter : uint32_t
	{
		// min mag mip / point linear
		PPP,
		PPL,
		PLP,
		PLL,
		LPP,
		LPL,
		LLP,
		LLL,
		ANISOTROPIC,
		// comparison
		CMP_PPP,
		CMP_PPL,
		CMP_PLP,
		CMP_PLL,
		CMP_LPP,
		CMP_LPL,
		CMP_LLP,
		CMP_LLL,
		CMP_ANISOTROPIC,
	};

	class GSTextureInfo
	{
	public:
		GSTextureInfo() {}
		~GSTextureInfo() {}

		GSTextureType m_textureType = GSTextureType::Texture2D;
		GSTextureFilter m_filter = GSTextureFilter::PPP;
		GSTextureAddressMode m_addressMode = GSTextureAddressMode::Wrap;
		GSTextureComparisonFunc m_comparisonFunc = GSTextureComparisonFunc::Always;
		int32_t m_anisotropicLevel = 1;

		uint32_t m_flags = 0;
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

		uint32_t m_width = 0;
		uint32_t m_height = 0;

		mgImage* m_imagePtr = nullptr;

		// if use miGetTexture then this ptr will have address to CacheNode<miGPUTexture>.m_path.data()
		// and then you can use Unload/Reload
		const wchar_t* m_imagePathPtr = nullptr;
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
