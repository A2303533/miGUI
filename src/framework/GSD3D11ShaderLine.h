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

#pragma once
#ifndef MGF_D3D11_SHADER_LINE_H__
#define MGF_D3D11_SHADER_LINE_H__

#include "GSD3D11Shader.h"

namespace mgf
{

	class GSD3D11ShaderLine : public GSD3D11Shader
	{
		ID3D11Device* m_device = 0;
		ID3D11DeviceContext* m_deviceCotext = 0;
	public:
		GSD3D11ShaderLine(ID3D11Device*, ID3D11DeviceContext*);
		virtual ~GSD3D11ShaderLine();

		ID3D11Buffer* m_cb;

		struct cb
		{
			Mat4 VP;
			v4f  P1;
			v4f  P2;
			mgColor  Color;
		}m_cbData;

		bool init();
		virtual void SetConstants(void*) override;
		virtual bool OnCreate(void*) override { return true; }

		void SetData(const v4f& p1, const v4f& p2, const mgColor& c, Mat4*);
	};

}

#endif