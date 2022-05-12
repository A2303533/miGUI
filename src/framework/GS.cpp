#include "mgf.h"
#include "GS.h"

#ifdef MGF_GS_D3D11

using namespace mgf;

GS::GS() 
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
