///////////////////////////////////////////////////////////////////////////////
// ExampleDriver.cpp
#include "TcPch.h"
#pragma hdrstop

#include "ExampleDriver.h"
#include "ExampleClassFactory.h"

DECLARE_GENERIC_DEVICE(EXAMPLEDRV)

IOSTATUS CExampleDriver::OnLoad( )
{
	TRACE(_T("CObjClassFactory::OnLoad()\n") );
	m_pObjClassFactory = new CExampleClassFactory();

	return IOSTATUS_SUCCESS;
}

VOID CExampleDriver::OnUnLoad( )
{
	delete m_pObjClassFactory;
}

unsigned long _cdecl CExampleDriver::EXAMPLEDRV_GetVersion( )
{
	return( (EXAMPLEDRV_Major << 8) | EXAMPLEDRV_Minor );
}

