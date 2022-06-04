///////////////////////////////////////////////////////////////////////////////
// ExampleDriver.h

#ifndef __EXAMPLEDRIVER_H__
#define __EXAMPLEDRIVER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcBase.h"

#define EXAMPLEDRV_NAME        "EXAMPLE"
#define EXAMPLEDRV_Major       1
#define EXAMPLEDRV_Minor       0

#define DEVICE_CLASS CExampleDriver

#include "ObjDriver.h"

class CExampleDriver : public CObjDriver
{
public:
	virtual IOSTATUS	OnLoad();
	virtual VOID		OnUnLoad();

	//////////////////////////////////////////////////////
	// VxD-Services exported by this driver
	static unsigned long	_cdecl EXAMPLEDRV_GetVersion();
	//////////////////////////////////////////////////////
	
};

Begin_VxD_Service_Table(EXAMPLEDRV)
	VxD_Service( EXAMPLEDRV_GetVersion )
End_VxD_Service_Table


#endif // ifndef __EXAMPLEDRIVER_H__