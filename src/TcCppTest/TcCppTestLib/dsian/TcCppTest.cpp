﻿#include "../TcPch.h"
#pragma hdrstop

#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <stdarg.h>
#include <time.h>

#include "TcCppTest.h"


namespace dsian
{


    //---------------------------------------------------------------------------------
    // statics
    //---------------------------------------------------------------------------------
    void DefaultPrint(char const* string) { }

    TestFixture* TestFixture::ourFirstTest{};
    TestFixture* TestFixture::ourLastTest{};
    TestFixture* TestFixture::ourCurrentTest{};
    std::function<void(char const*)> TestFixture::Print = DefaultPrint;

    //---------------------------------------------------------------------------------
    // Standard type printers
    //---------------------------------------------------------------------------------
    TempString::TempString(const TempString& other)
    {
        if (other.myTextPointer == other.myTextBuffer)
        {
            strcpy(myTextBuffer, other.myTextBuffer);
            myTextPointer = myTextBuffer;
        }
        else
        {
            myTextPointer = other.myTextPointer;
        }
    }
    TempString TypeToString(int value)
    {
        TempString tempString;
        if (TestFixture::GetCurrentTest()->GetPrintMethod() == TestFixture::PrintMethod::PrintHexadecimal)
            snprintf(tempString.myTextBuffer, STRING_LENGTH, "0x%08X", value);
        else
            snprintf(tempString.myTextBuffer, STRING_LENGTH, "%d", value);
        return tempString;
    }
    TempString TypeToString(unsigned int value)
    {
        TempString tempString;
        if (TestFixture::GetCurrentTest()->GetPrintMethod() == TestFixture::PrintMethod::PrintHexadecimal)
            snprintf(tempString.myTextBuffer, STRING_LENGTH, "0x%08X", value);
        else
            snprintf(tempString.myTextBuffer, STRING_LENGTH, "%u", value);
        return tempString;
    }
    TempString TypeToString(long value)
    {
        TempString tempString;
        if (TestFixture::GetCurrentTest()->GetPrintMethod() == TestFixture::PrintMethod::PrintHexadecimal)
            snprintf(tempString.myTextBuffer, STRING_LENGTH, "0x%016lX", value);
        else
            snprintf(tempString.myTextBuffer, STRING_LENGTH, "%ld", value);
        return tempString;
    }
    TempString TypeToString(unsigned long value)
    {
        TempString tempString;
        if (TestFixture::GetCurrentTest()->GetPrintMethod() == TestFixture::PrintMethod::PrintHexadecimal)
            snprintf(tempString.myTextBuffer, STRING_LENGTH, "0x%016lX", value);
        else
            snprintf(tempString.myTextBuffer, STRING_LENGTH, "%lu", value);
        return tempString;
    }
    TempString TypeToString(long long value)
    {
        TempString tempString;
        if (TestFixture::GetCurrentTest()->GetPrintMethod() == TestFixture::PrintMethod::PrintHexadecimal)
            snprintf(tempString.myTextBuffer, STRING_LENGTH, "0x%016llX", value);
        else
            snprintf(tempString.myTextBuffer, STRING_LENGTH, "%lld", value);
        return tempString;
    }
    TempString TypeToString(unsigned long long value)
    {
        TempString tempString;
        if (TestFixture::GetCurrentTest()->GetPrintMethod() == TestFixture::PrintMethod::PrintHexadecimal)
            snprintf(tempString.myTextBuffer, STRING_LENGTH, "0x%016llX", value);
        else
            snprintf(tempString.myTextBuffer, STRING_LENGTH, "%llu", value);
        return tempString;
    }
    TempString TypeToString(float value)
    {
        TempString tempString;
        snprintf(tempString.myTextBuffer, STRING_LENGTH, "%.16g", value);
        return tempString;
    }
    TempString TypeToString(double value)
    {
        TempString tempString;
        snprintf(tempString.myTextBuffer, STRING_LENGTH, "%.16g", value);
        return tempString;
    }
    TempString TypeToString(bool value)
    {
        return TempString(value ? "true" : "false");
    }
    TempString TypeToString(char const* value)
    {
        return TempString(value ? value : "(nullptr)");
    }
    TempString TypeToString(void const* value)
    {
        if (value == nullptr)
            return TempString("(nullptr)");
        TempString tempString;
        snprintf(tempString.myTextBuffer, STRING_LENGTH, "0x%p", value);
        return tempString;
    }
    TempString TypeToString(void const* value, char const* extra)
    {
        if (value == nullptr)
            return TempString("(nullptr)");
        TempString tempString;
        snprintf(tempString.myTextBuffer, STRING_LENGTH, "(0x%p) %s", value, extra);
        return tempString;
    }

    //---------------------------------------------------------------------------------
    // Fixture implementation
    //---------------------------------------------------------------------------------
    TestFixture::TestFixture()
        : myNextTest(nullptr)
        , myNextError(nullptr)
        , myNumTestsChecked(0)
        , myNumErrors(0)
        , myPrintMethod(PrintMethod::PrintDefault)
    {
        // global link list registration, add in order of discovery
        if (ourFirstTest == nullptr)
        {
            ourFirstTest = this;
            ourLastTest = this;
        }
        else
        {
            ourLastTest->myNextTest = this;
            ourLastTest = this;
        }
    }
    //---------------------------------------------------------------------------------
    bool TestFixture::ExecuteTest()
    {
        myNumTestsChecked = myNumErrors = 0;
        myNextError = (TestError*)myMessageSpace;

        TestFixture* lastCurrent = ourCurrentTest;
        ourCurrentTest = this;
        Setup();
        RunTest();
        TearDown();
        ourCurrentTest = lastCurrent;

        return myNumErrors == 0;
    }
    //---------------------------------------------------------------------------------
    // Utility to print a part of a string to show where the error is and put elipse
    // where the string is truncated
    //---------------------------------------------------------------------------------
    static void locCopyStringWithElipse(char dest[STRING_EQ_PRINT_LENGTH], char const* string, size_t offset = 0)
    {
        char const* start = string + offset - STRING_EQ_PRINT_LENGTH / 2;
        if (start < string)
            start = string;

        int i = 0;
        for (; i < STRING_EQ_PRINT_LENGTH - 1 && start[i]; ++i)
        {
            if (i < 3 && start > string)
                dest[i] = '.';
            else if (start[i] == '\r' || start[i] == '\n' || start[i] == '\t')
                dest[i] = '\\'; // simply replace this with '\', we're just aiming for a general idea not an exact representation
            else
                dest[i] = start[i];
        }

        dest[i] = 0;

        if (i == STRING_EQ_PRINT_LENGTH - 1 && start[i])
        {
            dest[i - 1] = '.';
            dest[i - 2] = '.';
            dest[i - 3] = '.';
        }
    }
    //---------------------------------------------------------------------------------
    // Instead of just check for error and printing the string, try go be smart about
    // how the information is written out:
    // ... quick brown fox jumps over ...
    //                      ^
    // ... quick brown fox jamps over ...
    //---------------------------------------------------------------------------------
    bool TestFixture::TestStrings(char const* left, char const* right, char const* prefix, char const* condition)
    {
        AddTest();
        if (left == right)
        {
            return true;
        }

        char leftLine[STRING_EQ_PRINT_LENGTH]{};
        char rightLine[STRING_EQ_PRINT_LENGTH]{};
        char locationLine[STRING_EQ_PRINT_LENGTH]{};

        if (left == nullptr || right == nullptr)
        {
            locationLine[0] = '^';
            locationLine[1] = 0;
            if (left == nullptr)
            {
                strcpy(leftLine, "nullptr");
                locCopyStringWithElipse(rightLine, right);
            }
            else
            {
                locCopyStringWithElipse(leftLine, left);
                strcpy(rightLine, "nullptr");
            }
        }
        else
        {
            char const* testLeft = left;
            char const* testRight = right;

            int offset = 0;
            for (; *testLeft && *testRight; ++offset, ++testLeft, ++testRight)
            {
                if (*testLeft != *testRight)
                    break;
            }

            // reached the end of both strings, so they're the same
            if (!*testLeft && !*testRight)
                return true;

            locCopyStringWithElipse(leftLine, left, offset);
            locCopyStringWithElipse(rightLine, right, offset);

            if (offset > STRING_EQ_PRINT_LENGTH / 2)
                offset = STRING_EQ_PRINT_LENGTH / 2;

            memset(locationLine, ' ', offset);
            locationLine[offset] = '^';
            locationLine[offset + 1] = 0;
        }

        AddError();
        LogMessage(prefix, condition, leftLine, locationLine, rightLine);
        return false;
    }
    //---------------------------------------------------------------------------------
    // Write error into current error object and advance pointer if there's still enough space
    //---------------------------------------------------------------------------------
    void TestFixture::LogMessage(char const* string, ...)
    {
        uintptr_t spaceLeft = (myMessageSpace + MESSAGE_SPACE) - (char*)myNextError;

        if (spaceLeft == 0)
        {
            return;
        }

        spaceLeft -= sizeof(TestError);

        va_list args;

        va_start(args, string);

        int printedChars = vsnprintf(myNextError->message, spaceLeft, string, args);

        va_end(args);

        // if there isn't a reasonable amount of space left then just advance to end and stop printing errors
        if (printedChars < (int)(spaceLeft - sizeof(TestError) - 64))
        {
            uintptr_t nextOffset = (uintptr_t(myNextError->message) + printedChars + sizeof(TestError) * 2 - 1);
            nextOffset -= nextOffset % alignof(TestError);
            TestError* next = (TestError*)(nextOffset);
            myNextError->next = next;
        }
        else
        {
            myNextError->next = (TestError*)(myMessageSpace + MESSAGE_SPACE);
        }

        myNextError = myNextError->next;
    }
    TestFixture const* TestFixture::LinkTest(TestFixture* test)
    {
        test->myNextTest = TestFixture::ourFirstTest;
        TestFixture::ourFirstTest = test;
        return test;
    }

    //---------------------------------------------------------------------------------
    // Standard / Example runners
    //---------------------------------------------------------------------------------
    static bool locExecuteTest(TestFixture* test, TestFixture::OutputMode output)
    {
        if (output != TestFixture::OutputMode::Silent)
        {
            TestFixture::Printf("[dsian::TcCppTest]: Running [%s/%s]", test->TestGroup(), test->TestName());
        }

        if (test->ExecuteTest())
        {
            if (output != TestFixture::OutputMode::Silent)
            {
                TestFixture::Printf("[dsian::TcCppTest]: [%s/%s]: Passed %d out of %d tests.", test->TestGroup(), test->TestName(), test->NumTests(), test->NumTests());
            }
            return true;
        }

        if (output != TestFixture::OutputMode::Silent)
        {
            TestFixture::Printf("[dsian::TcCppTest]: [%s/%s]: Failed %d out of %d tests", test->TestGroup(), test->TestName(), test->NumErrors(), test->NumTests());

            for (TestError const* err = test->GetFirstError(), *e = test->GetLastError(); err != e; err = err->next)
            {
                TestFixture::Printf("[dsian::TcCppTest]: %s", err->message);
            }
        }

        return false;
    }
    void TestFixture::Printf(char const* string, ...)
    {
        char tempSpace[4096];
        va_list args;

        va_start(args, string);

        vsnprintf(tempSpace, sizeof(tempSpace), string, args);

        va_end(args);

        if (TestFixture::Print)
        {
            TestFixture::Print(tempSpace);
        }
    }
    bool TestFixture::ExecuteAllTests(char const* groupFilter, char const* nameFilter, OutputMode output, std::function<void(const char*)> printCallback)
    {
        if (printCallback) {
            TestFixture::Print = printCallback;
        }

        if (output != OutputMode::Silent)
        {
            if (groupFilter == nullptr && nameFilter == nullptr)
                Printf("[dsian::TcCppTest]: Running all tests.");
            else if (groupFilter != nullptr && nameFilter == nullptr)
                Printf("[dsian::TcCppTest]: Running all tests in groups [%s].", groupFilter);
            else if (groupFilter == nullptr && nameFilter != nullptr)
                Printf("[dsian::TcCppTest]: Running all tests named [%s].", nameFilter);
            else
                Printf("[dsian::TcCppTest]: Running all tests named [%s/%s].", groupFilter, nameFilter);
        }

        int count = 0;
        int passes = 0;
        int fails = 0;
        bool passed = true;
        for (auto i = TestFixture::GetFirstTest(); i; i = i->GetNextTest())
        {
            bool matchGroup = groupFilter == nullptr || strcmp(groupFilter, i->TestGroup()) == 0;
            bool matchName = nameFilter == nullptr || strcmp(nameFilter, i->TestName()) == 0;
            if (matchGroup && matchName)
            {
                ++count;
                passed &= locExecuteTest(i, output);
                passes += i->NumTests();
                fails += i->NumErrors();
            }
        }

        if (output != OutputMode::Silent)
        {
            if (count == 0)
                Printf("[dsian::TcCppTest]: Failed to find any tests.\n");
            else if (passed)
                Printf("[dsian::TcCppTest]: %d Tests finished. All %d assertions are passing.\n", count, passes);
            else
                Printf("[dsian::TcCppTest]: %d Tests finished, %d of %d assertions failed. Some tests are reporting errors.\n", count, fails, passes);
        }

        TestFixture::Print = nullptr;
        return passed && passes > 0;
    }

}
