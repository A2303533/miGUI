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
#ifndef _MGF_MATERIAL_H_
#define _MGF_MATERIAL_H_

#define MaterialMapCount 8

namespace mgf
{
	
	class GSTexture;

	class MaterialMap
	{
	public:
		MaterialMap() {}
		~MaterialMap() {}

		GSTexture* m_texture = 0;
		StringW m_filename;
	};

	class Material
	{
	public:
		Material() {}
		~Material() {}

		StringW m_name; // newmtl
		v3f m_ambient;   // Ka
		v3f m_diffuse;   // Kd
		v3f m_specular;  // Ks
		float m_specularExponent = 10.f; // Ns
		float m_opacity = 1.f;    // d
		float m_transparency = 0.f; // Tr
		float m_refraction = 1.f;  // Ni

		//StringW m_map_diffuse; // map_Kd  
		//StringW m_map_ambient; // map_Ka 
		//StringW m_map_specular; // map_Ks   
		//StringW m_map_specularHighlight; // map_Ns    
		//StringW m_map_alpha; // map_d     
		//StringW m_map_bump; // map_bump bump 
		//StringW m_map_displacement; // disp 
		//StringW m_map_reflection; // refl  

		MaterialMap m_maps[MaterialMapCount];
	};
}

#endif