///////////////////////////////////////////////////////////////////////////////
// ExampleCtrl.h

#ifndef __EXAMPLECTRL_H__
#define __EXAMPLECTRL_H__

#include <atlbase.h>
#include <atlcom.h>


#include "resource.h"       // main symbols
#include "ExampleW32.h"
#include "TcBase.h"
#include "ExampleClassFactory.h"
#include "TcOCFCtrlImpl.h"

class CExampleCtrl 
	: public CComObjectRootEx<CComMultiThreadModel>
	, public CComCoClass<CExampleCtrl, &CLSID_ExampleCtrl>
	, public IExampleCtrl
	, public ITcOCFCtrlImpl<CExampleCtrl, CExampleClassFactory>
{
public:
	CExampleCtrl();
	virtual ~CExampleCtrl();

DECLARE_REGISTRY_RESOURCEID(IDR_EXAMPLECTRL)
DECLARE_NOT_AGGREGATABLE(CExampleCtrl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CExampleCtrl)
	COM_INTERFACE_ENTRY(IExampleCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl2)
END_COM_MAP()

};

#endif // #ifndef __EXAMPLECTRL_H__
