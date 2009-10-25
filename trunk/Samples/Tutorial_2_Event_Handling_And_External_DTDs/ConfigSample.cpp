#include "ConfigSample.h"
#include "XULWin/MenuItemElement.h"
#include "XULWin/MenuPopupElement.h"
#include "XULWin/Unicode.h"
#include "Poco/Path.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <sstream>


namespace XULWin
{

    // Helper function for closing the window
    LRESULT onCancel(Window * inWindow, WPARAM, LPARAM)
    {
        inWindow->close();
        return 0;
    }


    ConfigSample::ConfigSample(HMODULE inModuleHandle, const std::string & inAppDir) : 
        mAppDir(inAppDir),
        mXULRunner(inModuleHandle)
    {
    }


    void ConfigSample::run()
    {
        Windows::CurrentDirectoryChanger curdir(mAppDir);
        mConfigWindow = mXULRunner.loadApplication("application.ini");
        if (!mConfigWindow)
        {
            ReportError("Failed to load application.ini file for configpanel.");
            return;
        }
        
        mSetsPopup = mConfigWindow->getElementById("setsMenuList");


        mNewSetButton = mConfigWindow->getElementById("newSetButton");


        //
        // Usage of the ScopedEventListener class is illustrated below.
        // I think the code is self-explanatory. Let me know if it isn't.
        // 


        // Connect the button click action with the showNewSetDialog method.
        mEvents.connect(mNewSetButton,
                        boost::bind(&ConfigSample::showNewSetDialog, this));

        
        // Connect the checkbox with the showMessage method.
        mEvents.connect(mConfigWindow->getElementById("allowRatingsCheckBox"),
                        boost::bind(&ConfigSample::showMessage, this, "Checked"));

        
        // Connect the key-up event in the tags textbox with a showMessage.
        mEvents.connect(mConfigWindow->getElementById("tagsTextBox"),
                        WM_KEYUP,
                        boost::bind(&ConfigSample::showMessage, this, "Received WM_KEYUP event."));

        
        // Connect the upload button with the showUpload method.
        mEvents.connect(mConfigWindow->getElementById("uploadButton"),
                        boost::bind(&ConfigSample::showUpload, this));


        if (Window * win = mConfigWindow->component()->downcast<Window>())
        {
            ::DragAcceptFiles(win->handle(), TRUE);

            // Connect the WM_DROPFILES message with the dropFiles method.
            mEvents.connect(mConfigWindow.get(),
                            WM_DROPFILES,
                            boost::bind(&ConfigSample::dropFiles, this, _1, _2));

            // Connect the cancel button with the onCancel function.
            mEvents.connect(mConfigWindow->getElementById("cancelButton"),
                            boost::bind(&onCancel, win, _1, _2));

            win->showModal(WindowElement::CenterInScreen);
        }
    }


    LRESULT ConfigSample::dropFiles(WPARAM wParam, LPARAM lParam)
    {
        std::vector<std::string> files;

        // Obtain the number of dropped files.
        int numFiles = ::DragQueryFile((HDROP)wParam, 0xFFFFFFFF, 0, 0);
        for (int idx = 0; idx < numFiles; ++idx)
        {
    	    TCHAR fileName[MAX_PATH];

            // Obtain the filename for the dropped file.
            ::DragQueryFile((HDROP)wParam, idx, &fileName[0], MAX_PATH);
            files.push_back(ToUTF8(&fileName[0]));
        }
        return 0;
    }


    LRESULT ConfigSample::showMessage(const std::string & inMessage)
    {
        std::stringstream ss;
        ss << inMessage;
        ::MessageBoxA(0, ss.str().c_str(), "Message", MB_OK);
        return 0;
    }


    LRESULT ConfigSample::showUpload()
    {
        ::MessageBox(0, TEXT("Upload initiated!"), TEXT("Config panel"), MB_OK);
        return 0;
    }


    LRESULT ConfigSample::addNewSet(const std::string & inSetName)
    { 
        AttributesMapping attr;
        attr["label"] = inSetName;
        if (!mSetsPopup->children().empty())
        {
            if (MenuPopupElement * popup = mSetsPopup->children()[0]->downcast<MenuPopupElement>())
            {
                ElementPtr item = MenuItemElement::Create(popup, attr);
                item->init();
                return 0;
            }
        }
        return 1;
    }


    LRESULT ConfigSample::showNewSetDialog()
    {
        mNewSetDlg = mXULRunner.loadXUL("chrome://configpanel/content/newsetdialog.xul");

        mNewSetTextBox = mNewSetDlg->getElementById("settextbox");
        
        mNewSetOK = mNewSetDlg->getElementById("newSetOKButton");
        ScopedEventListener localEvents;
        localEvents.connect(mNewSetOK, boost::bind(&ConfigSample::newSetOK, this));

        mNewSetCancel = mNewSetDlg->getElementById("newSetCancelButton");
        localEvents.connect(mNewSetCancel, boost::bind(&ConfigSample::closeDialog, this, mNewSetDlg.get()));

        if (Dialog * dlg = mNewSetDlg->component()->downcast<Dialog>())
        {
            if (Window * wnd = mConfigWindow->component()->downcast<Window>())
            {
                dlg->showModal(wnd);
            }
        }
        return 0;
    }


    LRESULT ConfigSample::newSetOK()
    {        
        AttributesMapping attr;
        if (TextBox * nativeTextBox = mNewSetTextBox->component()->downcast<TextBox>())
        {
            addNewSet(nativeTextBox->getValue());
        }
        if (Dialog * nativeDialog = mNewSetDlg->component()->downcast<Dialog>())
        {
            nativeDialog->endModal(XULWin::DialogResult_Ok);
            return 0;
        }
        return 1;
    }


    LRESULT ConfigSample::closeDialog(Element * inDialog)
    {
        if (Dialog * nativeDialog = inDialog->component()->downcast<Dialog>())
        {
            nativeDialog->endModal(XULWin::DialogResult_Cancel);
        }
        return 0;
    }


} // namespace XULWin
