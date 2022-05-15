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

#include "mgf.h"
#include "Framework.h"
#include "Color.h"

using namespace mgf;

void Framework::_initColors()
{
	m_colors[static_cast<uint32_t>(ColorName::AliceBlue)] = 0xffF0F8FF;
	m_colors[static_cast<uint32_t>(ColorName::AntiqueWhite)] = 0xffFAEBD7;
	m_colors[static_cast<uint32_t>(ColorName::Aqua)] = 0xff00FFFF;
	m_colors[static_cast<uint32_t>(ColorName::Aquamarine)] = 0xff7FFFD4;
	m_colors[static_cast<uint32_t>(ColorName::Azure)] = 0xffF0FFFF;
	m_colors[static_cast<uint32_t>(ColorName::Beige)] = 0xffF5F5DC;
	m_colors[static_cast<uint32_t>(ColorName::Bisque)] = 0xffFFE4C4;
	m_colors[static_cast<uint32_t>(ColorName::Black)] = 0xff000000;
	m_colors[static_cast<uint32_t>(ColorName::BlanchedAlmond)] = 0xffFFEBCD;
	m_colors[static_cast<uint32_t>(ColorName::Blue)] = 0xff0000FF;
	m_colors[static_cast<uint32_t>(ColorName::BlueViolet)] = 0xff8A2BE2;
	m_colors[static_cast<uint32_t>(ColorName::Brown)] = 0xffA52A2A;
	m_colors[static_cast<uint32_t>(ColorName::BurlyWood)] = 0xffDEB887;
	m_colors[static_cast<uint32_t>(ColorName::CadetBlue)] = 0xff5F9EA0;
	m_colors[static_cast<uint32_t>(ColorName::Chartreuse)] = 0xff7FFF00;
	m_colors[static_cast<uint32_t>(ColorName::Chocolate)] = 0xffD2691E;
	m_colors[static_cast<uint32_t>(ColorName::Coral)] = 0xffFF7F50;
	m_colors[static_cast<uint32_t>(ColorName::CornflowerBlue)] = 0xff6495ED;
	m_colors[static_cast<uint32_t>(ColorName::Cornsilk)] = 0xffFFF8DC;
	m_colors[static_cast<uint32_t>(ColorName::Crimson)] = 0xffDC143C;
	m_colors[static_cast<uint32_t>(ColorName::Cyan)] = 0xff00FFFF;
	m_colors[static_cast<uint32_t>(ColorName::DarkBlue)] = 0xff00008B;
	m_colors[static_cast<uint32_t>(ColorName::DarkCyan)] = 0xff008B8B;
	m_colors[static_cast<uint32_t>(ColorName::DarkGoldenRod)] = 0xffB8860B;
	m_colors[static_cast<uint32_t>(ColorName::DarkGray)] = 0xffA9A9A9;
	m_colors[static_cast<uint32_t>(ColorName::DarkGrey)] = 0xffA9A9A9;
	m_colors[static_cast<uint32_t>(ColorName::DarkGreen)] = 0xff006400;
	m_colors[static_cast<uint32_t>(ColorName::DarkKhaki)] = 0xffBDB76B;
	m_colors[static_cast<uint32_t>(ColorName::DarkMagenta)] = 0xff8B008B;
	m_colors[static_cast<uint32_t>(ColorName::DarkOliveGreen)] = 0xff556B2F;
	m_colors[static_cast<uint32_t>(ColorName::DarkOrange)] = 0xffFF8C00;
	m_colors[static_cast<uint32_t>(ColorName::DarkOrchid)] = 0xff9932CC;
	m_colors[static_cast<uint32_t>(ColorName::DarkRed)] = 0xff8B0000;
	m_colors[static_cast<uint32_t>(ColorName::DarkSalmon)] = 0xffE9967A;
	m_colors[static_cast<uint32_t>(ColorName::DarkSeaGreen)] = 0xff8FBC8F;
	m_colors[static_cast<uint32_t>(ColorName::DarkSlateBlue)] = 0xff483D8B;
	m_colors[static_cast<uint32_t>(ColorName::DarkSlateGray)] = 0xff2F4F4F;
	m_colors[static_cast<uint32_t>(ColorName::DarkSlateGrey)] = 0xff2F4F4F;
	m_colors[static_cast<uint32_t>(ColorName::DarkTurquoise)] = 0xff00CED1;
	m_colors[static_cast<uint32_t>(ColorName::DarkViolet)] = 0xff9400D3;
	m_colors[static_cast<uint32_t>(ColorName::DeepPink)] = 0xffFF1493;
	m_colors[static_cast<uint32_t>(ColorName::DeepSkyBlue)] = 0xff00BFFF;
	m_colors[static_cast<uint32_t>(ColorName::DimGray)] = 0xff696969;
	m_colors[static_cast<uint32_t>(ColorName::DimGrey)] = 0xff696969;
	m_colors[static_cast<uint32_t>(ColorName::DodgerBlue)] = 0xff1E90FF;
	m_colors[static_cast<uint32_t>(ColorName::FireBrick)] = 0xffB22222;
	m_colors[static_cast<uint32_t>(ColorName::FloralWhite)] = 0xffFFFAF0;
	m_colors[static_cast<uint32_t>(ColorName::ForestGreen)] = 0xff228B22;
	m_colors[static_cast<uint32_t>(ColorName::Fuchsia)] = 0xffFF00FF;
	m_colors[static_cast<uint32_t>(ColorName::Gainsboro)] = 0xffDCDCDC;
	m_colors[static_cast<uint32_t>(ColorName::GhostWhite)] = 0xffF8F8FF;
	m_colors[static_cast<uint32_t>(ColorName::Gold)] = 0xffFFD700;
	m_colors[static_cast<uint32_t>(ColorName::GoldenRod)] = 0xffDAA520;
	m_colors[static_cast<uint32_t>(ColorName::Gray)] = 0xff808080;
	m_colors[static_cast<uint32_t>(ColorName::Grey)] = 0xff808080;
	m_colors[static_cast<uint32_t>(ColorName::Green)] = 0xff008000;
	m_colors[static_cast<uint32_t>(ColorName::GreenYellow)] = 0xffADFF2F;
	m_colors[static_cast<uint32_t>(ColorName::HoneyDew)] = 0xffF0FFF0;
	m_colors[static_cast<uint32_t>(ColorName::HotPink)] = 0xffFF69B4;
	m_colors[static_cast<uint32_t>(ColorName::IndianRed)] = 0xffCD5C5C;
	m_colors[static_cast<uint32_t>(ColorName::Indigo)] = 0xff4B0082;
	m_colors[static_cast<uint32_t>(ColorName::Ivory)] = 0xffFFFFF0;
	m_colors[static_cast<uint32_t>(ColorName::Khaki)] = 0xffF0E68C;
	m_colors[static_cast<uint32_t>(ColorName::Lavender)] = 0xffE6E6FA;
	m_colors[static_cast<uint32_t>(ColorName::LavenderBlush)] = 0xffFFF0F5;
	m_colors[static_cast<uint32_t>(ColorName::LawnGreen)] = 0xff7CFC00;
	m_colors[static_cast<uint32_t>(ColorName::LemonChiffon)] = 0xffFFFACD;
	m_colors[static_cast<uint32_t>(ColorName::LightBlue)] = 0xffADD8E6;
	m_colors[static_cast<uint32_t>(ColorName::LightCoral)] = 0xffF08080;
	m_colors[static_cast<uint32_t>(ColorName::LightCyan)] = 0xffE0FFFF;
	m_colors[static_cast<uint32_t>(ColorName::LightGoldenRodYellow)] = 0xffFAFAD2;
	m_colors[static_cast<uint32_t>(ColorName::LightGray)] = 0xffD3D3D3;
	m_colors[static_cast<uint32_t>(ColorName::LightGrey)] = 0xffD3D3D3;
	m_colors[static_cast<uint32_t>(ColorName::LightGreen)] = 0xff90EE90;
	m_colors[static_cast<uint32_t>(ColorName::LightPink)] = 0xffFFB6C1;
	m_colors[static_cast<uint32_t>(ColorName::LightSalmon)] = 0xffFFA07A;
	m_colors[static_cast<uint32_t>(ColorName::LightSeaGreen)] = 0xff20B2AA;
	m_colors[static_cast<uint32_t>(ColorName::LightSkyBlue)] = 0xff87CEFA;
	m_colors[static_cast<uint32_t>(ColorName::LightSlateGray)] = 0xff778899;
	m_colors[static_cast<uint32_t>(ColorName::LightSlateGrey)] = 0xff778899;
	m_colors[static_cast<uint32_t>(ColorName::LightSteelBlue)] = 0xffB0C4DE;
	m_colors[static_cast<uint32_t>(ColorName::LightYellow)] = 0xffFFFFE0;
	m_colors[static_cast<uint32_t>(ColorName::Lime)] = 0xff00FF00;
	m_colors[static_cast<uint32_t>(ColorName::LimeGreen)] = 0xff32CD32;
	m_colors[static_cast<uint32_t>(ColorName::Linen)] = 0xffFAF0E6;
	m_colors[static_cast<uint32_t>(ColorName::Magenta)] = 0xffFF00FF;
	m_colors[static_cast<uint32_t>(ColorName::Maroon)] = 0xff800000;
	m_colors[static_cast<uint32_t>(ColorName::MediumAquaMarine)] = 0xff66CDAA;
	m_colors[static_cast<uint32_t>(ColorName::MediumBlue)] = 0xff0000CD;
	m_colors[static_cast<uint32_t>(ColorName::MediumOrchid)] = 0xffBA55D3;
	m_colors[static_cast<uint32_t>(ColorName::MediumPurple)] = 0xff9370DB;
	m_colors[static_cast<uint32_t>(ColorName::MediumSeaGreen)] = 0xff3CB371;
	m_colors[static_cast<uint32_t>(ColorName::MediumSlateBlue)] = 0xff7B68EE;
	m_colors[static_cast<uint32_t>(ColorName::MediumSpringGreen)] = 0xff00FA9A;
	m_colors[static_cast<uint32_t>(ColorName::MediumTurquoise)] = 0xff48D1CC;
	m_colors[static_cast<uint32_t>(ColorName::MediumVioletRed)] = 0xffC71585;
	m_colors[static_cast<uint32_t>(ColorName::MidnightBlue)] = 0xff191970;
	m_colors[static_cast<uint32_t>(ColorName::MintCream)] = 0xffF5FFFA;
	m_colors[static_cast<uint32_t>(ColorName::MistyRose)] = 0xffFFE4E1;
	m_colors[static_cast<uint32_t>(ColorName::Moccasin)] = 0xffFFE4B5;
	m_colors[static_cast<uint32_t>(ColorName::NavajoWhite)] = 0xffFFDEAD;
	m_colors[static_cast<uint32_t>(ColorName::Navy)] = 0xff000080;
	m_colors[static_cast<uint32_t>(ColorName::OldLace)] = 0xffFDF5E6;
	m_colors[static_cast<uint32_t>(ColorName::Olive)] = 0xff808000;
	m_colors[static_cast<uint32_t>(ColorName::OliveDrab)] = 0xff6B8E23;
	m_colors[static_cast<uint32_t>(ColorName::Orange)] = 0xffFFA500;
	m_colors[static_cast<uint32_t>(ColorName::OrangeRed)] = 0xffFF4500;
	m_colors[static_cast<uint32_t>(ColorName::Orchid)] = 0xffDA70D6;
	m_colors[static_cast<uint32_t>(ColorName::PaleGoldenRod)] = 0xffEEE8AA;
	m_colors[static_cast<uint32_t>(ColorName::PaleGreen)] = 0xff98FB98;
	m_colors[static_cast<uint32_t>(ColorName::PaleTurquoise)] = 0xffAFEEEE;
	m_colors[static_cast<uint32_t>(ColorName::PaleVioletRed)] = 0xffDB7093;
	m_colors[static_cast<uint32_t>(ColorName::PapayaWhip)] = 0xffFFEFD5;
	m_colors[static_cast<uint32_t>(ColorName::PeachPuff)] = 0xffFFDAB9;
	m_colors[static_cast<uint32_t>(ColorName::Peru)] = 0xffCD853F;
	m_colors[static_cast<uint32_t>(ColorName::Pink)] = 0xffFFC0CB;
	m_colors[static_cast<uint32_t>(ColorName::Plum)] = 0xffDDA0DD;
	m_colors[static_cast<uint32_t>(ColorName::PowderBlue)] = 0xffB0E0E6;
	m_colors[static_cast<uint32_t>(ColorName::Purple)] = 0xff800080;
	m_colors[static_cast<uint32_t>(ColorName::RebeccaPurple)] = 0xff663399;
	m_colors[static_cast<uint32_t>(ColorName::Red)] = 0xffFF0000;
	m_colors[static_cast<uint32_t>(ColorName::RosyBrown)] = 0xffBC8F8F;
	m_colors[static_cast<uint32_t>(ColorName::RoyalBlue)] = 0xff4169E1;
	m_colors[static_cast<uint32_t>(ColorName::SaddleBrown)] = 0xff8B4513;
	m_colors[static_cast<uint32_t>(ColorName::Salmon)] = 0xffFA8072;
	m_colors[static_cast<uint32_t>(ColorName::SandyBrown)] = 0xffF4A460;
	m_colors[static_cast<uint32_t>(ColorName::SeaGreen)] = 0xff2E8B57;
	m_colors[static_cast<uint32_t>(ColorName::SeaShell)] = 0xffFFF5EE;
	m_colors[static_cast<uint32_t>(ColorName::Sienna)] = 0xffA0522D;
	m_colors[static_cast<uint32_t>(ColorName::Silver)] = 0xffC0C0C0;
	m_colors[static_cast<uint32_t>(ColorName::SkyBlue)] = 0xff87CEEB;
	m_colors[static_cast<uint32_t>(ColorName::SlateBlue)] = 0xff6A5ACD;
	m_colors[static_cast<uint32_t>(ColorName::SlateGray)] = 0xff708090;
	m_colors[static_cast<uint32_t>(ColorName::SlateGrey)] = 0xff708090;
	m_colors[static_cast<uint32_t>(ColorName::Snow)] = 0xffFFFAFA;
	m_colors[static_cast<uint32_t>(ColorName::SpringGreen)] = 0xff00FF7F;
	m_colors[static_cast<uint32_t>(ColorName::SteelBlue)] = 0xff4682B4;
	m_colors[static_cast<uint32_t>(ColorName::Tan)] = 0xffD2B48C;
	m_colors[static_cast<uint32_t>(ColorName::Teal)] = 0xff008080;
	m_colors[static_cast<uint32_t>(ColorName::Thistle)] = 0xffD8BFD8;
	m_colors[static_cast<uint32_t>(ColorName::Tomato)] = 0xffFF6347;
	m_colors[static_cast<uint32_t>(ColorName::Turquoise)] = 0xff40E0D0;
	m_colors[static_cast<uint32_t>(ColorName::Violet)] = 0xffEE82EE;
	m_colors[static_cast<uint32_t>(ColorName::Wheat)] = 0xffF5DEB3;
	m_colors[static_cast<uint32_t>(ColorName::White)] = 0xffffffff;
	m_colors[static_cast<uint32_t>(ColorName::WhiteSmoke)] = 0xffF5F5F5;
	m_colors[static_cast<uint32_t>(ColorName::Yellow)] = 0xffFFFF00;
	m_colors[static_cast<uint32_t>(ColorName::YellowGreen)] = 0xff9ACD32;
}

