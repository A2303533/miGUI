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
#ifndef _MGF_TABLE_H_
#define _MGF_TABLE_H_

#include "Rectangle.h"

#include <string>

/*
* Например я собираю информацию о всех файлах на жёстком диске.
* Информация где-то хранится, и будет правильным использовать то же самое место и в таблице,
* то есть не создавать отдельные массивы.
* Ещё, для множества данных правильнее использовать linked list а не массив.
struct FileInfo // info for row
{
	string filename;
	u32 fileSize;
	string fileType;
	u32 date;
	MyStruct* data;
};

я храню всю информацию в списке
list<FileInfo> allFiles;
каждый FileInfo лежит отдельно от другого, не по порядку.

я хочу использовать allFiles в таблице.
Таблица работает по индексам (напр. начинать рисовать линию с такого идекса), значит всё равно необходим массив.

Например, в allFiles добавляю новую запись. Далее необходимо создать массив указателей, и каждый раз при
добавлении или удалении записи надо будет пересоздавать этот массив.
Значит, таблица будет работать с массивом указателей.

FileInfo** arrFileInfo = malloc(количество * sizeof(FileInfo*));
далее заполнение arrFileInfo

потом передаю таблице
table->rows = arrFileInfo;

При рисовании будет вызываться колбэк в который будет передан указатель на текущий FileInfo*, и 
 значение индекс - столбец. 
*/

namespace mgf
{
	enum TableFlag
	{
		TableFlag_select = 0x1,
	};

	class Table : public Element
	{
		mgElementTable_s* m_elementTable = 0;

		// size must be equal `column number`
		std::vector<int> m_colSizes;
		
		/*std::vector<mgTableRow_s*> m_rows;*/
	public:
		Table(Window* w, uint32_t colNum);
		virtual ~Table();
		virtual void SetTextProcessor(TextProcessor*) override;

		virtual void SetRect(int left, int top, int right, int bottom) override;

		void SetDrawItemBG(bool);

		void SetFont(Font*);

		// 0 - no limit;
		void SetTextInputCharLimit(uint32_t);
		
		void SetData(void** arr, uint32_t arrSz);
		void SetScrollSpeed(float);

		void** GetData();

		// return array with size == num of cols
		int* GetColSizes();

		uint32_t GetRowHeight();
		void SetRowHeight(uint32_t);

		/// <summary>
		/// Filter for textInput. Return 0 to skip this char
		/// </summary>
		//virtual wchar_t OnTextInputCharEnter(Table*, wchar_t);

		/// <summary>
		/// You can edit some information.
		/// </summary>
		/// <param name="i"> - 1 when pressed Enter, 2 when clicked somewhere, 3 whenpressed Escape</param>
		/// <param name="str"> - new text</param>
		/// <param name="editItem"> - first byte for edited item</param>
		/// <returns></returns>
		//virtual int OnTextInputEndEdit(Table*, int i, const wchar_t* str, uint8_t* editItem);

		/*
	* Return 0 - no draw
	*        1 - this is wchar_t* (you must set `text` and `textlen`)
	*
	*/

		/// <summary>
		/// To get text.
		/// Return 1 if need to draw this row.
		/// </summary>
		virtual int OnDrawRow(Table*, void* row, uint32_t col, mgUnicodeChar** text, size_t* textlen);


		/// <summary>
		/// If you want selection you must add this callbacks: 
		/// Return 1 if this row selected
		/// </summary>
		virtual int OnIsRowSelected(Table*, void* row);

		/// <summary>
		/// If you want selection you must add this callbacks: 
		/// Select here row here.
		/// mouseButton: 1 - lmb, 2 - rmb, 3 - mmb
		/// </summary>
		virtual void OnRowClick(Table*, void* row, uint32_t rowIndex, uint32_t mouseButton, mgInputContext_s* input);
		
		/// <summary>
		/// Return 1 if need to enable textinput
		/// </summary>
		virtual int OnCellClick(Table*, void* row, uint32_t rowIndex, uint32_t colIndex, uint32_t mouseButton, mgInputContext_s* input);
		
		/// <summary>
		/// When OnCellClick return 1, will be activated textInput, and need to put some text in it.
		/// Return text that will be placed in textInput.
		/// </summary>
		virtual const mgUnicodeChar* OnCellTextInputActivate(Table*, void* row, uint32_t rowIndex, uint32_t colIndex);
		
		/// <summary>
		/// Return that character that you want to put in textInput. return 0 for skip.
		/// </summary>
		virtual mgUnicodeChar OnCellTextInputCharEnter(Table*, mgUnicodeChar c);
		
		/// <summary>
		/// Will be called when Escape or Enter or click somewhere (type: see onEndEdit in mgElementTextInput.h)
		/// Return 1 if need to deactivate input
		/// </summary>
		virtual int OnCellTextInputEndEdit(Table*, int type, const mgUnicodeChar* textinputText, void* row, uint32_t rowIndex, uint32_t colIndex);

		/// <summary>
		/// Set this for column title, that can change column width.
		/// Return text for current column, set also textLen.
		/// </summary>
		virtual const mgUnicodeChar* OnColTitleText(Table*, size_t* textLen, uint32_t colIndex);

		/// <summary>
		/// 
		/// </summary>
		virtual void OnColTitleClick(Table*, uint32_t colIndex, uint32_t mouseButton);


		void SetActiveColTitle(uint32_t col);
		void SetColTitleHeight(uint32_t h);

		virtual int OnGetUserElementNum(Table*, void* row, uint32_t rowIndex, uint32_t colIndex);
		virtual Element* OnGetUserElement(Table*, uint32_t index, void* row, uint32_t rowIndex, uint32_t colIndex);
		virtual void OnBeginGetUserElement(Table*);
	};
}

#endif