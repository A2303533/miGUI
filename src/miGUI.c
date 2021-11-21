/*
  Copyright (C) 2021 Basov Artyom
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

#include "miGUI.h"

#include <stdlib.h>
#include <assert.h>

void miGUI_update(mgContext*);
void miGUI_startFrame(mgContext*);

MG_API 
mgContext* MG_C_DECL
mgCreateContext_f(mgVideoDriverAPI* gpu, mgInputContext* input)
{
	assert(gpu);
	assert(input);

	mgContext* c = malloc(sizeof(mgContext));
	c->m_gpu = gpu;
	c->m_input = input;

	c->createFont = mgCreateFont;
	c->update = miGUI_update;
	c->startFrame = miGUI_startFrame;

	return c;
}

MG_API 
void MG_C_DECL
mgDestroyContext_f(mgContext* c)
{
	assert(c);

	/*destroy everything here*/
	/*...*/

	free(c);
}

void 
miGUI_update(mgContext* c)
{
	assert(c);

	unsigned int ctrl_shift_alt = 0;
	if (mgIsKeyHold(c->m_input, MG_KEY_LALT) || mgIsKeyHold(c->m_input, MG_KEY_RALT))
		ctrl_shift_alt |= 1;

	if (mgIsKeyHold(c->m_input, MG_KEY_LSHIFT) || mgIsKeyHold(c->m_input, MG_KEY_RSHIFT))
		ctrl_shift_alt |= 2;

	if (mgIsKeyHold(c->m_input, MG_KEY_LCTRL) || mgIsKeyHold(c->m_input, MG_KEY_RCTRL))
		ctrl_shift_alt |= 4;

	switch (ctrl_shift_alt)
	{
	default:
	case 0:  c->m_input->keyboardModifier = MG_KBMOD_clear;         break;
	case 1:  c->m_input->keyboardModifier = MG_KBMOD_ALT;           break;
	case 2:  c->m_input->keyboardModifier = MG_KBMOD_SHIFT;         break;
	case 3:  c->m_input->keyboardModifier = MG_KBMOD_SHIFTALT;      break;
	case 4:  c->m_input->keyboardModifier = MG_KBMOD_CTRL;          break;
	case 5:  c->m_input->keyboardModifier = MG_KBMOD_CTRLALT;       break;
	case 6:  c->m_input->keyboardModifier = MG_KBMOD_CTRLSHIFT;     break;
	case 7:  c->m_input->keyboardModifier = MG_KBMOD_CTRLSHIFTALT;  break;
	}

}

void 
miGUI_startFrame(mgContext* c)
{
	assert(c);
	
	c->m_input->mouseMoveDeltaOld = c->m_input->mouseMoveDelta;
	c->m_input->mouseWheelDeltaOld = c->m_input->mouseWheelDelta;
	c->m_input->mousePositionOld = c->m_input->mousePosition;

	if ((c->m_input->mouseButtonFlags1 & MG_MBFL_LMBDOWN) == MG_MBFL_LMBDOWN)
		c->m_input->mouseButtonFlags1 ^= MG_MBFL_LMBDOWN;
	if ((c->m_input->mouseButtonFlags1 & MG_MBFL_RMBDOWN) == MG_MBFL_RMBDOWN)
		c->m_input->mouseButtonFlags1 ^= MG_MBFL_RMBDOWN;
	if ((c->m_input->mouseButtonFlags1 & MG_MBFL_MMBDOWN) == MG_MBFL_MMBDOWN)
		c->m_input->mouseButtonFlags1 ^= MG_MBFL_MMBDOWN;
	if ((c->m_input->mouseButtonFlags1 & MG_MBFL_X1MBDOWN) == MG_MBFL_X1MBDOWN)
		c->m_input->mouseButtonFlags1 ^= MG_MBFL_X1MBDOWN;
	if ((c->m_input->mouseButtonFlags1 & MG_MBFL_X2MBDOWN) == MG_MBFL_X2MBDOWN)
		c->m_input->mouseButtonFlags1 ^= MG_MBFL_X2MBDOWN;

	if ((c->m_input->mouseButtonFlags1 & MG_MBFL_LMBUP) == MG_MBFL_LMBUP)
		c->m_input->mouseButtonFlags1 ^= MG_MBFL_LMBUP;
	if ((c->m_input->mouseButtonFlags1 & MG_MBFL_RMBUP) == MG_MBFL_RMBUP)
		c->m_input->mouseButtonFlags1 ^= MG_MBFL_RMBUP;
	if ((c->m_input->mouseButtonFlags1 & MG_MBFL_MMBUP) == MG_MBFL_MMBUP)
		c->m_input->mouseButtonFlags1 ^= MG_MBFL_MMBUP;
	if ((c->m_input->mouseButtonFlags1 & MG_MBFL_X1MBUP) == MG_MBFL_X1MBUP)
		c->m_input->mouseButtonFlags1 ^= MG_MBFL_X1MBUP;
	if ((c->m_input->mouseButtonFlags1 & MG_MBFL_X2MBUP) == MG_MBFL_X2MBUP)
		c->m_input->mouseButtonFlags1 ^= MG_MBFL_X2MBUP;
	
	for (int i = 0; i < 256; ++i)
	{
		if ((c->m_input->keyFlags[i] & MG_KEYFL_HIT) == MG_KEYFL_HIT)
			c->m_input->keyFlags[i] ^= MG_KEYFL_HIT;

		if ((c->m_input->keyFlags[i] & MG_KEYFL_RELEASE) == MG_KEYFL_RELEASE)
			c->m_input->keyFlags[i] ^= MG_KEYFL_RELEASE;
	}
}
