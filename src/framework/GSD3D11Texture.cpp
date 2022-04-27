#include "mgf.h"
#ifdef MGF_GS_D3D11

#include "GSD3D11.h"

#include "GSD3D11Texture.h"

#include "framework/Image.h"

#include "DirectXTK/Inc/DDSTextureLoader.h"

using namespace mgf;

HRESULT	D3D11Texture_createSamplerState(
	ID3D11Device* device,
	D3D11_FILTER filter,
	D3D11_TEXTURE_ADDRESS_MODE addressMode,
	uint32_t anisotropic_level,
	ID3D11SamplerState** samplerState,
	D3D11_COMPARISON_FUNC cmpFunc)
{
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = filter;
	samplerDesc.MipLODBias = 0.0f;

	samplerDesc.AddressU = addressMode;
	samplerDesc.AddressV = addressMode;
	samplerDesc.AddressW = addressMode;

	samplerDesc.ComparisonFunc = cmpFunc; //D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	samplerDesc.MaxAnisotropy = anisotropic_level;

	return device->CreateSamplerState(&samplerDesc, samplerState);
}

GSD3D11Texture::GSD3D11Texture(GSTextureInfo* ti, ID3D11Device* d, ID3D11DeviceContext* dc)
{
	m_textureInfo = *ti;

	D3D11_FILTER filter;
	GSTextureFilter tf = m_textureInfo.m_filter;
	switch (tf)
	{
	case GSTextureFilter::PPP:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
	case GSTextureFilter::PPL:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		break;
	case GSTextureFilter::PLP:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		break;
	case GSTextureFilter::PLL:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		break;
	case GSTextureFilter::LPP:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		break;
	case GSTextureFilter::LPL:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		break;
	case GSTextureFilter::LLP:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case GSTextureFilter::LLL:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	case GSTextureFilter::ANISOTROPIC:
		filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
		break;
	case GSTextureFilter::CMP_PPP:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		break;
	case GSTextureFilter::CMP_PPL:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		break;
	case GSTextureFilter::CMP_PLP:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
		break;
	case GSTextureFilter::CMP_PLL:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
		break;
	case GSTextureFilter::CMP_LPP:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		break;
	case GSTextureFilter::CMP_LPL:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		break;
	case GSTextureFilter::CMP_LLP:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case GSTextureFilter::CMP_LLL:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		break;
	case GSTextureFilter::CMP_ANISOTROPIC:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_ANISOTROPIC;
		break;
	default:
		break;
	}

	if (m_textureInfo.m_textureType == GSTextureType::Texture2D)
	{
		mgImage* img = (mgImage*)m_textureInfo.m_imagePtr;

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = m_textureInfo.m_width;
		desc.Height = m_textureInfo.m_height;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		bool isGenMips = m_textureInfo.IsUseMipMaps();

		switch (img->type)
		{
		case mgImageType_bc1:
		case mgImageType_bc2:
		case mgImageType_bc3:
		{
			auto hr = DirectX::CreateDDSTextureFromMemory(
				d,
				dc,
				(const uint8_t*)img->data,
				(size_t)img->fileSize,
				(ID3D11Resource**)&m_texture,
				&m_textureResView);

			if (FAILED(hr))
				return;
		}break;
		case mgImageType_r8g8b8a8:
		{
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			desc.MiscFlags = 0;
			desc.MipLevels = 1;

			desc.ArraySize = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;

			if (isGenMips)
			{
				desc.MipLevels = 0;
				desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
				auto hr = d->CreateTexture2D(&desc, 0, &m_texture);
				if (FAILED(hr))
				{
					printf("Can't create 2D texture\n");
					return;
				}
				dc->UpdateSubresource(m_texture, 0, NULL, img->data,
					img->pitch, 0);
			}
			else
			{
				D3D11_SUBRESOURCE_DATA initData;
				ZeroMemory(&initData, sizeof(initData));
				initData.pSysMem = img->data;
				initData.SysMemPitch = img->pitch;
				initData.SysMemSlicePitch = img->dataSize;
				auto hr = d->CreateTexture2D(&desc, &initData, &m_texture);
				if (FAILED(hr))
				{
					printf("Can't create 2D texture\n");
					return;
				}
			}


			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			ZeroMemory(&SRVDesc, sizeof(SRVDesc));
			SRVDesc.Format = desc.Format;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MostDetailedMip = 0;
			SRVDesc.Texture2D.MipLevels = 1;
			if (isGenMips)
				SRVDesc.Texture2D.MipLevels = -1;

			auto hr = d->CreateShaderResourceView(m_texture,
				&SRVDesc, &m_textureResView);
			if (FAILED(hr))
			{
				printf("Can't create shader resource view\n");
				return;
			}
		}break;
		default:
			printf("Unsupported texture format\n");
			return;
		}
		if (isGenMips)
			dc->GenerateMips(m_textureResView);
	}
	else
	{
		// RTT
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = m_textureInfo.m_width;
		desc.Height = m_textureInfo.m_height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = 0;
		desc.ArraySize = 1;
		desc.MipLevels = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;

		auto hr = d->CreateTexture2D(&desc, NULL, &m_texture);
		if (FAILED(hr))
		{
			printf("Can't create render target texture\n");
			return;
		}
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = desc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		hr = d->CreateRenderTargetView(m_texture, &renderTargetViewDesc, &m_RTV);
		if (FAILED(hr))
		{
			printf("Can't create render target view\n");
			return;
		}



		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = desc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.MipLevels = 1;

		hr = d->CreateShaderResourceView(m_texture,
			&SRVDesc, &m_textureResView);
		if (FAILED(hr))
		{
			printf("Can't create shader resource view\n");
			return;
		}

		return;
	}

	D3D11_TEXTURE_ADDRESS_MODE tam;
	switch (m_textureInfo.m_addressMode)
	{
	case GSTextureAddressMode::Wrap:
	default:
		tam = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case GSTextureAddressMode::Mirror:
		tam = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;
	case GSTextureAddressMode::Clamp:
		tam = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case GSTextureAddressMode::Border:
		tam = D3D11_TEXTURE_ADDRESS_BORDER;
		break;
	case GSTextureAddressMode::MirrorOnce:
		tam = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
		break;
	}

	D3D11_COMPARISON_FUNC cmpFunc;
	switch (m_textureInfo.m_comparisonFunc)
	{
	case GSTextureComparisonFunc::Never:
		cmpFunc = D3D11_COMPARISON_NEVER;
		break;
	case GSTextureComparisonFunc::Less:
		cmpFunc = D3D11_COMPARISON_LESS;
		break;
	case GSTextureComparisonFunc::Equal:
		cmpFunc = D3D11_COMPARISON_EQUAL;
		break;
	case GSTextureComparisonFunc::LessEqual:
		cmpFunc = D3D11_COMPARISON_LESS_EQUAL;
		break;
	case GSTextureComparisonFunc::Greater:
		cmpFunc = D3D11_COMPARISON_GREATER;
		break;
	case GSTextureComparisonFunc::NotEqual:
		cmpFunc = D3D11_COMPARISON_NOT_EQUAL;
		break;
	case GSTextureComparisonFunc::GreaterEqual:
		cmpFunc = D3D11_COMPARISON_GREATER_EQUAL;
		break;
	case GSTextureComparisonFunc::Always:
	default:
		cmpFunc = D3D11_COMPARISON_ALWAYS;
		break;
	}


	auto hr = D3D11Texture_createSamplerState(d, filter, tam, m_textureInfo.m_anisotropicLevel,
		&m_samplerState, cmpFunc);
	if (FAILED(hr))
	{
		printf("Can't create sampler state\n");
		return;
	}
}

GSD3D11Texture::~GSD3D11Texture()
{
	if (m_RTV)
	{
		m_RTV->Release();
		m_RTV = 0;
	}
	if (m_samplerState)
	{
		m_samplerState->Release();
		m_samplerState = 0;
	}
	if (m_textureResView)
	{
		m_textureResView->Release();
		m_textureResView = 0;
	}
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}
}

#endif

