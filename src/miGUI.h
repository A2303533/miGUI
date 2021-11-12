#ifndef _MI_GUI_H_
#define _MI_GUI_H_

namespace migui
{
	template<typename T>
	struct vec2
	{
		T x = static_cast<T>(0);
		T y = static_cast<T>(0);
	};
	using vec2f = vec2<float>;
	using vec2i = vec2<int>;
	using vec2u = vec2<unsigned int>;

	template<typename T>
	struct vec4
	{
		T x = static_cast<T>(0);
		T y = static_cast<T>(0);
		T z = static_cast<T>(0);
		T w = static_cast<T>(0);

		void SetAsColorRGB(unsigned int val)
		{
		//	x = ;
		}
	};
	using vec4f = vec4<float>;
	using vec4i = vec4<int>;
	using vec4u = vec4<unsigned int>;

	// RGBA
	struct Image
	{
		unsigned int m_width = 0;
		unsigned int m_height = 0;
		unsigned int m_dataSize = 0;
		unsigned char* m_data = 0;
	};

	using Texture = void*;

	// Before creating GUI context you must create this objects.
	// VideoDriverAPI - callbacks for drawing.
	// InputContext - information about pressed buttons, cursor position and other
	struct VideoDriverAPI
	{
		// Create GPU texture using RGBA data.
		// Library can generate fonts, fonts is a textures.
		Texture(*m_createTexture)(Image*) = 0;

		// Destroy texture. For fonts.
		void(*m_destroyTexture)(Texture) = 0;

		// 
		void(*m_drawRectangle)(const vec2i& position, const vec2i& size, const vec4f& color1, const vec4f& color2, Texture optionalTexture, vec4f* optionalUVRegion) = 0;
	};

	// You must update states by yourself
	struct InputContext
	{

	};

	class GUIContext
	{
	public:
		GUIContext();
		~GUIContext();

		VideoDriverAPI* m_gpu = 0;
		InputContext* m_input = 0;
	};
}

#endif