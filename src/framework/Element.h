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
#ifndef _MGF_ELEMENT_H_
#define _MGF_ELEMENT_H_

namespace mgf
{
	class Element;

	/// <summary>
	/// delete this
	/// </summary>
	typedef void(*ElementCallback)(Element*);

	class Element
	{
	protected:
		Window* m_window = 0;
	public:
		Element(Window*);
		virtual ~Element();

		enum Alignment
		{
			LeftTop,
			Top,
			RightTop,
			Right,
			RightBottom,
			Bottom,
			LeftBottom,
			Left,
			Center
		};

		virtual mgElement_s* GetElement();

		virtual void SetUserData(void*);
		virtual void* GetUserData();

		virtual Alignment GetAlignment();
		virtual void SetAlignment(Alignment);
		
		virtual void SetUserStyle(mgStyle_s*);

		virtual void SetVisible(bool);
		virtual bool IsVisible();
		
		virtual void SetID(int);
		virtual int GetID();
		
		virtual void SetDrawBG(bool);
		virtual bool IsDrawBG();

		virtual void SetEnabled(bool);
		virtual bool IsEnabled();

		virtual void SetRect(mgRect*);
		virtual void SetRect(int left, int top, int right, int bottom);
		virtual void SetRectLeft(int v);
		virtual void SetRectTop(int v);
		virtual void SetRectRight(int v);
		virtual void SetRectBottom(int v);
		virtual void SetPositionAndSize(int posX, int posY, int sizeX, int sizeY);

		virtual void OnMouseEnter();
		virtual void OnMouseLeave();
		virtual void OnClickLMB();
		virtual void OnClickRMB();
		virtual void OnClickMMB();
		virtual void OnClickX1MB();
		virtual void OnClickX2MB();
		virtual void OnReleaseLMB();
		virtual void OnReleaseRMB();
		virtual void OnReleaseMMB();
		virtual void OnReleaseX1MB();
		virtual void OnReleaseX2MB();
		/*ElementCallback onMouseEnter = 0;
		ElementCallback onMouseLeave = 0;
		ElementCallback onClickLMB = 0;
		ElementCallback onClickRMB = 0;
		ElementCallback onClickMMB = 0;
		ElementCallback onClickX1MB = 0;
		ElementCallback onClickX2MB = 0;
		ElementCallback onReleaseLMB = 0;
		ElementCallback onReleaseRMB = 0;
		ElementCallback onReleaseMMB = 0;
		ElementCallback onReleaseX1MB = 0;
		ElementCallback onReleaseX2MB = 0;*/


		/// <summary>
		/// Make this element last for drawing, first for input.
		/// It will be like on top of all other elements.
		/// It's just changing the order.
		/// </summary>
		virtual void ToTop();

		virtual void SetTextProcessor(TextProcessor*);
		virtual TextProcessor* GetTextProcessor();

	protected:
		mgElement_s* m_element = 0;
		void* m_userData = 0;
		Alignment m_alignment = LeftTop;
		TextProcessor* m_textProcessor;

		void PostInit();
	};
}

#endif