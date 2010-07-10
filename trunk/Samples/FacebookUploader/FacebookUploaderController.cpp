#include "FacebookUploaderController.h"
#include "ItemView.h"
#include "XULWin/Component.h"
#include "XULWin/Components.h"
#include "XULWin/Toolbar.h"
#include "XULWin/Decorator.h"
#include "XULWin/ErrorReporter.h"
#include "XULWin/Unicode.h"
#include "XULWin/Window.h"
#include "XULWin/XMLWindow.h"
#include "XULWin/WindowsOpenFileDialog.h"
#include "XULWin/WinUtils.h"
#include "XULWin/XULRunner.h"
#include <algorithm>
#include <boost/bind.hpp>


namespace XULWin
{

    
    FacebookUploaderController::FacebookUploaderController(HINSTANCE inInstanceHandle, const std::string & inAppDir) :
        mModel(new FacebookUploaderModel),
        mAppDir(inAppDir),
        mCurrentDirectoryChanger(mAppDir),
        mXULRunner(inInstanceHandle),
        mLogTextBox(0)
    {
    }


    void FacebookUploaderController::runApplication()
    {
        //
        // Parse the XUL file
        //
        mXULRunner.loadApplication("application.ini");


        //
        // Check if the root element is a window
        //
        ElementPtr root = mXULRunner.rootElement();
        Window * window = root->component()->downcast<Window>();
        if (!window)
        {
            throw std::runtime_error("Root element is not a window.");
        }

        
        Element * logTextBox = mXULRunner.rootElement()->getElementById("logTextBox");
        if (logTextBox)
        {
            mLogTextBox = logTextBox->component()->downcast<TextBox>();
        }
        

        //
        // Register the events
        //
        mEvents.connect(root->getElementById("loginButton"),
                        boost::bind(&FacebookUploaderController::handleLoginButton, this, _1, _2));

        mEvents.connect(root->getElementById("logoutButton"),
                        boost::bind(&FacebookUploaderController::handleLogoutButton, this, _1, _2));

        mEvents.connect(root->getElementById("addButton"),
                        boost::bind(&FacebookUploaderController::handleAddButton, this, _1, _2));

        mEvents.connect(root->getElementById("removeButton"),
                        boost::bind(&FacebookUploaderController::handleRemoveButton, this, _1, _2));

        mEvents.connect(root->getElementById("rotateLeftButton"),
                        boost::bind(&FacebookUploaderController::handleRotateLeftButton, this, _1, _2));

        mEvents.connect(root->getElementById("rotateRightButton"),
                        boost::bind(&FacebookUploaderController::handleRotateRightButton, this, _1, _2));

        mEvents.connect(root->getElementById("uploadButton"),
                        boost::bind(&FacebookUploaderController::handleUploadButton, this, _1, _2));

        // Show the Window
        // This is blocking (modal window).
        window->showModal(WindowPos_CenterInScreen);
    }


    void FacebookUploaderController::showMessageBox(const std::string & inMessage)
    {
        std::wstring utf16Message = ToUTF16(inMessage);
        ::MessageBox(0, utf16Message.c_str(), TEXT("Facebook Uploader"), MB_OK);
    }
    
    
    void FacebookUploaderController::log(const std::string & inMessage)
    {
        if (!mLogTextBox)
        {
            return;
        }

        mLogTextBox->setValue(mLogTextBox->getValue() + inMessage + "\r\n");
    }


    LRESULT FacebookUploaderController::handleLoginButton(WPARAM wParam, LPARAM lParam)
    {
        showMessageBox("login");
        return cHandled;
    }


    LRESULT FacebookUploaderController::handleLogoutButton(WPARAM wParam, LPARAM lParam)
    {
        showMessageBox("logout");
        return cHandled;
    }


    LRESULT FacebookUploaderController::handleAddButton(WPARAM wParam, LPARAM lParam)
    {
        std::vector<std::string> selectedFiles;
        WinAPI::ChooseFile(0, "Facebook Uploader: Choose your images.", WinAPI::GetImagesFilter(), true, selectedFiles);
        addItems(selectedFiles);
        return cHandled;
    }


    LRESULT FacebookUploaderController::handleRemoveButton(WPARAM wParam, LPARAM lParam)
    {
        showMessageBox("remove");
        return cHandled;
    }


    LRESULT FacebookUploaderController::handleRotateLeftButton(WPARAM wParam, LPARAM lParam)
    {
        showMessageBox("rotate left");
        return cHandled;
    }


    LRESULT FacebookUploaderController::handleRotateRightButton(WPARAM wParam, LPARAM lParam)
    {
        showMessageBox("rotate right");
        return cHandled;
    }


    LRESULT FacebookUploaderController::handleUploadButton(WPARAM wParam, LPARAM lParam)
    {
        showMessageBox("upload");
        return cHandled;
    }


    void FacebookUploaderController::addItems(const std::vector<std::string> & inFiles)
    {
        for (size_t idx = 0; idx != inFiles.size(); ++idx)
        {
            ItemPtr item(new Item(inFiles[idx]));
            item->setUserData(new ItemView(item.get()));
            mModel->addItem(item);
            log("Added " + item->path());
        }
    }


} // namespace XULWin
