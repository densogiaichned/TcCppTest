# TcCppTest

A simple test-framework for [TwinCAT3 C/C++](https://infosys.beckhoff.com/english.php?content=../content/1033/tc3_c/index.html).

## Credits

Based on [kudaba/simpletest](https://github.com/kudaba/simpletest), kudos!

## Usage

> See [kudaba/simpletest README.md](https://github.com/kudaba/simpletest#simpletest) for basic usage.

### Include static library
Include the static library `TcCppTestLib.lib` into your project.  
See [Sample25: Static Library](https://infosys.beckhoff.com/content/1033/tc3_c/112613003.html) for more informations.

### Define tests

```c++
// Tests/ExampleTests.h
#pragma once

#include "dsian/TcCppTest.h"

DEFINE_TEST_G(BasicExampleTests, ExampleTestsGroup)
{
    TEST(1 == 1);
}
```

### Execute tests

```c++
///////////////////////////////////////////////////////////////////////////////
// Example.cpp
#include "TcPch.h"
#pragma hdrstop

#include "dsian/TcCppTest.h"
#include "Tests/ExampleTests.h"

// . . .

///////////////////////////////////////////////////////////////////////////////
// State transition from PREOP to SAFEOP
//
// Initialize input parameters 
// Allocate memory
HRESULT CExample::SetObjStatePS(PTComInitDataHdr pInitData)
{
    m_Trace.Log(tlVerbose, FENTERA);

    HRESULT hr = S_OK;
    IMPLEMENT_ITCOMOBJECT_EVALUATE_INITDATA(pInitData);

    hr = dsian::TestFixture::ExecuteAllTests(
        [&](auto msg) { m_Trace.Log(tlAlways, "%s", msg); }) ? hr : ADS_E_ABORTED;

    m_Trace.Log(tlVerbose, FLEAVEA "hr=0x%08x", hr);
    return hr;
}

// ...
```
  