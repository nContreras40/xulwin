#ifndef WINDOWSLISTVIEW_H_INCLUDED
#define WINDOWSLISTVIEW_H_INCLUDED


#include "XULWin/Types.h"
#include "XULWin/Windows.h"
#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include <vector>


namespace XULWin
{

namespace WinAPI
{

    class ListItem;
    typedef boost::shared_ptr<ListItem> ListItemPtr;

    class ListView
    {
    public:
        typedef std::vector<ListItemPtr> ListItems;

        ListView (HMODULE inModuleHandle,
                  HWND inParent,
                  int inChildId);

        ~ListView();

        // Takes ownership
        void add(ListItem * inListItem);

        size_t size() const;

        bool empty() const;

        void removeByIndex(size_t inIndex);

        void clear();

        const ListItem * getByIndex(size_t inIndex) const;

        ListItem * getByIndex(size_t inIndex);


        HMODULE moduleHandle() const;

        HWND handle() const;

        void rebuildLayout();

        void buildListView();

    private:
        friend class ListViewDropDown;

        static LRESULT CALLBACK ListViewProc(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK ParentProc(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);

        typedef std::map<ListView *, HWND> Parents;
        static Parents sParents;

        HMODULE mModuleHandle;
        HWND mParentWindow;
        HWND mHandle;
        int mChildId;
        WNDPROC mParentProc;
        ListItems mListItems;
    };


    class ListItem
    {
    public:
        ListItem(ListView * inListView);

        virtual void draw(HDC inHDC, const RECT & inRect) = 0;

    private:
        ListView * mListView;
    };


    class ListItem_Text : public ListItem
    {
    public:
        ListItem_Text(ListView * inListView, const std::string & inText);

        virtual void draw(HDC inHDC, const RECT & inRect);

    private:
        std::wstring mText;
    };


} // namespace WinAPI

} // namespace XULWin


#endif // WINDOWSLISTVIEW_H_INCLUDED