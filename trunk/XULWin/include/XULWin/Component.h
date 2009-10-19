#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED


#include "XULWin/AttributeController.h"
#include "XULWin/Conversions.h"
#include "XULWin/BoxLayouter.h"
#include "XULWin/Element.h"
#include "XULWin/EventListener.h"
#include "XULWin/Fallible.h"
#include "XULWin/GdiplusLoader.h"
#include "XULWin/Layout.h"
#include "XULWin/StyleController.h"
#include "XULWin/Toolbar.h"
#include "XULWin/ToolbarItem.h"
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include <windows.h>
#include <CommCtrl.h>


namespace XULWin
{

    namespace Windows
    {
        class ConcreteToolbarItem;
        class PopupMenu;
    }

    class CommandId
    {
    public:
        CommandId() : mId(sId++) {}

        CommandId(int inId) : mId(inId) {}

        int intValue() const { return mId; }

    private:
        int mId;
        static int sId;
    };


    class Element;
    class Component;
    class Decorator;
    class BoxLayouter;
    class NativeComponent;
    typedef boost::shared_ptr<Component> ElementImplPtr;


    /**
     * Listener for XUL notifications
     */
    class NotificationListener
    {
    public:
        // This notificion is sent just after the child has been added.
        virtual void onChildAdded(Component * inChild) {}

        // This notificion is sent after the child has been removed and
        // just before the child is destroyed.
        virtual void onChildRemoved(Component * inChild) {}

        virtual void onContentChanged() {}
    };


    /**
     * Component is base class for all native UI elements.
     */
    class Component : public NotificationListener,
                        public virtual AlignController,
                        public virtual WidthController,
                        public virtual HeightController,
                        public virtual ScreenXController,
                        public virtual ScreenYController,
                        public virtual FillController,
                        public virtual StrokeController,
                        public virtual StrokeWidthController,
                        public virtual CSSFillController,
                        public virtual CSSHeightController,
                        public virtual CSSMarginController,
                        public virtual CSSStrokeController,
                        public virtual CSSWidthController,
                        public virtual CSSXController,
                        public virtual CSSYController,
                        public virtual FlexController,
                        public virtual HiddenController,
                        public virtual OrientController,
                        private boost::noncopyable
    {
    public:
        virtual ~Component() {}        

        virtual bool initComponent() = 0;

        // Returns this element's index in its parent's children collection.
        virtual int getIndex() const = 0;

        virtual size_t getChildCount() const = 0;

        virtual const Component * getChild(size_t inIndex) const = 0;

        virtual Component * getChild(size_t inIndex) = 0;

        virtual HWND getFirstParentHandle() = 0;


        // WidthController methods
        // Returns value from last call to setWidth. If setWidth has not yet
        // been called, then this method returns the value as defined in XUL
        // document. If the value in the XUL document has not been defined,
        // then the optimal size of the element is returned.
        virtual int getWidth() const = 0;

        virtual void setWidth(int inWidth) = 0;

        // HeightController methods
        virtual int getHeight() const = 0;

        virtual void setHeight(int inHeight) = 0;

        // ScreenXController methods
        virtual int getScreenX() const = 0;

        virtual void setScreenX(int inX) = 0;

        // ScreenYController methods
        virtual int getScreenY() const = 0;

        virtual void setScreenY(int inY) = 0;

        // StrokeController methods
        virtual void setStroke(const RGBColor & inColor) = 0;

        virtual const RGBColor & getStroke() const = 0;

        // StrokeWidthController methods
        virtual void setStrokeWidth(int inStrokeWidth) = 0;

        virtual int getStrokeWidth() const = 0;

        // FillController methods
        virtual void setFill(const RGBColor & inColor) = 0;

        virtual const RGBColor & getFill() const = 0;

        // FlexController methods
        virtual int getFlex() const = 0;

        virtual void setFlex(int inFlex) = 0;

        // HiddenController methods
        virtual bool isHidden() const = 0;

        virtual void setHidden(bool inHidden) = 0;

        // OrientController methods
        virtual Orient getOrient() const = 0;
        
        virtual void setOrient(Orient inOrient) = 0;

        // AlignController methods
        virtual Align getAlign() const = 0;

        virtual void setAlign(Align inAlign) = 0;

        // CSSWidthController methods
        virtual int getCSSWidth() const = 0;

        virtual void setCSSWidth(int inWidth) = 0;

        // CSSHeightController methods
        virtual int getCSSHeight() const = 0;

        virtual void setCSSHeight(int inHeight) = 0;

        // CSSMarginController methods
        virtual void getCSSMargin(int & outTop, int & outLeft, int & outRight, int & outBottom) const = 0;

        virtual void setCSSMargin(int inTop, int inLeft, int inRight, int inBottom) = 0;

        // CSSFillController methods
        virtual void setCSSFill(const RGBColor & inColor) = 0;

        virtual const RGBColor & getCSSFill() const = 0;

        // CSSStrokeController methods
        virtual void setCSSStroke(const RGBColor & inColor) = 0;

        virtual const RGBColor & getCSSStroke() const = 0;

        // CSSXController methods
        virtual int getCSSX() const = 0;

        virtual void setCSSX(int inX) = 0;

        // CSSYController methods
        virtual int getCSSY() const = 0;

        virtual void setCSSY(int inY) = 0;

        // Downcast that also resolves decorators.
        // Don't use dynamic_cast, because that would almost always fail since
        // Components are usually wrapped in Decorator objects.
        template<class Type>
        Type * downcast()
        {
            return const_cast<Type*>(static_cast<const Component*>(this)->downcast<Type>());
        }

        // Downcast that also resolves decorators.
        // Don't use dynamic_cast, because that would almost always fail since
        // Components are usually wrapped in Decorator objects.
        template<class Type>
        const Type * downcast() const
        {
            if (const Type * obj = dynamic_cast<const Type*>(this))
            {
                return obj;
            }
            else if (const Decorator * obj = dynamic_cast<const Decorator*>(this))
            {
                return obj->decoratedElement()->downcast<Type>();
            }
            return 0;
        }


        template<class Type>
        Type * findParentOfType()
        {
            if (Type * obj = dynamic_cast<Type*>(this))
            {
                return obj;
            }
            else if (const Decorator * obj = dynamic_cast<const Decorator*>(this))
            {
                return obj->decoratedElement()->findParentOfType<Type>();
            }
            else if (el() && el()->parent() && el()->parent()->component())
            {
                return el()->parent()->component()->findParentOfType<Type>();
            }
            return 0;
        }


        // Searches for a child of given type.
        // Returns the first one found.
        // Only searches one level deep.
        template<class Type>
        Type * findChildOfType()
        {
            return const_cast<Type*>(static_cast<const Component*>(this)->findChildOfType<Type>());
        }


        // Searches for a child of given type.
        // Returns the first one found.
        // Only searches one level deep.
        template<class Type>
        const Type * findChildOfType() const
        {
            if (!el())
            {
                return 0;
            }

            for (size_t idx = 0; idx != getChildCount(); ++idx)
            {
                const Component * child = getChild(idx);
                if (const Type * found = child->downcast<Type>())
                {
                    return found;
                }
            }
            return 0;
        }

        virtual int commandId() const = 0;

        virtual int getWidth(SizeConstraint inSizeConstraint) const = 0;

        virtual int getHeight(SizeConstraint inSizeConstraint) const = 0;

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const = 0;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const = 0;

        // Tendency to expand, used for separators, scrollbars, etc..
        virtual bool expansive() const = 0;

        virtual void move(int x, int y, int w, int h) = 0;

        virtual void move(const Rect & inRect) = 0;

        virtual Rect clientRect() const = 0;

        virtual void setOwningElement(Element * inElement) = 0;

        virtual Element * el() const = 0;

        virtual Component * parent() const = 0;

        virtual void rebuildLayout() = 0;

        virtual void rebuildChildLayouts() = 0;

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam) = 0;

        virtual bool getAttribute(const std::string & inName, std::string & outValue) = 0;

        virtual bool getStyle(const std::string & inName, std::string & outValue) = 0;

        virtual bool setStyle(const std::string & inName, const std::string & inValue) = 0;

        virtual bool setAttribute(const std::string & inName, const std::string & inValue) = 0;

        virtual bool initAttributeControllers() = 0;

        virtual bool initStyleControllers() = 0;

        virtual void setAttributeController(const std::string & inAttr, AttributeController * inController) = 0;

        virtual void setStyleController(const std::string & inAttr, StyleController * inController) = 0;
    };


    /**
     * ConcreteComponent is the base class for non-decorator components.
     */
    class ConcreteComponent : public Component
    {
    public:
        ConcreteComponent(Component * inParent);

        virtual ~ConcreteComponent() = 0;

        virtual bool initComponent();

        virtual int getIndex() const;

        virtual size_t getChildCount() const;

        virtual const Component * getChild(size_t inIndex) const;

        virtual Component * getChild(size_t inIndex);

        virtual HWND getFirstParentHandle();

        // WidthController methods
        // Returns value from last call to setWidth. If setWidth has not yet
        // been called, then this method returns the value as defined in XUL
        // document. If the value in the XUL document has not been defined,
        // then the optimal size of the element is returned.
        virtual int getWidth() const;

        virtual void setWidth(int inWidth);

        // HeightController methods
        virtual int getHeight() const;

        virtual void setHeight(int inHeight);

        // ScreenXController methods
        virtual int getScreenX() const;

        virtual void setScreenX(int inX);

        // ScreenYController methods
        virtual int getScreenY() const;

        virtual void setScreenY(int inY);

        // StrokeController methods
        virtual void setStroke(const RGBColor & inColor);

        virtual const RGBColor & getStroke() const;

        // StrokeWidthController methods
        virtual void setStrokeWidth(int inStrokeWidth);

        virtual int getStrokeWidth() const;

        // FillController methods
        virtual void setFill(const RGBColor & inColor);

        virtual const RGBColor & getFill() const;

        // FlexController methods
        virtual int getFlex() const;

        virtual void setFlex(int inFlex);

        // HiddenController methods
        virtual bool isHidden() const;

        virtual void setHidden(bool inHidden);

        // OrientController methods
        virtual Orient getOrient() const;
        
        virtual void setOrient(Orient inOrient);

        // AlignController methods
        virtual Align getAlign() const;

        virtual void setAlign(Align inAlign);

        // CSSWidthController methods
        virtual int getCSSWidth() const;

        virtual void setCSSWidth(int inWidth);

        // CSSHeightController methods
        virtual int getCSSHeight() const;

        virtual void setCSSHeight(int inHeight);

        // CSSMarginController methods
        virtual void getCSSMargin(int & outTop, int & outLeft, int & outRight, int & outBottom) const;

        virtual void setCSSMargin(int inTop, int inLeft, int inRight, int inBottom);

        // CSSFillController methods
        virtual void setCSSFill(const RGBColor & inColor);

        virtual const RGBColor & getCSSFill() const;

        // CSSStrokeController methods
        virtual void setCSSStroke(const RGBColor & inColor);

        virtual const RGBColor & getCSSStroke() const;

        // CSSXController methods
        virtual int getCSSX() const;

        virtual void setCSSX(int inX);

        // CSSYController methods
        virtual int getCSSY() const;

        virtual void setCSSY(int inY);

        // Downcast that also resolves decorators.
        // Use this instead of manual cast, because
        // you may get a decorator instead of the 
        // actual element.
        template<class Type>
        Type * downcast()
        {
            if (Type * obj = dynamic_cast<Type*>(this))
            {
                return obj;
            }
            else if (Decorator * obj = dynamic_cast<Decorator*>(this))
            {
                return obj->decoratedElement()->downcast<Type>();
            }
            return 0;
        }


        template<class ConstType>
        const ConstType * downcast() const
        {
            if (const ConstType * obj = dynamic_cast<const ConstType*>(this))
            {
                return obj;
            }
            else if (const Decorator * obj = dynamic_cast<const Decorator*>(this))
            {
                return obj->decoratedElement()->downcast<ConstType>();
            }
            return 0;
        }

        int commandId() const { return mCommandId.intValue(); }

        virtual int getWidth(SizeConstraint inSizeConstraint) const;

        virtual int getHeight(SizeConstraint inSizeConstraint) const;

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const = 0;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const = 0;

        // Tendency to expand, used for separators, scrollbars, etc..
        bool expansive() const;

        virtual void move(int x, int y, int w, int h) = 0;

        virtual void move(const Rect & inRect);

        virtual Rect clientRect() const = 0;

        virtual void setOwningElement(Element * inElement);

        virtual Element * el() const;

        Component * parent() const;

        virtual void rebuildLayout() = 0;

        void rebuildChildLayouts();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam) = 0;

        virtual bool getAttribute(const std::string & inName, std::string & outValue);

        virtual bool getStyle(const std::string & inName, std::string & outValue);

        virtual bool setStyle(const std::string & inName, const std::string & inValue);

        virtual bool setAttribute(const std::string & inName, const std::string & inValue);

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        void setAttributeController(const std::string & inAttr, AttributeController * inController);

        void setStyleController(const std::string & inAttr, StyleController * inController);

    protected:
        Component * mParent;
        Element * mElement;
        CommandId mCommandId;
        bool mExpansive;
        int mFlex;

        Fallible<int> mScreenX;
        Fallible<int> mScreenY; 
        Fallible<int> mCSSX;
        Fallible<int> mCSSY;
        Fallible<int> mWidth;
        Fallible<int> mHeight;       
        Fallible<int> mCSSWidth;
        Fallible<int> mCSSHeight;
        Fallible<RGBColor> mFill;
        Fallible<RGBColor> mStroke;
        Fallible<int> mStrokeWidth;
        Fallible<RGBColor> mCSSFill;
        Fallible<RGBColor> mCSSStroke;
        Fallible<Orient> mOrient;
        Fallible<Align> mAlign;
        bool mIsInitialized;

        // We need to remember the hidden state ourselves because we can't
        // rely on WinAPI IsWindowVisible call, because it will return false
        // for child windows of which the parent is not visible.
        // This is a problem because calculation of minimum size depends on
        // visibility of child items. Hidden elements return 0 when asked for
        // their min width or height. This problem becomes apparent when
        // calling NativeWindow's showModal method which calculates its minimum
        // height *before* the window becomes visible. This would result in a
        // 'minified' window state.
        bool mHidden;
        
        typedef std::map<std::string, AttributeController *> AttributeControllers;
        AttributeControllers mAttributeControllers;

        typedef std::map<std::string, StyleController *> StyleControllers;
        StyleControllers mStyleControllers;
    };


    class NativeComponent : public ConcreteComponent,
                            public virtual DisabledController,
                            public virtual LabelController
    {
    public:
        typedef ConcreteComponent Super;

        NativeComponent(Component * inParent, const AttributesMapping & inAttributes);

        virtual ~NativeComponent();             

        virtual void setHandle(HWND inHandle, bool inPassOwnership);

        bool addEventListener(EventListener * inEventListener);

        bool removeEventListener(EventListener * inEventListener);

        // DisabledController methods
        virtual bool isDisabled() const;

        virtual void setDisabled(bool inDisabled);

        // LabelController methods
        virtual std::string getLabel() const;

        virtual void setLabel(const std::string & inLabel);

        virtual void setHidden(bool inHidden);

        static void SetModuleHandle(HMODULE inModule);

        virtual HWND handle() const;

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual void handleCommand(WPARAM wParam, LPARAM lParam);

        virtual void handleMenuCommand(WORD inMenuId);

        virtual void handleDialogCommand(WORD inNotificationCode, WPARAM wParam, LPARAM lParam);

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

#ifndef SWIG
        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);
#endif

    protected:
        static NativeComponent * FindComponentByHandle(HWND inHandle);

        static NativeComponent * FindComponentById(int inId);

        void registerHandle();
        
        void subclass();

        void unsubclass();

        void unregisterHandle();

        HWND mHandle;
        HMODULE mModuleHandle;


        typedef std::set<EventListener*> EventListeners;
        EventListeners mEventListeners;

    private:
        typedef std::map<int, NativeComponent*> ComponentsById;
        static ComponentsById sComponentsById;

        typedef std::map<HWND, NativeComponent*> ComponentsByHandle;
        static ComponentsByHandle sComponentsByHandle;
        
        WNDPROC mOrigProc;

        static HMODULE sModuleHandle;
        bool mOwnsHandle;
    };


    class NativeDialog;
    class MenuImpl;

    class NativeWindow : public NativeComponent,
                         public BoxLayouter::ContentProvider,
                         public virtual TitleController
    {
    public:
        typedef NativeComponent Super;

        static void Register(HMODULE inModuleHandle);

        NativeWindow(const AttributesMapping & inAttributesMapping);

        virtual ~NativeWindow();

        virtual bool initComponent();

        // BoxLayouter
        virtual Orient getOrient() const;

        // BoxLayouter
        virtual Align getAlign() const;

        // TitleController methods
        virtual std::string getTitle() const;

        virtual void setTitle(const std::string & inTitle);

        virtual const Component * getChild(size_t idx) const;

        virtual Component * getChild(size_t idx);

        virtual void rebuildChildLayouts()
        { return Super::rebuildChildLayouts(); }

        virtual void setAttributeController(const std::string & inAttr, AttributeController * inController)
        { return Super::setAttributeController(inAttr, inController); }

        void show(Window::Positioning inPositioning);

        void showModal(Window::Positioning inPositioning);

        void close();

        virtual void move(int x, int y, int w, int h);

        virtual void rebuildLayout();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual Rect clientRect() const;

        virtual Rect windowRect() const;

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

        virtual Orient BoxLayouter_getOrient() const
        { return getOrient(); }

        virtual Align BoxLayouter_getAlign() const
        { return getAlign(); }

        virtual size_t BoxLayouter_getChildCount() const
        { return getChildCount(); }

        virtual const Component * BoxLayouter_getChild(size_t idx) const
        { return getChild(idx); }

        virtual Component * BoxLayouter_getChild(size_t idx)
        { return getChild(idx); }

        virtual Rect BoxLayouter_clientRect() const
        { return clientRect(); }

        virtual void BoxLayouter_rebuildChildLayouts()
        { rebuildChildLayouts(); }

#ifndef SWIG
        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);
#endif

    private:
        void showMenu(MenuImpl * inMenu);

        size_t getPositionInMenuBar(MenuImpl * inMenu);

        friend class NativeDialog;
        void setBlockingDialog(NativeDialog * inDlg);
        NativeDialog * mActiveDialog;
        BoxLayouter mBoxLayouter;
        HMENU mMenuHandle;
        MenuImpl * mActiveMenu;
        bool mHasMessageLoop;
    };


    // NativeDialog is actually a normal Window with some customizations to make it behave like a dialog.
    class NativeDialog : public NativeComponent,
                         public BoxLayouter::ContentProvider,
                         public virtual TitleController
    {
    public:
        typedef NativeComponent Super;

        static void Register(HMODULE inModuleHandle);

        NativeDialog(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual ~NativeDialog();

        // BoxLayouter
        virtual Orient getOrient() const;

        // BoxLayouter
        virtual Align getAlign() const;


        // TitleController methods
        virtual std::string getTitle() const;

        virtual void setTitle(const std::string & inTitle);

        virtual const Component * getChild(size_t idx) const;

        virtual Component * getChild(size_t idx);

        virtual void rebuildChildLayouts()
        { return Super::rebuildChildLayouts(); }

        virtual void setAttributeController(const std::string & inAttr, AttributeController * inController)
        { return Super::setAttributeController(inAttr, inController); }

        DialogResult showModal(NativeWindow * inInvoker);

        LRESULT endModal(DialogResult inDialogResult);

        virtual void move(int x, int y, int w, int h);

        virtual void rebuildLayout();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual Rect clientRect() const;

        virtual Rect windowRect() const;

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual Orient BoxLayouter_getOrient() const
        { return getOrient(); }

        virtual Align BoxLayouter_getAlign() const
        { return getAlign(); }

        virtual size_t BoxLayouter_getChildCount() const
        { return getChildCount(); }

        virtual const Component * BoxLayouter_getChild(size_t idx) const
        { return getChild(idx); }

        virtual Component * BoxLayouter_getChild(size_t idx)
        { return getChild(idx); }

        virtual Rect BoxLayouter_clientRect() const
        { return clientRect(); }

        virtual void BoxLayouter_rebuildChildLayouts()
        { rebuildChildLayouts(); }

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

#ifndef SWIG
        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);
#endif
    private:
        // Invoker is the stored parameter for showModal.
        NativeWindow * mInvoker;
        BoxLayouter mBoxLayouter;
        DialogResult mDialogResult;
    };


    class NativeControl : public NativeComponent
    {
    public:
        typedef NativeComponent Super;

        NativeControl(Component * inParent, const AttributesMapping & inAttributesMapping, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle);

        // use this constructor if you want to provide your own handle later using NativeControl::setHandle
        NativeControl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual ~NativeControl();
        
        bool initStyleControllers();

        virtual void rebuildLayout();

        virtual Rect clientRect() const;

        virtual void move(int x, int y, int w, int h);

        // Gets a NativeComponent object from this object. This
        // is only needed in constructors of NativeComponents, because
        // they need to know which is their native parent handle object.
        // If this is a NativeComponent, return this.
        // If this is a VirtualComponent, return first parent that is a NativeComponent.
        // If this is a Decorator, resolve until a NativeComponent is found.
        static NativeComponent * GetNativeThisOrParent(Component * inElementImpl);
        static const NativeComponent * GetNativeThisOrParent(const Component * inElementImpl);
    };


    class VirtualComponent : public ConcreteComponent
    {
    public:
        typedef ConcreteComponent Super;

        VirtualComponent(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual ~VirtualComponent();

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const = 0;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const = 0;

        virtual void rebuildLayout();

        virtual Rect clientRect() const;

        virtual void move(int x, int y, int w, int h);

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    protected:
        Rect mRect;
    };


    class PassiveComponent : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        PassiveComponent(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual ~PassiveComponent();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const { return 0; }

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const { return 0; }
    };


    class NativeButton : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeButton(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeLabel : public NativeControl,
                        public virtual StringValueController,
                        public virtual CSSTextAlignController
    {
    public:
        typedef NativeControl Super;

        NativeLabel(Component * inParent, const AttributesMapping & inAttributesMapping);

        // StringValueController methods
        virtual std::string getValue() const;

        virtual void setValue(const std::string & inStringValue);

        // CSSTextAlignController methods
        virtual CSSTextAlign getCSSTextAlign() const;

        virtual void setCSSTextAlign(CSSTextAlign inValue);

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeDescription : public NativeControl,
                              public virtual StringValueController
    {
    public:
        typedef NativeControl Super;

        NativeDescription(Component * inParent, const AttributesMapping & inAttributesMapping);

        // StringValueController methods
        virtual std::string getValue() const;

        virtual void setValue(const std::string & inStringValue);

        virtual bool initAttributeControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeTextBox : public NativeControl,
                          public virtual StringValueController,
                          public virtual ReadOnlyController,
                          public virtual RowsController
    {
    public:
        typedef NativeControl Super;

        NativeTextBox(Component * inParent, const AttributesMapping & inAttributesMapping);

        // StringValueController methods
        virtual std::string getValue() const;

        virtual void setValue(const std::string & inStringValue);

        // ReadOnlyController methods
        virtual bool isReadOnly() const;

        virtual void setReadOnly(bool inReadOnly);

        // RowsControll methods
        virtual int getRows() const;

        virtual void setRows(int inRows);

        virtual bool initAttributeControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual void handleCommand(WPARAM wParam, LPARAM lParam);

    private:
        bool mReadonly;
        int mRows;
        static DWORD GetFlags(const AttributesMapping & inAttributesMapping);
        static bool IsReadOnly(const AttributesMapping & inAttributesMapping);
    };


    class NativeCheckBox : public NativeControl,
                           public virtual CheckedController
    {
    public:
        typedef NativeControl Super;

        NativeCheckBox(Component * inParent, const AttributesMapping & inAttributesMapping);

        // CheckedController methods
        virtual bool isChecked() const;

        virtual void setChecked(bool inChecked);

        virtual bool initAttributeControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class VirtualBox : public VirtualComponent,
                       public BoxLayouter::ContentProvider
    {
    public:
        typedef VirtualComponent Super;

        VirtualBox(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual Orient getOrient() const;

        virtual Align getAlign() const;

        virtual bool initAttributeControllers();

        virtual void rebuildLayout();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const
        { return mBoxLayouter.calculateWidth(inSizeConstraint); }

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const
        { return mBoxLayouter.calculateHeight(inSizeConstraint); }

        virtual Rect clientRect() const
        { return Super::clientRect(); }

        virtual void rebuildChildLayouts()
        { return Super::rebuildChildLayouts(); }        

        virtual void setAttributeController(const std::string & inAttr, AttributeController * inController);

        virtual Orient BoxLayouter_getOrient() const
        { return getOrient(); }

        virtual Align BoxLayouter_getAlign() const
        { return getAlign(); }

        virtual size_t BoxLayouter_getChildCount() const
        { return getChildCount(); }

        virtual const Component * BoxLayouter_getChild(size_t idx) const
        { return getChild(idx); }

        virtual Component * BoxLayouter_getChild(size_t idx)
        { return getChild(idx); }

        virtual Rect BoxLayouter_clientRect() const
        { return clientRect(); }

        virtual void BoxLayouter_rebuildChildLayouts()
        { rebuildChildLayouts(); }

    protected:
        BoxLayouter mBoxLayouter;
    };


    class NativeBox : public NativeControl,
                      public BoxLayouter::ContentProvider
    {
    public:
        typedef NativeControl Super;

        NativeBox(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual Orient getOrient() const;

        virtual Align getAlign() const;

        virtual void rebuildLayout();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual Rect clientRect() const;

        virtual const Component * getChild(size_t idx) const;

        virtual Component * getChild(size_t idx);

        virtual void rebuildChildLayouts();
        
        virtual Orient BoxLayouter_getOrient() const
        { return getOrient(); }

        virtual Align BoxLayouter_getAlign() const
        { return getAlign(); }

        virtual size_t BoxLayouter_getChildCount() const
        { return getChildCount(); }

        virtual const Component * BoxLayouter_getChild(size_t idx) const
        { return getChild(idx); }

        virtual Component * BoxLayouter_getChild(size_t idx)
        { return getChild(idx); }

        virtual Rect BoxLayouter_clientRect() const
        { return clientRect(); }

        virtual void BoxLayouter_rebuildChildLayouts()
        { rebuildChildLayouts(); }

    private:
        BoxLayouter mBoxLayouter;
    };


    class MenuPopupContainer
    {
    public:
        virtual void showPopupMenu(RECT inToolbarButtonRect) = 0;
    };


    class MenuBarImpl : public PassiveComponent
    {
    public:
        typedef PassiveComponent Super;

        MenuBarImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class MenuImpl : public PassiveComponent,
                     public MenuPopupContainer,
                     public LabelController
    {
    public:
        typedef PassiveComponent Super;

        MenuImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual ~MenuImpl();

        virtual bool initAttributeControllers();
        
        // MenuPopupContainer methods
        virtual void showPopupMenu(RECT inExcludeRect);

        virtual std::string getLabel() const;

        virtual void setLabel(const std::string & inLabel);

        static MenuImpl * FindById(int inId);

    private:
        typedef std::map<int, MenuImpl*> MenusById;
        static MenusById sMenusById;

        std::string mLabel;
    };


    class MenuPopupImpl : public PassiveComponent
    {
    public:
        typedef PassiveComponent Super;

        MenuPopupImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        void show(RECT inExcludeRect);

    protected:
        virtual void onChildAdded(Component * inChild);

        virtual void onChildRemoved(Component * inChild);

    private:
        Windows::PopupMenu * getMenu();
    };


    class MenuItemImpl : public PassiveComponent,
                         public LabelController
    {
    public:
        typedef PassiveComponent Super;

        MenuItemImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual ~MenuItemImpl();

        virtual bool initAttributeControllers();
            
        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual std::string getLabel() const;

        virtual void setLabel(const std::string & inLabel);

        static MenuItemImpl * FindById(int inId);

    private:
        typedef std::map<int, MenuItemImpl*> MenuItemsById;
        static MenuItemsById sMenuItemsById;

        std::string mLabel;
    };


    class MenuListImpl : public NativeControl,
                         public MenuPopupContainer
    {
    public:
        typedef NativeControl Super;

        MenuListImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initComponent();
            
        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual void move(int x, int y, int w, int h);

        virtual void showPopupMenu(RECT inToolbarButtonRect);

        virtual void onContentChanged();

    private:
        void fillComboBox();

        std::vector<std::string> mItems;
    };


    class NativeSeparator : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeSeparator(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeSpacer : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        NativeSpacer(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeMenuButton : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeMenuButton(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class VirtualGrid : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        VirtualGrid(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual void rebuildLayout();
    };


    class NativeGrid : public NativeControl
    {
    public:
        typedef NativeComponent Super;

        NativeGrid(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual void rebuildLayout();
    };


    class NativeRows : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        NativeRows(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const { return 0; }

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const { return 0; }
    };


    class NativeRow : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        NativeRow(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeColumns : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        NativeColumns(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const { return 0; }

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const { return 0; }
    };


    class NativeColumn : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        NativeColumn(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual Align getAlign() const;

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeRadioGroup : public VirtualBox
    {
    public:
        typedef VirtualBox Super;

        NativeRadioGroup(Component * inParent, const AttributesMapping & inAttributesMapping);
    };


    class NativeRadio : public NativeControl
    {
    public:
        typedef NativeControl Super;

        NativeRadio(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class NativeProgressMeter : public NativeControl,
                                public virtual IntValueController
    {
    public:
        typedef NativeControl Super;

        NativeProgressMeter(Component * inParent, const AttributesMapping & inAttributesMapping);

        // IntValueController methods
        virtual int getValue() const;

        virtual void setValue(int inValue);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        bool initAttributeControllers();
    };


    class NativeDeck : public VirtualComponent,
                       public virtual SelectedIndexController
    {
    public:
        typedef VirtualComponent Super;

        NativeDeck(Component * inParent, const AttributesMapping & inAttributesMapping);

        // SelectedIndexController methods
        virtual int getSelectedIndex() const;

        virtual void setSelectedIndex(int inSelectedIndex);

        virtual void rebuildLayout();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        bool initAttributeControllers();

    private:
        int mSelectedIndex;
    };


    class NativeScrollbar : public NativeControl,
                            public virtual ScrollbarCurrentPositionController,
                            public virtual ScrollbarMaxPositionController,
                            public virtual ScrollbarIncrementController,
                            public virtual ScrollbarPageIncrementController
    {
    public:
        typedef NativeControl Super;

        NativeScrollbar(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int getCurrentPosition() const;

        virtual void setCurrentPosition(int inCurrentPosition);

        virtual int getMaxPosition() const;

        virtual void setMaxPosition(int inMaxPosition);

        virtual int getIncrement() const;

        virtual void setIncrement(int inIncrement);

        virtual int getPageIncrement() const;

        virtual void setPageIncrement(int inPageIncrement);

        class EventListener
        {
        public:
            virtual bool curposChanged(NativeScrollbar * inSender, int inOldPos, int inNewPos) = 0;
        };

        EventListener * eventHandler() { return mEventListener; }

        void setEventListener(EventListener * inEventListener)
        { mEventListener = inEventListener; }

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        bool initAttributeControllers();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    private:
        static DWORD GetFlags(const AttributesMapping & inAttributesMapping);

        EventListener * mEventListener;
        int mIncrement;
    };


    class TabsImpl : public PassiveComponent
    {
    public:
        typedef PassiveComponent Super;

        TabsImpl(Component * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TabImpl : public PassiveComponent
    {
    public:
        typedef PassiveComponent Super;

        TabImpl(Component * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TabPanelImpl;
    class TabImpl;
    class TabPanelsImpl : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        TabPanelsImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual ~TabPanelsImpl();
        
        void addTabPanel(TabPanelImpl * inPanel);

        virtual void rebuildLayout();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        static LRESULT MessageHandler(HWND inHandle, UINT inMessage, WPARAM wParam, LPARAM lParam);

    private:
        void update();

        TabImpl * getCorrespondingTab(size_t inIndex);
        HWND mParentHandle;
        HWND mTabBarHandle;
        typedef std::map<HWND, TabPanelsImpl*> Instances;
        static Instances sInstances;
        WNDPROC mOrigProc;
        size_t mChildCount;
        int mSelectedIndex;
    };


    class TabPanelImpl : public VirtualBox
    {
    public:
        typedef VirtualComponent Super;

        TabPanelImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initComponent();
    };


    class GroupBoxImpl : public VirtualBox
    {
    public:
        typedef VirtualBox Super;

        GroupBoxImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual ~GroupBoxImpl();

        void setCaption(const std::string & inLabel);

        virtual void rebuildLayout();

        virtual Orient getOrient() const;

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual size_t getChildCount() const;

        virtual const Component * getChild(size_t idx) const;

        virtual Component * getChild(size_t idx);

        virtual Rect clientRect() const;

    private:
        HWND mGroupBoxHandle;
        int mMarginLeft;
        int mMarginTop;
        int mMarginRight;
        int mMarginBottom;
    };


    class CaptionImpl : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        CaptionImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual bool initComponent();
    };
    

    class TreeItemInfo
    {
    public:
        TreeItemInfo() {}

        TreeItemInfo(const std::string & inLabel) : mLabel(inLabel){}

        const std::string & label() const
        { return mLabel; }

        void setLabel(const std::string & inLabel)
        { mLabel = inLabel; }

        size_t getChildCount() const
        { return mChildren.size(); }

        const TreeItemInfo * getChild(size_t idx) const
        { return mChildren[idx]; }

        TreeItemInfo * getChild(size_t idx)
        { return mChildren[idx]; }

        void addChild(TreeItemInfo * inItem)
        { mChildren.push_back(inItem); }

    private:
        std::string mLabel;
        typedef std::vector<TreeItemInfo* > Children;
        Children mChildren;
    };


    class TreeCellImpl;
    class TreeImpl : public NativeControl
    {
    public:
        typedef NativeControl Super;

        TreeImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        void addInfo(const TreeItemInfo & inInfo);

        HTREEITEM addInfo(HTREEITEM inRoot, HTREEITEM inPrev, const TreeItemInfo & inInfo);

        virtual bool initComponent();
    };

    
    class TreeItemImpl;
    class TreeChildrenImpl : public PassiveComponent
    {
    public:
        typedef PassiveComponent Super;

        TreeChildrenImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };

    
    class TreeRowImpl;
    class TreeItemImpl : public PassiveComponent
    {
    public:
        typedef PassiveComponent Super;

        TreeItemImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initComponent();

        const TreeItemInfo & itemInfo() const { return mItemInfo; }

        TreeItemInfo & itemInfo() { return mItemInfo; }

        bool isOpened() const;

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    private:
        TreeItemInfo mItemInfo;
    };


    class TreeColsImpl : public PassiveComponent
    {
    public:
        typedef PassiveComponent Super;

        TreeColsImpl(Component * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TreeColImpl : public PassiveComponent
    {
    public:
        typedef PassiveComponent Super;

        TreeColImpl(Component * inParent, const AttributesMapping & inAttributesMapping);
    };


    class TreeCellImpl;
    class TreeRowImpl : public PassiveComponent
    {
    public:
        typedef PassiveComponent Super;

        TreeRowImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class TreeCellImpl : public PassiveComponent,
                         public LabelController
    {
    public:
        typedef PassiveComponent Super;

        TreeCellImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initAttributeControllers();

        virtual std::string getLabel() const;

        virtual void setLabel(const std::string & inLabel);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    private:
        std::string mLabel;
    };



    class StatusbarImpl : public NativeControl,
                          public BoxLayouter::ContentProvider
    {
    public:
        typedef NativeControl Super;

        StatusbarImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initAttributeControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual Orient getOrient() const;

        virtual Align getAlign() const;

        virtual Rect clientRect() const;

        virtual void rebuildChildLayouts();

        virtual void rebuildLayout();

        virtual Orient BoxLayouter_getOrient() const
        { return getOrient(); }

        virtual Align BoxLayouter_getAlign() const
        { return getAlign(); }

        virtual size_t BoxLayouter_getChildCount() const
        { return getChildCount(); }

        virtual const Component * BoxLayouter_getChild(size_t idx) const
        { return getChild(idx); }

        virtual Component * BoxLayouter_getChild(size_t idx)
        { return getChild(idx); }

        virtual Rect BoxLayouter_clientRect() const
        { return clientRect(); }

        virtual void BoxLayouter_rebuildChildLayouts()
        { rebuildChildLayouts(); }

    private:
        BoxLayouter mBoxLayouter;
    };



    class StatusbarPanelImpl : public NativeControl
    {
    public:
        typedef NativeControl Super;

        StatusbarPanelImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initAttributeControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };



    class ToolbarImpl : public NativeControl,
                        public Windows::Toolbar::EventHandler,
                        public GdiplusLoader
    {
    public:
        typedef NativeControl Super;

        ToolbarImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual ~ToolbarImpl();

        virtual bool initComponent();

        virtual bool initAttributeControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual void rebuildLayout();

        // Toolbar::EventHandler methods
        virtual void onRequestFocus() {}

        boost::shared_ptr<Windows::Toolbar> nativeToolbar() const { return mToolbar; }

    private:
        boost::shared_ptr<Windows::Toolbar> mToolbar;
    };


    class ToolbarButtonImpl : public PassiveComponent,
                              public Windows::ToolbarDropDown::EventHandler,
                              public MenuPopupContainer,
                              public virtual DisabledController,
                              public virtual LabelController,
                              public virtual CSSListStyleImageController
    {
    public:
        typedef PassiveComponent Super;

        ToolbarButtonImpl(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initComponent();

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        // From ToolbarDropDown::EventHandler
        virtual void showToolbarMenu(RECT inToolbarButtonRect);

        // From MenuPopupContainer
        virtual void showPopupMenu(RECT inToolbarButtonRect);

        virtual std::string getLabel() const;

        virtual void setLabel(const std::string & inLabel);

        virtual bool isDisabled() const;

        virtual void setDisabled(bool inDisabled);

        virtual void setCSSListStyleImage(const std::string & inURL);

        virtual const std::string & getCSSListStyleImage() const;

    private:
        Windows::ConcreteToolbarItem * mButton;
        bool mDisabled;
        std::string mLabel;
        std::string mCSSListStyleImage;
    };


} // namespace XULWin


#endif // COMPONENT_H_INCLUDED