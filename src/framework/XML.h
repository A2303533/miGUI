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
#ifndef _MGF_XML_H_
#define _MGF_XML_H_

#include <string>

namespace mgf
{
    namespace XML
    {
		enum class XPathTokenType
		{
			Slash,
			Double_slash,
			Name,
			Equal,			// price=9.80
			Not_equal,		// price!=9.80
			More,			// price>9.80
			Less,			// price<9.80
			More_eq,		// price>=9.80
			Less_eq,		// price<=9.80
			Apos,
			Number,
			Comma, //,
			Function,
			Function_open,  //(
			Function_close, //)
			Attribute,
			Bit_or,			// //book | //cd
			Sq_open,		// [
			Sq_close,		// ]
			Div,			// 8 div 4
			Mod,			// 5 mod 2
			Add,			// 6 + 4
			Sub,			// 6 - 4
			Mul,			// 6 * 4
			And,			// price>9.00 and price<9.90
			Or,				// price=9.80 or price=9.70,
			Axis_namespace,	//::
			Axis,
			NONE = 0xFFFFFFF
		};

		enum class XPathAxis
		{
			Ancestor,			// parent, grandparent, etc.
			Ancestor_or_self,	// parent, grandparent, etc. + current node
			Attribute,
			Child,
			Descendant,			// children, grandchildren, etc.
			Descendant_or_self,	// children, grandchildren, etc. + current node
			Following,
			Following_sibling,
			Namespace,
			Parent,
			Preceding,
			Preceding_sibling,
			Self,
			NONE = 0xFFFFFFF
		};

		struct XPathToken
		{
			XPathToken() :
				m_type(XPathTokenType::NONE),
				m_axis(XPathAxis::NONE),
				m_number(0.f)
			{}

			XPathToken(XPathTokenType type,
				StringU16 string,
				float number)
				: m_type(type),
				m_axis(XPathAxis::NONE),
				m_string(string),
				m_number(number)
			{}

			XPathTokenType    m_type;
			XPathAxis         m_axis;
			StringU16  m_string;
			float          m_number;
		};

		struct XMLAttribute : public BaseClass
		{
			XMLAttribute() {}
			XMLAttribute(const StringU16& Name,
				const StringU16& Value) :
				name(Name),
				value(Value)
			{}
			virtual ~XMLAttribute() {}
			StringU16 name;
			StringU16 value;
		};

		struct XMLNode : public BaseClass
		{
			XMLNode() {}
			XMLNode(const StringU16& Name) :
				name(Name)
			{}
			XMLNode(const XMLNode& node)
			{
				name = node.name;
				text = node.text;
				attributeList = node.attributeList;
				nodeList = node.nodeList;
			}

			virtual ~XMLNode()
			{
				clear();
			}

			StringU16 name;
			StringU16 text;
			std::vector<XMLAttribute*> attributeList;
			std::vector<XMLNode*> nodeList;

			void addAttribute(const StringU16& Name,
				const StringU16& Value)
			{
				attributeList.push_back(new XMLAttribute(Name, Value));
			}

			void addAttribute(XMLAttribute* a)
			{
				attributeList.push_back(a);
			}

			void addNode(XMLNode* node)
			{
				nodeList.push_back(node);
			}

			XMLNode& operator=(const XMLNode& node)
			{
				name = node.name;
				text = node.text;
				attributeList = node.attributeList;
				nodeList = node.nodeList;

				return *this;
			}

			XMLAttribute* getAttribute(const StringU16& Name)
			{
				size_t sz = attributeList.size();
				for (size_t i = 0; i < sz; ++i)
				{
					if (attributeList[i]->name == Name)
					{
						return attributeList[i];
					}
				}
				return nullptr;
			}

			void clear()
			{
				name.clear();
				text.clear();
				size_t sz = attributeList.size();
				for (size_t i = 0; i < sz; ++i)
				{
					delete attributeList[i];
				}
				sz = nodeList.size();
				for (size_t i = 0; i < sz; ++i)
				{
					delete nodeList[i];
				}
				attributeList.clear();
				nodeList.clear();
			}
		};

		class XMLDocument
		{
		public:
			XMLDocument() {}
			virtual ~XMLDocument() {}

			virtual              XMLNode* GetRootNode() = 0;
			virtual                  void Print() = 0;
			virtual const StringU16&      GetText() = 0;
			virtual std::vector<XMLNode*> SelectNodes(const StringU16& XPath_expression) = 0;
		};
    }
}

#endif
