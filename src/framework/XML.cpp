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
#include "XML.h"
#include "FileSystem.h"
#include "FileBuffer.h"

#include <filesystem>

using namespace mgf;
using namespace mgf::XML;

class XMLDocumentImpl : public XML::XMLDocument
{
	bool		m_isInit;
	XML::XMLNode	m_root;
	StringU16	m_fileName;
	StringU16	m_text;

	StringU16 m_expect_apos;
	StringU16 m_expect_quot;
	StringU16 m_expect_eq;
	StringU16 m_expect_slash;
	StringU16 m_expect_lt;
	StringU16 m_expect_gt;
	StringU16 m_expect_sub;
	StringU16 m_expect_ex;

	size_t m_cursor, m_sz;

	enum _toke_type
	{
		tt_default,
		tt_string
	};
	struct _token
	{
		_token(StringU16 N, uint32_t R, uint32_t C, _toke_type t = _toke_type::tt_default) :
			name(N), line(R), col(C), type(t)
		{
		}
		StringU16 name;
		uint32_t line;
		uint32_t col;
		_toke_type type;
	};

	std::vector<_token> m_tokens;

	void getTokens();
	void decodeEnts(StringU16& outText);

	char16_t* getName(char16_t* ptr, StringU16& outText, uint32_t& line, uint32_t& col);
	char16_t* getString(char16_t* ptr, StringU16& outText, uint32_t& line, uint32_t& col);
	char16_t* skipSpace(char16_t* ptr, uint32_t& line, uint32_t& col);

	bool charForName(char16_t* ptr);
	bool charForString(char16_t* ptr);
	bool charIsSymbol(char16_t* ptr);

	bool analyzeTokens();
	bool buildXMLDocument();
	bool getSubNode(XMLNode* node);
	bool getAttributes(XMLNode* node);
	bool tokenIsName();
	bool nextToken();
	bool unexpectedToken(const _token& token, StringU16 expected);

	void skipPrologAndDTD();

	void printNode(XMLNode* node, uint32_t indent);

	bool tokenIsString();

	bool XPathGetTokens(std::vector<XPathToken>* arr, const StringU16& XPath_expression);
	bool XPathIsName(char16_t* ptr);

	char16_t* XPathGetName(char16_t* ptr, StringU16* name);
	void XPathGetNodes(uint32_t level, uint32_t maxLevel, std::vector<StringU16*> elements, XMLNode* node, std::vector<XMLNode*>* outArr);
public:
	
	XMLDocumentImpl();
	XMLDocumentImpl(const StringU16& fileName);
	virtual ~XMLDocumentImpl();

	bool init();
	bool init(const uint8_t* data, size_t dataSize);

	virtual XMLNode* GetRootNode() override;
	virtual void Print() override;
	virtual const StringU16& GetText() override;
	virtual std::vector<XMLNode*> SelectNodes(const StringU16& XPath_expression) override;
};

XMLDocumentImpl::XMLDocumentImpl()
	:
	m_isInit(false)
{
	m_expect_apos = u"\'";
	m_expect_quot = u"\"";
	m_expect_eq = u"=";
	m_expect_slash = u"/";
	m_expect_lt = u"<";
	m_expect_gt = u">";
	m_expect_sub = u"-";
	m_expect_ex = u"!";
	m_cursor = m_sz = 0;
}

XMLDocumentImpl::XMLDocumentImpl(const StringU16& fileName) :
	XMLDocumentImpl()
{
	m_fileName = fileName;
}

XMLDocumentImpl::~XMLDocumentImpl()
{
}

bool XMLDocumentImpl::init(const uint8_t* data, size_t dataSize)
{
	size_t sz = dataSize;
	if (sz < 4)
		return false;

	FileBuffer fb(data, dataSize);

	uint8_t bom[3u];
	fb.read(bom, 3u);
	fb.seek(0u, SEEK_SET);

	bool isUTF8 = false;
	bool isBE = false;

	if (bom[0u] == 0xEF) {
		fb.seek(3, SEEK_SET);
		isUTF8 = true;
		sz -= 3u;
	}
	else if (bom[0u] == 0xFE) { // utf16 BE
		fb.seek(2, SEEK_SET);
		isBE = true;
		sz -= 2u;
	}
	else if (bom[0u] == 0xFF) {
		fb.seek(2, SEEK_SET);
		sz -= 2u;
	}
	else {
		// else - utf8 w/o bom
		isUTF8 = true;
	}

	StringA textBytes;
	textBytes.reserve((uint32_t)sz);
	textBytes.setSize((uint32_t)sz);
	fb.read((uint8_t*)textBytes.data(), sz);

	if (!isUTF8) {
		union {
			char16_t unicode;
			char b[2u];
		}un;
		for (uint32_t i = 0u; i < sz; i += 2u) {
			/*char16_t ch16 = textBytes[ i ];
			if( isBE ){
				ch16 <<= kkConst8U;
				ch16 |= textBytes[ i + 1u ];
			}else{
				char16_t ch16_2 = textBytes[ i + 1u ];
				ch16_2 <<= kkConst8U;
				ch16 |= ch16_2;
			}*/

			if (isBE) {
				un.b[0u] = textBytes[i + 1u];
				un.b[1u] = textBytes[i];
			}
			else {
				un.b[0u] = textBytes[i];
				un.b[1u] = textBytes[i + 1u];
			}

			m_text += un.unicode;
		}

	}
	else
	{
		util::string_UTF8_to_UTF16(m_text, textBytes);
	}

	getTokens();

	if (!analyzeTokens())
		return false;

	m_tokens.clear();
	m_isInit = true;

	return true;
}

bool XMLDocumentImpl::init()
{
	FileSystem fs;
	if (fs.ExistFile(m_fileName))
	{
		if (!util::readTextFromFileForUnicode(m_fileName, m_text))
		{
			return false;
		}
	}
	else
	{
		m_text = m_fileName;
	}

	getTokens();

	if (!analyzeTokens())
		return false;

	m_tokens.clear();
	m_isInit = true;

	return true;
}

void XMLDocumentImpl::skipPrologAndDTD()
{
	uint32_t sz = (uint32_t)m_tokens.size();
	while (m_cursor < sz) {
		if (m_tokens[m_cursor].name == m_expect_gt) {
			++m_cursor;
			return;
		}
		else {
			++m_cursor;
		}
	}
}

bool XMLDocumentImpl::tokenIsName()
{
	if (m_tokens[m_cursor].name == m_expect_lt) return false;
	if (m_tokens[m_cursor].name == m_expect_gt) return false;
	if (m_tokens[m_cursor].name == m_expect_sub) return false;
	if (m_tokens[m_cursor].name == m_expect_ex) return false;
	if (m_tokens[m_cursor].name == m_expect_eq) return false;
	if (m_tokens[m_cursor].name == m_expect_apos) return false;
	if (m_tokens[m_cursor].name == m_expect_quot) return false;
	if (m_tokens[m_cursor].name == m_expect_slash) return false;
	return true;
}

bool XMLDocumentImpl::nextToken()
{
	++m_cursor;
	if (m_cursor >= m_sz)
	{
		fprintf(stderr, "End of XML\n");
		return true;
	}
	//	NAME = m_tokens[ m_cursor ].name;
	return false;
}

bool XMLDocumentImpl::unexpectedToken(const _token& token, StringU16 expected)
{
	fprintf(stderr, "XML: Unexpected token: %s Line:%u Col:%u\n", token.name.to_utf8().data(), token.line, token.col);
	fprintf(stderr, "XML: Expected: %s\n", expected.to_utf8().data());
	return false;
}

bool XMLDocumentImpl::tokenIsString()
{
	return m_tokens[m_cursor].type == XMLDocumentImpl::_toke_type::tt_string;
}

bool XMLDocumentImpl::getAttributes(XMLNode* node)
{
	for (;;)
	{
		Ptr<XMLAttribute> at = new XMLAttribute();
		if (nextToken())
			return false;

		if (tokenIsName())
		{
			at.m_data->name = m_tokens[m_cursor].name;

			if (nextToken())
				return false;

			if (m_tokens[m_cursor].name == m_expect_eq)
			{
				if (nextToken())
					return false;

				if (m_tokens[m_cursor].name == m_expect_apos)
				{
					if (nextToken())
						return false;

					//if( tokenIsName() ){
					if (tokenIsString())
					{
						at.m_data->value = m_tokens[m_cursor].name;
						if (nextToken())
							return false;

						if (m_tokens[m_cursor].name == m_expect_apos)
						{
							///							at->addRef();
							node->addAttribute(at.m_data);
							at = nullptr; /// âìåñòî at->addRef();
							continue;
						}
						else
						{
							return unexpectedToken(m_tokens[m_cursor], m_expect_apos);
						}
					}
				}
				else if (m_tokens[m_cursor].name == m_expect_quot)
				{
					if (nextToken())
						return false;

					//if( tokenIsName() ){ //is string
					if (tokenIsString())
					{
						at.m_data->value = m_tokens[m_cursor].name;
						if (nextToken())
							return false;

						if (m_tokens[m_cursor].name == m_expect_quot)
						{
							///							at->addRef();
							node->addAttribute(at.m_data);
							at = nullptr; /// âìåñòî at->addRef();
							continue;
						}
						else
						{
							return unexpectedToken(m_tokens[m_cursor], m_expect_quot);
						}
					}
				}
				else
				{
					return unexpectedToken(m_tokens[m_cursor], u"\' or \"");
				}
			}
			else
			{
				return unexpectedToken(m_tokens[m_cursor], m_expect_eq);
			}
		}
		else if (m_tokens[m_cursor].name == m_expect_gt || m_tokens[m_cursor].name == m_expect_slash)
		{
			return true;
		}
		else
		{
			return unexpectedToken(m_tokens[m_cursor], u"attribute or / or >");
		}
	}
	return false;
}

bool XMLDocumentImpl::getSubNode(XMLNode* node)
{
	Ptr<XMLNode> subNode = new XMLNode();

	StringU16 name;

	bool next = false;
	while (m_cursor < m_sz)
	{
		if (m_tokens[m_cursor].name == m_expect_lt)
		{
			if (nextToken())
				return false;

			if (tokenIsName())
			{
				name = m_tokens[m_cursor].name;
				node->name = name;
				if (nextToken())
					return false;

				// First - attributes
				if (tokenIsName())
				{
					--m_cursor;
					if (!getAttributes(node))
					{
						return false;
					}
				}

				if (m_tokens[m_cursor].name == m_expect_gt)
				{
					if (nextToken())
						return false;

					if (tokenIsName())
					{
						node->text = m_tokens[m_cursor].name;

						if (nextToken())
							return false;

					closeNode:
						if (m_tokens[m_cursor].name == m_expect_lt)
						{
							if (nextToken())
								return false;

							if (m_tokens[m_cursor].name == m_expect_slash)
							{
								if (nextToken())
									return false;

								if (m_tokens[m_cursor].name == name)
								{
									if (nextToken())
										return false;

									if (m_tokens[m_cursor].name == m_expect_gt)
									{
										++m_cursor;
										return true;
									}
									else
									{
										return unexpectedToken(m_tokens[m_cursor], m_expect_gt);
									}
								}
								else
								{
									return unexpectedToken(m_tokens[m_cursor], name);
								}
							}
							else if (tokenIsName())
							{
								--m_cursor;
								subNode = new XMLNode();
								goto newNode;
							}
							else
							{
								return unexpectedToken(m_tokens[m_cursor], m_expect_slash);
							}
						}
						else
						{
							return unexpectedToken(m_tokens[m_cursor], m_expect_lt);
						}
					}
					else if (m_tokens[m_cursor].name == m_expect_lt) { // next or </
						if (nextToken())
							return false;

						if (tokenIsName())
						{ // next node
							next = true;
							--m_cursor;
						}
						else if (m_tokens[m_cursor].name == m_expect_slash) { // return true
							if (nextToken())
								return false;

							if (m_tokens[m_cursor].name == name)
							{
								if (nextToken())
									return false;

								if (m_tokens[m_cursor].name == m_expect_gt)
								{
									++m_cursor;
									return true;
								}
								else
								{
									return unexpectedToken(m_tokens[m_cursor], m_expect_gt);
								}
							}
							else
							{
								return unexpectedToken(m_tokens[m_cursor], name);
							}
						}
						else
						{
							return unexpectedToken(m_tokens[m_cursor], u"/ or <entity>");
						}
					}
					else
					{
						return unexpectedToken(m_tokens[m_cursor], u"\"text\" or <entity>");
					}
				}
				else if (m_tokens[m_cursor].name == m_expect_slash)
				{
					if (nextToken())
						return false;

					if (m_tokens[m_cursor].name == m_expect_gt)
					{
						++m_cursor;
						return true;
					}
					else
					{
						return unexpectedToken(m_tokens[m_cursor], m_expect_gt);
					}
				}
				else
				{
					return unexpectedToken(m_tokens[m_cursor], u"> or /");
				}
			}
			else
			{
				return unexpectedToken(m_tokens[m_cursor], u"name");
			}

		}
		else
		{
			return unexpectedToken(m_tokens[m_cursor], m_expect_lt);
		}

		if (next)
		{
		newNode:
			if (getSubNode(subNode.m_data)) {
				///				subNode->addRef();

				node->addNode(subNode.m_data);

				subNode = nullptr; /// âìåñòî subNode->addRef();

				--m_cursor;
				if (nextToken())
					return false;

				if (m_tokens[m_cursor].name == m_expect_lt)
				{
					if (nextToken())
						return false;

					if (m_tokens[m_cursor].name == m_expect_slash)
					{
						--m_cursor;
						goto closeNode;
					}
					else if (tokenIsName())
					{
						--m_cursor;
						subNode = new XMLNode();
						goto newNode;
					}
					else
					{
						return unexpectedToken(m_tokens[m_cursor], u"</close tag> or <new tag>");
					}
				}
				else if (tokenIsName())
				{
					node->text = m_tokens[m_cursor].name;

					if (nextToken())
						return false;

					if (m_tokens[m_cursor].name == m_expect_lt)
					{
						if (nextToken())
							return false;

						if (m_tokens[m_cursor].name == m_expect_slash)
						{
							--m_cursor;
							goto closeNode;
						}
						else if (tokenIsName())
						{
							--m_cursor;
							//subNode.clear();
							subNode = new XMLNode();
							goto newNode;
						}
						else
						{
							return unexpectedToken(m_tokens[m_cursor], u"</close tag> or <new tag>");
						}
					}
					else
					{
						return unexpectedToken(m_tokens[m_cursor], m_expect_lt);
					}
				}

			}
			else {
				return false;
			}
		}
	}

	return true;
}

bool XMLDocumentImpl::analyzeTokens()
{
	uint32_t sz = (uint32_t)m_tokens.size();
	if (!sz)
	{
		fprintf(stderr, "Empty XML\n");
		return false;
	}

	m_cursor = 0;
	if (m_tokens[0].name == u"<")
	{
		if (m_tokens[1].name == u"?")
		{
			if (m_tokens[2].name == u"xml")
			{
				m_cursor = 2;
				skipPrologAndDTD();
			}
		}
	}

	if (m_tokens[m_cursor].name == u"<")
	{
		if (m_tokens[m_cursor + 1].name == u"!")
		{
			if (m_tokens[m_cursor + 2].name == u"DOCTYPE")
				skipPrologAndDTD();
		}
	}

	return buildXMLDocument();
}

bool XMLDocumentImpl::buildXMLDocument()
{
	m_sz = (uint32_t)m_tokens.size();
	return getSubNode(&m_root);
}

XMLNode* XMLDocumentImpl::GetRootNode()
{
	return &m_root;
}

bool XMLDocumentImpl::charForName(char16_t* ptr)
{
	char16_t c = *ptr;
	if (c > 0x80) return true;
	if (util::isAlpha(*ptr)
		|| util::isDigit(*ptr)
		|| (*ptr == u'_')
		|| (*ptr == u'.'))
	{
		return true;
	}
	return false;
}

bool XMLDocumentImpl::charForString(char16_t* ptr)
{
	char16_t c = *ptr;
	if (c > 0x80) return true;
	if (util::isAlpha(*ptr)
		|| util::isDigit(*ptr)
		|| (*ptr == u'_')
		|| (*ptr == u'.'))
	{
		return true;
	}
	return false;
}

bool XMLDocumentImpl::charIsSymbol(char16_t* ptr)
{
	char16_t c = *ptr;
	if ((c == u'<') || (c == u'>')
		|| (c == u'/') || (c == u'\'')
		|| (c == u'\"') || (c == u'=')
		|| (c == u'?') || (c == u'!')
		|| (c == u'-'))
	{
		return true;
	}
	return false;
}

char16_t* XMLDocumentImpl::getName(char16_t* ptr, StringU16& outText, uint32_t& /*line*/, uint32_t& col)
{
	while (*ptr)
	{
		if (charForName(ptr))
		{
			outText += *ptr;
		}
		else
		{
			return ptr;
		}
		++ptr;
		++col;
	}
	return ptr;
}

char16_t* XMLDocumentImpl::getString(char16_t* ptr, StringU16& outText, uint32_t& line, uint32_t& col)
{
	while (*ptr)
	{
		if (*ptr == u'\n')
		{
			++line;
			col = 1;
			outText += *ptr;
			++ptr;
		}
		else if (*ptr == u'<')
		{
			break;
		}
		else
		{
			outText += *ptr;
			++col;
			++ptr;
		}
	}
	return ptr;
}

char16_t* XMLDocumentImpl::skipSpace(char16_t* ptr, uint32_t& line, uint32_t& col)
{
	while (*ptr)
	{
		if (*ptr == u'\n')
		{
			++line;
			col = 1;
			++ptr;
		}
		else if ((*ptr == u'\r')
			|| (*ptr == u'\t')
			|| (*ptr == u' '))
		{
			++col;
			++ptr;
		}
		else
			break;
	}
	return ptr;
}

void XMLDocumentImpl::decodeEnts(StringU16& str)
{
	util::stringReplaseSubString(str, StringU16(u"&apos;"), StringU16(u"\'"));
	util::stringReplaseSubString(str, StringU16(u"&quot;"), StringU16(u"\""));
	util::stringReplaseSubString(str, StringU16(u"&lt;"), StringU16(u"<"));
	util::stringReplaseSubString(str, StringU16(u"&kk;"), StringU16(u">"));
	util::stringReplaseSubString(str, StringU16(u"&amp;"), StringU16(u"&"));
}

void XMLDocumentImpl::getTokens()
{
	char16_t* ptr = m_text.data();
	uint32_t line = 1;
	uint32_t col = 1;

	bool isString = false;

	bool stringType = false; // "

	StringU16 str;

	uint32_t oldCol = 0;

	while (*ptr)
	{
		if (*ptr == u'\n')
		{
			col = 0;
			++line;
		}
		else
		{
			if (!isString)
			{
				if (charIsSymbol(ptr))
				{
					m_tokens.push_back(_token(StringU16(*ptr), line, col));

					if (*ptr == u'\'')
					{
						oldCol = col;
						str.clear();
						isString = true;
						stringType = true;
					}
					else if (*ptr == u'\"')
					{
						oldCol = col;
						isString = true;
						stringType = false;
						str.clear();
					}
					else if (*ptr == u'>')
					{
						++ptr;
						++col;
						ptr = skipSpace(ptr, line, col);
						oldCol = col;
						ptr = getString(ptr, str, line, col);
						if (str.size())
						{
							str.trim_spaces();
							//util::stringTrimSpace(str);
							decodeEnts(str);
							m_tokens.push_back(_token(str, line, oldCol));
							str.clear();
						}
						continue;
					}

				}
				else if (charForName(ptr))
				{
					oldCol = col;
					StringU16 name;
					ptr = getName(ptr, name, line, col);
					m_tokens.push_back(_token(name, line, oldCol));
					continue;
				}
			}
			else
			{
				if (stringType) { // '
					if (*ptr == u'\'')
					{
						decodeEnts(str);
						m_tokens.push_back(_token(str, line, oldCol + 1, XMLDocumentImpl::_toke_type::tt_string));
						m_tokens.push_back(_token(StringU16(*ptr), line, col));
						str.clear();
						isString = false;
						goto chponk;
					}
				}
				else { // "
					if (*ptr == u'\"')
					{
						decodeEnts(str);
						m_tokens.push_back(_token(str, line, oldCol + 1, XMLDocumentImpl::_toke_type::tt_string));
						m_tokens.push_back(_token(StringU16(*ptr), line, col));
						str.clear();
						isString = false;
						goto chponk;
					}
				}
				str += *ptr;
			}
		}
	chponk:

		++col;
		++ptr;

	}
}

void XMLDocumentImpl::printNode(XMLNode* node, uint32_t indent)
{
	if (node->name.size())
	{
		StringU16 line;
		for (uint32_t i = 0; i < indent; ++i)
		{
			line += u" ";
		}

		line += u"<";
		line += node->name;
		line += u">";

		if (node->attributeList.size())
		{
			line += u" ( ";

			for (uint32_t i = 0; i < node->attributeList.size(); ++i)
			{
				const XMLAttribute* at = node->attributeList[i];

				if (at->name.size())
				{
					line += at->name;
					line += u":";
					if (at->value.size()) {
						line += u"\"";
						line += at->value;
						line += u"\"";
						line += u" ";
					}
					else {
						line += u"ERROR ";
					}
				}

			}
			line += u" )";
		}

		if (node->text.size())
		{
			line += u" = ";
			line += node->text;
		}
		fprintf(stdout, "%s\n", line.to_utf8().data());

		if (node->nodeList.size())
		{
			for (uint32_t i = 0; i < node->nodeList.size(); ++i)
			{
				printNode(node->nodeList[i], ++indent);
				--indent;
			}
		}

	}
}

void XMLDocumentImpl::Print()
{
	printf("XML:n");
	printNode(&m_root, 0);
}

const StringU16& XMLDocumentImpl::GetText() {
	return m_text;
}

bool XMLDocumentImpl::XPathIsName(char16_t* ptr)
{

	if (*ptr == u':')
	{
		if (*(ptr + 1) == u':')
		{
			return false;
		}
	}

	switch (*ptr)
	{
	case u'/':
	case u'*':
	case u'\'':
	case u',':
	case u'=':
	case u'+':
	case u'-':
	case u'@':
	case u'[':
	case u']':
	case u'(':
	case u')':
	case u'|':
	case u'!':
		return false;
	}

	return true;
}

bool XMLDocumentImpl::XPathGetTokens(std::vector<XPathToken>* arr, const StringU16& XPath_expression)
{
	StringU16 name;
	char16_t* ptr = XPath_expression.data();
	char16_t next;
	while (*ptr)
	{
		name.clear();
		next = *(ptr + 1);

		XPathToken token;

		if (*ptr == u'/')
		{
			if (next)
			{
				if (next == u'/')
				{
					++ptr;
					token.m_type = XPathTokenType::Double_slash;
				}
				else
				{
					token.m_type = XPathTokenType::Slash;
				}
			}
			else
			{
				token.m_type = XPathTokenType::Slash;
			}
		}
		else if (*ptr == u'*') {
			token.m_type = XPathTokenType::Mul;
		}
		else if (*ptr == u'=') {
			token.m_type = XPathTokenType::Equal;
		}
		else if (*ptr == u'\'') {
			token.m_type = XPathTokenType::Apos;
		}
		else if (*ptr == u'@') {
			token.m_type = XPathTokenType::Attribute;
		}
		else if (*ptr == u'|') {
			token.m_type = XPathTokenType::Bit_or;
		}
		else if (*ptr == u',') {
			token.m_type = XPathTokenType::Comma;
		}
		else if (*ptr == u'+') {
			token.m_type = XPathTokenType::Add;
		}
		else if (*ptr == u'+') {
			token.m_type = XPathTokenType::Sub;
		}
		else if (*ptr == u'[') {
			token.m_type = XPathTokenType::Sq_open;
		}
		else if (*ptr == u']') {
			token.m_type = XPathTokenType::Sq_close;
		}
		else if (*ptr == u'(') {
			token.m_type = XPathTokenType::Function_open;
		}
		else if (*ptr == u')') {
			token.m_type = XPathTokenType::Function_close;
		}
		else if (*ptr == u'<') {
			if (next) {
				if (next == u'=') {
					++ptr;
					token.m_type = XPathTokenType::Less_eq;
				}
				else {
					token.m_type = XPathTokenType::Less;
				}
			}
			else {
				token.m_type = XPathTokenType::Less;
			}
		}
		else if (*ptr == u'>') {
			if (next) {
				if (next == u'/') {
					++ptr;
					token.m_type = XPathTokenType::More_eq;
				}
				else {
					token.m_type = XPathTokenType::More;
				}
			}
			else {
				token.m_type = XPathTokenType::More;
			}
		}
		else if (*ptr == u':') {
			if (next) {
				if (next == u':') {
					++ptr;
					token.m_type = XPathTokenType::Axis_namespace;
				}
				else {
					fprintf(stderr, "XPath: Bad token\n");
					return false;
				}
			}
			else {
				fprintf(stderr, "XPath: Bad tokenn");
				return false;
			}
		}
		else if (*ptr == u'!')
		{
			if (next) {
				if (next == u'=') {
					++ptr;
					token.m_type = XPathTokenType::Not_equal;
				}
				else {
					fprintf(stderr, "XPath: Bad token\n");
					return false;
				}
			}
			else {
				fprintf(stderr, "XPath: Bad token\n");
				return false;
			}
		}
		else if (XPathIsName(ptr))
		{
			ptr = XPathGetName(ptr, &name);
			token.m_type = XPathTokenType::Name;
			token.m_string = name;
		}
		else {
			fprintf(stderr, "XPath: Bad token\n");
			return false;
		}

		arr->push_back(token);

		++ptr;
	}

	return true;
}

char16_t* XMLDocumentImpl::XPathGetName(char16_t* ptr, StringU16* name)
{
	while (*ptr)
	{
		if (XPathIsName(ptr)) *name += *ptr;
		else {
			break;
		}
		++ptr;
	}
	--ptr;
	return ptr;
}

std::vector<XMLNode*> XMLDocumentImpl::SelectNodes(const StringU16& XPath_expression)
{
	std::vector<XMLNode*> a = std::vector<XMLNode*>(0xff);

	if (!m_isInit)
	{
		fprintf(stderr, "Bad kkXMLDocument\n");
		return a;
	}

	std::vector<XPathToken> XPathTokens;

	if (!XPathGetTokens(&XPathTokens, XPath_expression))
	{
		fprintf(stderr, "Bad XPath expression\n");
		return a;
	}

	std::vector<StringU16*> elements;

	uint32_t next = 0;
	uint32_t sz = (uint32_t)XPathTokens.size();
	for (uint32_t i = 0; i < sz; ++i)
	{
		next = i + 1;
		if (i == 0)
		{
			if (XPathTokens[i].m_type != XPathTokenType::Slash && XPathTokens[i].m_type != XPathTokenType::Double_slash)
			{
				fprintf(stderr, "Bad XPath expression \"%s\". Expression must begin with `/`\n", XPath_expression.to_utf8().data());
				return a;
			}
		}

		switch (XPathTokens[i].m_type) {
		case XPathTokenType::Slash:
			if (next >= sz) {
				fprintf(stderr, "Bad XPath expression\n");
				return a;
			}
			if (XPathTokens[next].m_type == XPathTokenType::Name) {
				elements.push_back(&XPathTokens[next].m_string);
				++i;
			}
			else {
				fprintf(stderr, "Bad XPath expression \"%s\". Expected XML element name\n", XPath_expression.to_utf8().data());
				return a;
			}
			break;

		case XPathTokenType::Double_slash:
			break;
		case XPathTokenType::Name:
			break;
		case XPathTokenType::Equal:
			break;
		case XPathTokenType::Not_equal:
			break;
		case XPathTokenType::More:
			break;
		case XPathTokenType::Less:
			break;
		case XPathTokenType::More_eq:
			break;
		case XPathTokenType::Less_eq:
			break;
		case XPathTokenType::Apos:
			break;
		case XPathTokenType::Number:
			break;
		case XPathTokenType::Comma:
			break;
		case XPathTokenType::Function:
			break;
		case XPathTokenType::Function_open:
			break;
		case XPathTokenType::Function_close:
			break;
		case XPathTokenType::Attribute:
			break;
		case XPathTokenType::Bit_or:
			break;
		case XPathTokenType::Sq_open:
			break;
		case XPathTokenType::Sq_close:
			break;
		case XPathTokenType::Div:
			break;
		case XPathTokenType::Mod:
			break;
		case XPathTokenType::Add:
			break;
		case XPathTokenType::Sub:
			break;
		case XPathTokenType::Mul:
			break;
		case XPathTokenType::And:
			break;
		case XPathTokenType::Or:
			break;
		case XPathTokenType::Axis_namespace:
			break;
		case XPathTokenType::Axis:
			break;
		case XPathTokenType::NONE:
			break;
		}

	}

	if (elements.size())
	{
		sz = (uint32_t)elements.size();
		XPathGetNodes(0, sz - 1, elements, &m_root, &a);

	}
	return a;
}

void XMLDocumentImpl::XPathGetNodes(
	uint32_t level,
	uint32_t maxLevel,
	std::vector<StringU16*> elements,
	XMLNode* node,
	std::vector<XMLNode*>* outArr)
{
	//_______________________________
	if (node->name == *elements[level])
	{
		if (level == maxLevel) {
			outArr->push_back(node);
			return;
		}
		else {
			uint32_t sz = (uint32_t)node->nodeList.size();
			for (uint32_t i = 0; i < sz; ++i) {
				XPathGetNodes(level + 1, maxLevel, elements, node->nodeList[i], outArr);
			}
		}

	}
}

XML::XMLDocument* FileSystem::XMLRead(const uint8_t* data, size_t dataSize)
{
	XMLDocumentImpl* xml = new XMLDocumentImpl();
	if (!xml)
		return nullptr;

	if (!xml->init(data, dataSize))
	{
		delete xml;
		return nullptr;
	}
	return xml;
}

XML::XMLDocument* FileSystem::XMLRead(const char* file)
{
	XMLDocumentImpl* xml = new XMLDocumentImpl(file);
	if (!xml)
		return nullptr;

	if (!xml->init())
	{
		delete xml;
		return nullptr;
	}
	return xml;
}

void writeText(StringU16& outText, const StringU16& inText)
{
	size_t sz = inText.size();
	for (size_t i = 0; i < sz; ++i) {
		if (inText[i] == u'\'') {
			outText += u"&apos;";
		}
		else if (inText[i] == u'\"') {
			outText += u"&quot;";
		}
		else if (inText[i] == u'<') {
			outText += u"&lt;";
		}
		else if (inText[i] == u'>') {
			outText += u"&gt;";
		}
		else if (inText[i] == u'&') {
			outText += u"&amp;";
		}
		else {
			outText += inText[i];
		}
	}
}

void writeName(StringU16& outText, const StringU16& inText)
{
	outText += "<";
	outText += inText;
}

bool writeNodes(StringU16& outText, XMLNode* node, uint32_t tabCount)
{
	for (uint32_t i = 0; i < tabCount; ++i)
	{
		outText += u"\t";
	}

	++tabCount;

	writeName(outText, node->name);

	size_t sz = node->attributeList.size();
	if (sz)
	{
		for (size_t i = 0; i < sz; ++i) {
			outText += u" ";
			outText += node->attributeList[i]->name;
			outText += u"=";
			outText += u"\"";
			writeText(outText, node->attributeList[i]->value);
			outText += u"\"";
		}
	}

	if (!node->nodeList.size() && !node->text.size())
	{
		outText += u"/>\r\n";
		return true;
	}
	else
	{
		outText += u">\r\n";
		sz = node->nodeList.size();
		for (size_t i = 0; i < sz; ++i) {
			if (!writeNodes(outText, node->nodeList[i], tabCount)) {
				for (size_t o = 0; o < tabCount; ++o) {
					outText += u"\t";
				}
				outText += u"</";
				outText += node->nodeList[i]->name;
				outText += u">\n";
			}
		}
	}

	if (node->text.size())
	{
		for (size_t o = 0; o < tabCount; ++o)
		{
			outText += u"\t";
		}
		writeText(outText, node->text);
		outText += u"\n";
	}
	--tabCount;

	return false;
}

void FileSystem::XMLWrite(const char* file, XML::XMLNode* rootNode, bool utf8)
{
	StringU16 outText(u"<?xml version=\"1.0\"");
	if (utf8) outText += " encoding=\"UTF-8\"";
	outText += " ?>\r\n";

	writeNodes(outText, rootNode, 0);
	outText += u"</";
	outText += rootNode->name;
	outText += u">\n";

	File * out = util::createFileForWriteText(file);
	if (out)
	{
		TextFileInfo ti;
		ti.m_hasBOM = true;
		if (utf8)
		{
			ti.m_format = TextFileFormat::UTF_8;
			out->SetTextFileInfo(ti);

			StringA mbstr;
			util::string_UTF16_to_UTF8(outText, mbstr);

			if (ti.m_hasBOM)
				out->Write(StringA("\xEF\xBB\xBF"));

			out->Write(mbstr);

		}
		else {
			ti.m_endian = TextFileEndian::Little;
			ti.m_format = TextFileFormat::UTF_16;

			if (ti.m_hasBOM)
				out->Write(StringA("\xFF\xFE"));

			out->SetTextFileInfo(ti);
			out->Write(outText);
		}
		delete out;
	}
}

