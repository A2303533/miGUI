#include "mgf.h"
#include "GS.h"

#ifdef MGF_GS_D3D11

using namespace mgf;

GS::GS() 
:
m_videoDriverType(DriverType_Unknown),
m_activeWindow(0)
{
}

GS::~GS() 
{
}

GS::DriverType GS::GetVideoDriverType()
{
	return m_videoDriverType;
}

mgf::SystemWindow* GS::GetActiveWindow()
{
	return m_activeWindow;
}

#endif
