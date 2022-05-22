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

#include <vector>
#include <string>

#include <shlobj.h>
#include <shlwapi.h>
#include <Shellapi.h>
#pragma comment(lib, "Shlwapi.lib")

using namespace mgf;

class DNDWin32DropDataObject : public IDataObject
{
    volatile LONG   m_lRefCount;
public:
    DNDWin32DropDataObject() { m_lRefCount = 1; }
    virtual ~DNDWin32DropDataObject() { m_lRefCount = 0; }
    virtual HRESULT STDMETHODCALLTYPE GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium) override { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium) override { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC* pformatetc)  override { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC* pformatectIn, FORMATETC* pformatetcOut)  override { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease)  override { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc)  override { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC* pformatetc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection)  override { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE DUnadvise(DWORD dwConnection)  override { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA** ppenumAdvise)  override { return S_OK; }
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR* __RPC_FAR* ppv) {
        static const QITAB qit[] = { QITABENT(DNDWin32DropDataObject, IDataObject),{ 0 } };
        return QISearch(this, qit, riid, ppv);
    }
    virtual ULONG STDMETHODCALLTYPE AddRef(void) { return InterlockedIncrement(&m_lRefCount); }
    virtual ULONG STDMETHODCALLTYPE Release(void) {
        ULONG lRef = InterlockedDecrement(&m_lRefCount);
        if (0 == lRef)delete this;
        return m_lRefCount;
    }
};

class DNDWin32DropTarget : public IDropTarget{
    volatile LONG   m_lRefCount;
    DND* m_dnd = 0;
    HWND m_hWnd = 0;
    DNDCallback* m_cb = 0;
public:
    DNDWin32DropTarget(DND* dnd, DNDCallback* cb, HWND hWnd) : m_dnd(dnd), m_cb(cb), m_hWnd(hWnd) {m_lRefCount = 1;}
    virtual ~DNDWin32DropTarget() {m_lRefCount = 0;}
    virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* pDataObj, DWORD grfKeyState,POINTL pt,DWORD* pdwEffect) override{
        HRESULT ret = S_OK;
        *pdwEffect = DROPEFFECT_NONE;
        POINT ppt;
        ppt.x = pt.x;
        ppt.y = pt.y;
        ScreenToClient(m_hWnd, &ppt);
        
        DNDCallback::Nature nature;
        nature.m_type = DNDCallback::NatureType::NatureType_user;
        
        char** fileList = 0;
        int isOnDragEnterCalled = 0;

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
                   //printf("CF_HDROP\n");
                    nature.m_type = DNDCallback::NatureType::NatureType_file;
                    FORMATETC fmt2 = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
                    STGMEDIUM stgmed;
                    hr = pDataObj->GetData(&fmt2, &stgmed);
                    if (hr == S_OK)
                    {
                        HDROP hDrop = (HDROP)stgmed.hGlobal;
                        UINT num = DragQueryFileA(hDrop, 0xFFFFFFFF, 0, 0);
                        nature._f.fileListSize = (int)num;
                        std::vector<std::string> arr;
                        char buf[0xffff];
                        for (UINT i = 0; i < num; ++i)
                        {
                            DragQueryFileA(hDrop, i, buf, 0xffff);
                            buf[0xffff - 1] = 0;
                            arr.push_back(buf);
                        }
                        DragFinish(hDrop);
                        ReleaseStgMedium(&stgmed);

                        fileList = new char*[nature._f.fileListSize];
                        nature._f.fileList = fileList;
                        for (int i = 0; i < nature._f.fileListSize; ++i)
                        {
                            fileList[i] = (char*)arr[i].data();
                        }

                        isOnDragEnterCalled = 1;
                        if (!m_cb->OnDragEnter(ppt.x, ppt.y, &nature))
                        {
                            *pdwEffect = DROPEFFECT_NONE;
                        }
                    }

                    break;
                }
                else if (fmt.cfFormat == CF_TEXT)
                {
                    break;
                }
                if (hr != S_OK)
                    break;
            }
        }

        if (fileList)
            delete[] fileList;

        if (!isOnDragEnterCalled)
        {
            if (!m_cb->OnDragEnter(ppt.x, ppt.y, &nature))
            {
                *pdwEffect = DROPEFFECT_NONE;
            }
        }

        /*FORMATETC fmt;
        STGMEDIUM stg;
        if (pDataObj->GetData(&fmt, &stg))
        {
            switch (fmt.cfFormat)
            {
            case CF_HDROP:
                printf("CF_HDROP\n");
                break;
            case CF_TEXT: printf("CF_TEXT\n"); break;
            case CF_BITMAP: printf("CF_BITMAP\n"); break;
            case CF_METAFILEPICT: printf("CF_METAFILEPICT\n"); break;
            case CF_SYLK: printf("CF_SYLK\n"); break;
            case CF_DIF: printf("CF_DIF\n"); break;
            case CF_TIFF: printf("CF_TIFF\n"); break;
            case CF_OEMTEXT: printf("CF_OEMTEXT\n"); break;
            case CF_DIB: printf("CF_DIB\n"); break;
            case CF_PALETTE: printf("CF_PALETTE\n"); break;
            case CF_PENDATA: printf("CF_PENDATA\n"); break;
            case CF_RIFF: printf("CF_RIFF\n"); break;
            case CF_WAVE: printf("CF_WAVE\n"); break;
            case CF_UNICODETEXT: printf("CF_UNICODETEXT\n"); break;
            case CF_ENHMETAFILE: printf("CF_ENHMETAFILE\n"); break;
            case CF_LOCALE: printf("CF_LOCALE\n"); break;
            case CF_DIBV5: printf("CF_DIBV5\n"); break;
            default:
                printf("unknown\n");
                break;
            }
        }*/

        return ret;
    }

    virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState,POINTL pt,DWORD* pdwEffect) override{
        POINT ppt;
        ppt.x = pt.x;
        ppt.y = pt.y;
        ScreenToClient(m_hWnd, &ppt);
        *pdwEffect = DROPEFFECT_NONE;
        if(m_cb->OnDragOver(ppt.x, ppt.y))
            *pdwEffect = DROPEFFECT_MOVE;
        /*if ((ppt.x >= g_rectTgt.left) && (ppt.x <= g_rectTgt.right)){
            if ((ppt.y >= g_rectTgt.top) && (ppt.y <= g_rectTgt.bottom)){
                *pdwEffect = DROPEFFECT_MOVE;
            }
        }*/
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE DragLeave(void) override{
        return S_OK;
    }
    virtual HRESULT STDMETHODCALLTYPE Drop(
        IDataObject* pDataObj,
        DWORD grfKeyState,
        POINTL pt,
        DWORD* pdwEffect)
    {
        *pdwEffect = DROPEFFECT_NONE;
        POINT ppt;
        ppt.x = pt.x;
        ppt.y = pt.y;
        ScreenToClient(m_hWnd, &ppt);

        DNDCallback::Nature nature;
        nature.m_type = DNDCallback::NatureType::NatureType_user;

        char** fileList = 0;

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
                    //printf("CF_HDROP\n");
                    nature.m_type = DNDCallback::NatureType::NatureType_file;
                    FORMATETC fmt2 = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
                    STGMEDIUM stgmed;
                    hr = pDataObj->GetData(&fmt2, &stgmed);
                    if (hr == S_OK)
                    {
                        HDROP hDrop = (HDROP)stgmed.hGlobal;
                        UINT num = DragQueryFileA(hDrop, 0xFFFFFFFF, 0, 0);
                        nature._f.fileListSize = (int)num;
                        std::vector<std::string> arr;
                        char buf[0xffff];
                        for (UINT i = 0; i < num; ++i)
                        {
                            DragQueryFileA(hDrop, i, buf, 0xffff);
                            buf[0xffff - 1] = 0;
                            arr.push_back(buf);
                        }
                        DragFinish(hDrop);
                        ReleaseStgMedium(&stgmed);

                        fileList = new char* [nature._f.fileListSize];
                        nature._f.fileList = fileList;
                        for (int i = 0; i < nature._f.fileListSize; ++i)
                        {
                            fileList[i] = (char*)arr[i].data();
                        }

                        m_cb->OnDrop(&nature);
                    }
                    break;
                }
                else if (fmt.cfFormat == CF_TEXT)
                {
                    break;
                }
                if (hr != S_OK)
                    break;
            }
        }

        if (fileList)
            delete[] fileList;

     //   printf("DROP [%u]\n", pDataObj);
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

class DNDWin32DropSource : public IDropSource{
    volatile LONG   m_lRefCount;
public:
    DNDWin32DropSource() {m_lRefCount = 1;}
    virtual ~DNDWin32DropSource() {m_lRefCount = 0;}
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
        static const QITAB qit[] ={QITABENT(DNDWin32DropSource, IDropSource),{ 0 }};
        return QISearch(this, qit, riid, ppv);
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void){return InterlockedIncrement(&m_lRefCount);}
    virtual ULONG STDMETHODCALLTYPE Release(void){
        ULONG lRef = InterlockedDecrement(&m_lRefCount);
        if (0 == lRef)delete this;
        return m_lRefCount;
    }
};

DNDWin32::DNDWin32(DNDCallback* cb, HWND hWnd)
{
    m_cb = cb;
    m_dropTarget = new DNDWin32DropTarget(this, m_cb, hWnd);
    m_dropSource = new DNDWin32DropSource;
    m_dropDataObject = new DNDWin32DropDataObject;
    if (RegisterDragDrop(hWnd, m_dropTarget) == S_OK)
        m_isReady = true;
}

DNDWin32::~DNDWin32()
{
    if (m_dropSource)
        m_dropSource->Release();
    if (m_dropTarget)
        m_dropTarget->Release();
    if (m_dropDataObject)
        m_dropDataObject->Release();
}

void DNDWin32::DoDND()
{
    DWORD pdwEffect = 0;
    DoDragDrop(m_dropDataObject, m_dropSource, DROPEFFECT_MOVE, &pdwEffect);
    if (pdwEffect == DRAGDROP_S_DROP)
    {
        printf("DROPPPP\n");
    }
}

// ========================================================
DND::DND()
{

}

DND::~DND()
{

}

