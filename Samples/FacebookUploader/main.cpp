#include "XULWin/Component.h"
#include "XULWin/Toolbar.h"
#include "XULWin/Decorator.h"
#include "XULWin/ErrorReporter.h"
#include "XULWin/Initializer.h"
#include "XULWin/Unicode.h"
#include "XULWin/Window.h"
#include "XULWin/XMLWindow.h"
#include "XULWin/WindowsOpenFileDialog.h"
#include "XULWin/WinUtils.h"
#include "XULWin/XULRunner.h"
#include "Poco/File.h"
#include <sstream>
#include <string>


using namespace XULWin;


std::string unquote(const std::string & inString)
{
    if (inString.size() <= 2)
    {
        return inString;
    }

    if ((inString[0] == '"' && inString[inString.size() - 1] == '"') ||
            (inString[0] == '\'' && inString[inString.size() - 1] == '\''))
    {
        return inString.substr(1, inString.size() - 2);
    }
    return inString;
}


LRESULT ShowMessage(WPARAM, LPARAM, const std::string & inMessage)
{
    std::wstring msg = ToUTF16(inMessage);
    MessageBox(0, msg.c_str(), L"Facebook Uploader", MB_OK);
    return cHandled;
}


LRESULT OpenFileChooser(WPARAM, LPARAM)
{
    std::vector<std::string> selectedFiles;
    WinAPI::ChooseFile(0, "Facebook Uploader: Choose your images.", WinAPI::GetImagesFilter(), true, selectedFiles);
    return cHandled;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Initialize the XULWin library
    Initializer initializer(hInstance);
    ErrorCatcher errorCatcher;

    std::string xulPath = unquote(lpCmdLine);
    if (xulPath.empty())
    {
        xulPath = "facebook-uploader";
    }

    if (!Poco::File(xulPath).exists())
    {
        std::wstring msg = L"Could not find " + ToUTF16(xulPath) + L".";
        MessageBox(0, msg.c_str(), L"Facebook Uploader", MB_OK);
        return 1;
    }

    try
    {
        WinAPI::CurrentDirectoryChanger cd(xulPath);
        XULRunner xulRunner(hInstance);
        xulRunner.loadApplication("application.ini");
        ElementPtr root = xulRunner.rootElement();
        Window * window = root->component()->downcast<Window>();
        if (!window)
        {
            throw std::runtime_error("Root element is not a window.");
        }
        
        std::vector<XMLToolbarButton *> toolbarButtons;
        root->getElementsByType<XMLToolbarButton>(toolbarButtons);
        ScopedEventListener events;
        for (size_t idx = 0; idx != toolbarButtons.size(); ++idx)
        {
            XMLToolbarButton * button = toolbarButtons[idx];
            std::string id = button->getAttribute("id");
            if (id == "addButton")
            {
                events.connect(button, boost::bind(&OpenFileChooser, _1, _2));
            }
            else
            {
                events.connect(button, boost::bind(&ShowMessage, _1, _2, id));
            }
        }

        window->showModal(WindowPos_CenterInScreen);
    }
    catch (const std::exception & inException)
    {
        std::wstring msg = ToUTF16(inException.what());
        MessageBox(0, msg.c_str(), L"Facebook Uploader", MB_OK);
        return 1;
    }
    return 0;
}