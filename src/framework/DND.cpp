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

#include "miGUI.h"

#include "framework/mgf.h"
#include "framework/DND.h"

#include <shlobj.h>
#include <shlwapi.h>
#include <Shellapi.h>
#pragma comment(lib, "Shlwapi.lib")

using namespace mgf;

class DNDWin32DropTarget : public IDropTarget{
    volatile LONG   m_lRefCount;
public:
    DNDWin32DropTarget() {m_lRefCount = 1;}
    virtual ~DNDWin32DropTarget() {m_lRefCount = 0;}
    virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* pDataObj,DWORD grfKeyState,
        POINTL pt,DWORD* pdwEffect) override{
        *pdwEffect = DROPEFFECT_MOVE;
        /*POINT ppt;
        ppt.x = pt.x;
        ppt.y = pt.y;
        ScreenToClient(g_hWnd, &ppt);

        if ((ppt.x >= g_rectTgt.left) && (ppt.x <= g_rectTgt.right))
        {
            if ((ppt.y >= g_rectTgt.top) && (ppt.y <= g_rectTgt.bottom))
            {
                *pdwEffect = DROPEFFECT_NONE;
            }
        }*/
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState,POINTL pt,DWORD* pdwEffect) override{
        POINT ppt;
        ppt.x = pt.x;
        ppt.y = pt.y;
       // ScreenToClient(g_hWnd, &ppt);

        *pdwEffect = DROPEFFECT_NONE;
        /*if ((ppt.x >= g_rectTgt.left) && (ppt.x <= g_rectTgt.right))
        {
            if ((ppt.y >= g_rectTgt.top) && (ppt.y <= g_rectTgt.bottom))
            {
                *pdwEffect = DROPEFFECT_MOVE;
            }
        }*/
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE DragLeave(void) override{return S_OK;}
    virtual HRESULT STDMETHODCALLTYPE Drop(
        IDataObject* pDataObj,
        DWORD grfKeyState,
        POINTL pt,
        DWORD* pdwEffect)
    {
        *pdwEffect = DROPEFFECT_MOVE;

        POINT ppt;
        ppt.x = pt.x;
        ppt.y = pt.y;
        //ScreenToClient(g_hWnd, &ppt);

       /* if ((ppt.x >= g_rectTgt.left) && (ppt.x <= g_rectTgt.right))
        {
            if ((ppt.y >= g_rectTgt.top) && (ppt.y <= g_rectTgt.bottom))
            {
                printf("DROP [%u]\n", pDataObj);

                IEnumFORMATETC* ief = 0;
                pDataObj->EnumFormatEtc(DATADIR_GET, &ief);
                if (ief)
                {
                    FORMATETC fmt;
                    while (true)
                    {
                        HRESULT hr = ief->Next(1, &fmt, 0);
                        if (fmt.cfFormat == CF_HDROP)
                        {
                            FORMATETC fmt2 = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
                            STGMEDIUM stgmed;
                            if (pDataObj->GetData(&fmt2, &stgmed) == S_OK)
                            {
                                HDROP hDrop = (HDROP)stgmed.hGlobal;
                                UINT num = DragQueryFileW(hDrop, 0xFFFFFFFF, 0, 0);
                                printf("%u FILES\n", num);

                                wchar_t* buf = new wchar_t[0xffff];
                                for (UINT i = 0; i < num; ++i)
                                {
                                    DragQueryFileW(hDrop, i, buf, 0xffff);
                                    buf[0xffff - 1] = 0;
                                    wprintf(L"FILE: %s\n", buf);
                                }
                                delete[] buf;

                                DragFinish(hDrop);
                                ReleaseStgMedium(&stgmed);
                            }
                        }

                        if (hr != S_OK)
                            break;
                    }
                }
            }
        }*/
        return S_OK;
    }
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void __RPC_FAR* __RPC_FAR* ppv){
        static const QITAB qit[] ={QITABENT(DNDWin32DropTarget, IDropTarget),{ 0 }};
        return QISearch(this, qit, riid, ppv);
    }
    virtual ULONG STDMETHODCALLTYPE AddRef(void){return InterlockedIncrement(&m_lRefCount);}
    virtual ULONG STDMETHODCALLTYPE Release(void){
        ULONG lRef = InterlockedDecrement(&m_lRefCount);
        if (0 == lRef) delete this;
        return m_lRefCount;
    }
};

class DNDDropSource : public IDropSource{
    volatile LONG   m_lRefCount;
public:
    DNDDropSource() {m_lRefCount = 1;}
    virtual ~DNDDropSource() {m_lRefCount = 0;}
    virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL fEscapePressed,DWORD grfKeyState) override{
        HRESULT res = S_OK;
        if (fEscapePressed == TRUE) res = DRAGDROP_S_CANCEL;
        else
        {
            if (0 == (grfKeyState & (MK_LBUTTON))) res = DRAGDROP_S_DROP;
        }
        return res;
    }
    virtual HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD dwEffect) override{
        HRESULT res = S_OK;
        //if(dwEffect & DROPEFFECT_NONE)
        return DRAGDROP_S_USEDEFAULTCURSORS;
    }
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void __RPC_FAR* __RPC_FAR* ppv){
        static const QITAB qit[] ={QITABENT(DNDDropSource, IDropSource),{ 0 }};
        return QISearch(this, qit, riid, ppv);
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void){return InterlockedIncrement(&m_lRefCount);}
    virtual ULONG STDMETHODCALLTYPE Release(void){
        ULONG lRef = InterlockedDecrement(&m_lRefCount);
        if (0 == lRef)delete this;
        return m_lRefCount;
    }
};

DNDWin32::DNDWin32(HWND hWnd)
{
    m_dropTarget = new DNDWin32DropTarget;
    if (RegisterDragDrop(hWnd, m_dropTarget) == S_OK)
        m_isReady = true;
}

DNDWin32::~DNDWin32()
{
    if (m_dropTarget)
        delete m_dropTarget;
}

// ========================================================
DND::DND()
{

}

DND::~DND()
{

}

