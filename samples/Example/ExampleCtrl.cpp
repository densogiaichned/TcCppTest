// ExampleCtrl.cpp : Implementation of CTcExampleCtrl
#include "TcPch.h"
#pragma hdrstop

#include "ExampleW32.h"
#include "ExampleCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CExampleCtrl

CExampleCtrl::CExampleCtrl() 
	: ITcOCFCtrlImpl<CExampleCtrl, CExampleClassFactory>() 
{
}

CExampleCtrl::~CExampleCtrl()
{
}

