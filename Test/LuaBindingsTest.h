#ifndef LUABINDINGSTEST_H_INCLUDED
#define LUABINDINGSTEST_H_INCLUDED


#include "XULWin/Lua/XULRunnerWithLua.h"
#include "XULWin/Windows.h"
#include <string>


namespace XULWin
{


    class LuaBindingsTest
    {
    public:
        LuaBindingsTest(HMODULE inModuleHandle, const std::string & inPathToXULRunnerSamples);

        ~LuaBindingsTest();

        void runXULSample(const std::string & inAppname);

        void log(const std::string & inMessage);

    private:
        HMODULE mModuleHandle;
        Lua::XULRunnerWithLua mLoggerRunner;
        std::string mPathToXULRunnerSamples;
        ElementPtr mLoggerApp;
    };


} // namespace XULWin


#endif // LUABINDINGSTEST_H_INCLUDED
