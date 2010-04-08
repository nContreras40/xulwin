#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED


#include "XULWin/AttributeController.h"
#include "XULWin/AttributesMapping.h"
#include "XULWin/Conversions.h"
#include "XULWin/BoxLayouter.h"
#include "XULWin/EventListener.h" // TODO: rename to NativeEventListener.h
#include "XULWin/Fallible.h"
#include "XULWin/GdiplusLoader.h"
#include "XULWin/Node.h"
#include "XULWin/Layout.h"
#include "XULWin/StyleController.h"
#include "XULWin/Toolbar.h"
#include "XULWin/ToolbarItem.h"
#include "XULWin/Windows.h"
#include "Poco/DOM/Element.h"
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include <CommCtrl.h>


namespace XULWin
{
    class Decorator;

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

        int intValue() const
        {
            return mId;
        }

    private:
        int mId;
        static int sId;
    };


    class Component;
    class Decorator;
    class BoxLayouter;
    class NativeComponent;
    typedef boost::shared_ptr<Component> ComponentPtr;



    /**
     * Listener for XUL notifications
     */
    class NotificationListener
    {
    public:
        // This notification is sent just after the child has been added.
        virtual void onChildAdded(Component * inChild) {}

        // This notification is sent after the child has been removed and
        // just before the child is destroyed.
        virtual void onChildRemoved(Component * inChild) {}

        virtual void onContentChanged() {}
    };


    /**
     * Component
     *
     * The Component class represents the C++ side of a XUL element.
     * It provides methods for getting and setting attributes. This by means
     * of a system of AttributeController classes.
     *
     * Each xul attribute that has an effect on what the native UI looks like
     * requires a matching AttributeController class (a class that inherits
     * AttributeController) that implements the conversion from/to the typed
     * attribute value to a string value (XML attributes are strings).
     */
    class Component : public NotificationListener,
        public virtual AlignController,
        public virtual CSSBackgroundColorController,
        public virtual CSSHeightController,
        public virtual CSSMarginController,
        public virtual CSSWidthController,
        public virtual CSSXController,
        public virtual CSSYController,
        public virtual CSSFillController,
        public virtual CSSStrokeController,
        public virtual FlexController,
        public virtual HeightController,
        public virtual HiddenController,
        public virtual OrientController,
        public virtual ScreenXController,
        public virtual ScreenYController,
        public virtual WidthController,
        private boost::noncopyable
    {
    public:        
        template<class ComponentT>
        static ComponentPtr Create(Component * inParentComponent, Poco::XML::Element * inElement)
        {
            ComponentPtr result(new ComponentT(inParentComponent, inElement));
            result->initAttributeControllers();
            result->initStyleControllers();
            result->init();
            return result;
        }

        virtual ~Component() {}
        
        virtual void addChild(ComponentPtr inComponent) = 0;

        // Returns this element's index in its parent's childNodes collection.
        //virtual int getIndex() const = 0;

        virtual size_t getChildCount() const = 0;

        virtual const Component * getChild(size_t inIndex) const = 0;

        virtual Component * getChild(size_t inIndex) = 0;

        virtual HWND getFirstParentHandle() = 0;

        virtual void invalidateRect() const = 0;

        virtual void setOwningElement(Poco::XML::Element * inElement) = 0;


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

        // CSSBackgroundColor methods
        virtual void setCSSBackgroundColor(const RGBColor & inColor) = 0;

        virtual RGBColor getCSSBackgroundColor() const = 0;

        // CSSWidthController methods
        virtual int getCSSWidth() const = 0;

        virtual void setCSSWidth(int inWidth) = 0;

        // CSSHeightController methods
        virtual int getCSSHeight() const = 0;

        virtual void setCSSHeight(int inHeight) = 0;

        // CSSMarginController methods
        virtual void getCSSMargin(int & outTop, int & outLeft, int & outRight, int & outBottom) const = 0;

        virtual void setCSSMargin(int inTop, int inLeft, int inRight, int inBottom) = 0;

        // CSS_SVG_FillController methods
        virtual void setCSSFill(const RGBColor & inColor) = 0;

        virtual const RGBColor & getCSSFill() const = 0;

        // CSS_SVG_StrokeController methods
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
            return const_cast<Type *>(static_cast<const Component *>(this)->downcast<Type>());
        }

        // Downcast that also resolves decorators.
        // Don't use dynamic_cast, because that would almost always fail since
        // Components are usually wrapped in Decorator objects.
        template<class Type>
        const Type * downcast() const
        {
            if (const Type * obj = dynamic_cast<const Type *>(this))
            {
                return obj;
            }
            //
            // NOTE: if you get the following compiler error:
            //  "error C2680: 'const XULWin::Decorator *' : invalid target type for dynamic_cast"
            // then that means that you must include "XULWin/Decorator.h" in your code.
            //
            else if (const Decorator * obj = dynamic_cast<const Decorator *>(this))
            {
                return obj->decoratedComponent()->downcast<Type>();
            }
            return 0;
        }


        template<class Type>
        Type * findParentOfType()
        {
            if (Type * obj = dynamic_cast<Type *>(this))
            {
                return obj;
            }
            else if (const Decorator * obj = dynamic_cast<const Decorator *>(this))
            {
                return obj->decoratedComponent()->findParentOfType<Type>();
            }
            else if (el() && el()->parentNode() && el()->parentNode())
            {
                return el()->parentNode()->findParentOfType<Type>();
            }
            return 0;
        }


        // Searches for a child of given type.
        // Returns the first one found.
        // Only searches one level deep.
        template<class Type>
        Type * findChildOfType()
        {
            return const_cast<Type *>(static_cast<const Component *>(this)->findChildOfType<Type>());
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

        virtual Poco::XML::Element * el() const = 0;

        virtual Component * parentComponent() const = 0;

        virtual void sync() = 0;

        virtual void rebuildLayout() = 0;

        virtual void rebuildChildLayouts() = 0;

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam) = 0;

        virtual bool getAttribute(const std::string & inName, std::string & outValue) = 0;

        virtual bool getStyle(const std::string & inName, std::string & outValue) = 0;

        virtual bool setStyle(const std::string & inName, const std::string & inValue) = 0;

        virtual bool setAttribute(const std::string & inName, const std::string & inValue) = 0;

        virtual bool initAttributeControllers() = 0;

        virtual bool initStyleControllers() = 0;        

    private:
        virtual bool init() = 0;
        friend class Decorator;
    };


    /**
     * ConcreteComponent is the base class for non-decorator components.
     */
    class ConcreteComponent : public Component
    {
    public:
        ConcreteComponent(Component * inParent, Poco::XML::Element * inElement);

        virtual ~ConcreteComponent() = 0;

        //virtual int getIndex() const;

        virtual void addChild(ComponentPtr inComponent);

        virtual size_t getChildCount() const;

        virtual const Component * getChild(size_t inIndex) const;

        virtual Component * getChild(size_t inIndex);

        virtual HWND getFirstParentHandle();

        virtual void invalidateRect() const;

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

        // CSSBackgroundColor methods
        virtual void setCSSBackgroundColor(const RGBColor & inColor);

        virtual RGBColor getCSSBackgroundColor() const;

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
            if (Type * obj = dynamic_cast<Type *>(this))
            {
                return obj;
            }
            else if (Decorator * obj = dynamic_cast<Decorator *>(this))
            {
                return obj->decoratedComponent()->downcast<Type>();
            }
            return 0;
        }


        template<class ConstType>
        const ConstType * downcast() const
        {
            if (const ConstType * obj = dynamic_cast<const ConstType *>(this))
            {
                return obj;
            }
            else if (const Decorator * obj = dynamic_cast<const Decorator *>(this))
            {
                return obj->decoratedComponent()->downcast<ConstType>();
            }
            return 0;
        }

        int commandId() const
        {
            return mCommandId.intValue();
        }

        virtual int getWidth(SizeConstraint inSizeConstraint) const;

        virtual int getHeight(SizeConstraint inSizeConstraint) const;

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const = 0;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const = 0;

        // Tendency to expand, used for separators, scrollbars, etc..
        bool expansive() const;

        virtual void move(int x, int y, int w, int h) = 0;

        virtual void move(const Rect & inRect);

        virtual Rect clientRect() const = 0;

        virtual void setOwningElement(Poco::XML::Element * inElement);

        virtual Poco::XML::Element * el() const;

        Component * parentComponent() const;
        
        virtual void sync();

        virtual void rebuildLayout() = 0;

        void rebuildChildLayouts();

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam) = 0;

        virtual bool getAttribute(const std::string & inName, std::string & outValue);

        virtual bool getStyle(const std::string & inName, std::string & outValue);

        virtual bool setStyle(const std::string & inName, const std::string & inValue);

        virtual bool setAttribute(const std::string & inName, const std::string & inValue);

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        template<class T>
        void setAttributeController(T * inAttributeController)
        {
            AttributeControllers::iterator it = mAttributeControllers.find(T::AttributeName());
            assert(it == mAttributeControllers.end());
            if (it == mAttributeControllers.end())
            {
                mAttributeControllers.insert(std::make_pair(T::AttributeName(), inAttributeController));
            }
        }

        template<class T>
        void setStyleController(T * inStyleController)
        {
            StyleControllers::iterator it = mStyleControllers.find(T::PropertyName());
            assert(it == mStyleControllers.end());
            if (it == mStyleControllers.end())
            {
                mStyleControllers.insert(std::make_pair(T::PropertyName(), inStyleController));
            }
        }

        int calculateMaxChildWidth(SizeConstraint inSizeConstraint) const;

        int calculateMaxChildHeight(SizeConstraint inSizeConstraint) const;

        int calculateSumChildWidths(SizeConstraint inSizeConstraint) const;

        int calculateSumChildHeights(SizeConstraint inSizeConstraint) const;

    protected:
        Component * mParentComponent;
        Poco::XML::Element * mElement;
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
        Fallible<int> mStrokeWidth;
        Fallible<RGBColor> mCSSFill;
        Fallible<RGBColor> mCSSStroke;
        Fallible<RGBColor> mCSSBackgroundColor;
        Fallible<Orient> mOrient;
        Fallible<Align> mAlign;
        bool mIsInitialized;

        // We need to remember the hidden state ourselves because we can't
        // rely on WinAPI IsWindowVisible call, because it will return false
        // for child windows of which the parent is not visible.
        // This is a problem because calculation of minimum size depends on
        // visibility of child items. Hidden elements return 0 when asked for
        // their min width or height. This problem becomes apparent when
        // calling Window's showModal method which calculates its minimum
        // height *before* the window becomes visible. This would result in a
        // 'minified' window state.
        bool mHidden;

        typedef std::map<std::string, AttributeController *> AttributeControllers;
        AttributeControllers mAttributeControllers;

        typedef std::map<std::string, StyleController *> StyleControllers;
        StyleControllers mStyleControllers;
    
    private:
        virtual bool init();

        typedef std::vector<ComponentPtr> Children;
        
        // Component is owner of its child components.
        // This is done for ownership management.
        // Not suitable DOM nativation because not all
        // elements have Component counterparts.
        Children mChildren;
    };


    /**
     * NativeComponent
     *
     * Native components are UI components that map to a native HWND.
     * This is the base class for any native component.
     */
    class NativeComponent : public ConcreteComponent,
        public virtual DisabledController,
        public virtual LabelController
    {
    public:
        typedef ConcreteComponent Super;

        NativeComponent(Component * inParent, Poco::XML::Element * inDOMElement);

        virtual ~NativeComponent();

        virtual void invalidateRect() const;

        virtual void setHandle(HWND inHandle, bool inPassOwnership);

        bool addEventListener(NativeEventListener * inEventListener);

        bool removeEventListener(NativeEventListener * inEventListener);

        // DisabledController methods
        virtual bool isDisabled() const;

        virtual void setDisabled(bool inDisabled);

        // LabelController methods
        virtual std::string getLabel() const;

        virtual void setLabel(const std::string & inLabel);

        // HiddenController methods
        virtual void setHidden(bool inHidden);

        // Override component coloring.
        virtual bool getCustomBrush(HDC inHDC, HWND inHWND, HBRUSH & outHBRUSH);

        static void SetModuleHandle(HMODULE inModule);

        static HMODULE GetModuleHandle();

        virtual HWND handle() const;

        virtual bool initAttributeControllers();

        virtual bool initStyleControllers();

        virtual void handleCommand(WPARAM wParam, LPARAM lParam);

        //virtual void handleMenuCommand(WORD inMenuId);

        virtual void handleDialogCommand(WORD inNotificationCode, WPARAM wParam, LPARAM lParam);

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

#ifndef SWIG
        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);
#endif

    protected:
        static NativeComponent * FindByHandle(HWND inHandle);

        static NativeComponent * FindById(int inId);

        void registerHandle();

        void subclass();

        void unsubclass();

        void unregisterHandle();

        HWND mHandle;
        HMODULE mModuleHandle;


        typedef std::set<NativeEventListener *> EventListeners;
        EventListeners mEventListeners;

    private:
        typedef std::map<int, NativeComponent *> ComponentsById;
        static ComponentsById sComponentsById;

        typedef std::map<HWND, NativeComponent *> ComponentsByHandle;
        static ComponentsByHandle sComponentsByHandle;

        WNDPROC mOrigProc;

        static HMODULE sModuleHandle;
        bool mOwnsHandle;
    };


    class Window;


    /**
     * Dialog
     *
     * Represents a XUL dialog window.
     * In reality it is a Window with some customizations for Dialog-like behavior.
     */
    class Dialog : public NativeComponent,
        public BoxLayouter::ContentProvider,
        public virtual TitleController
    {
    public:
        typedef NativeComponent Super;

        static const char * TagName() { return "dialog"; }

        static ComponentPtr Create(Component * inParentComponent, Poco::XML::Element * inDOMElement)
        { return Component::Create<Dialog>(inParentComponent, inDOMElement); }

        static void Register(HMODULE inModuleHandle);

        Dialog(Component * inParent, Poco::XML::Element * inDOMElement);

        virtual ~Dialog();

        // BoxLayouter
        virtual Orient getOrient() const;

        // BoxLayouter
        virtual Align getAlign() const;

        // TitleController methods
        virtual std::string getTitle() const;

        virtual void setTitle(const std::string & inTitle);

        DialogResult showModal(Window * inInvoker);

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
        {
            return getOrient();
        }

        virtual Align BoxLayouter_getAlign() const
        {
            return getAlign();
        }

        virtual size_t BoxLayouter_getChildCount() const
        {
            return getChildCount();
        }

        virtual const Component * BoxLayouter_getChild(size_t idx) const
        {
            return getChild(idx);
        }

        virtual Component * BoxLayouter_getChild(size_t idx)
        {
            return getChild(idx);
        }

        virtual Rect BoxLayouter_clientRect() const
        {
            return clientRect();
        }

        virtual void BoxLayouter_rebuildChildLayouts()
        {
            rebuildChildLayouts();
        }

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

#ifndef SWIG
        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT inMessage, WPARAM wParam, LPARAM lParam);
#endif
    private:
        // Invoker is the stored parameter for showModal.
        Window * mInvoker;
        BoxLayouter mBoxLayouter;
        DialogResult mDialogResult;
    };


    /**
     * NativeControl
     *
     * A native control is a control that is not a Window
     * or a Dialog and that maps to a native HWND.
     * It is the base class for native WinAPI controls.
     */
    class NativeControl : public NativeComponent
    {
    public:
        typedef NativeComponent Super;

        NativeControl(Component * inParent, Poco::XML::Element * inDOMElement, LPCTSTR inClassName, DWORD inExStyle, DWORD inStyle);

        // use this constructor if you want to provide your own handle later using NativeControl::setHandle
        NativeControl(Component * inParent, Poco::XML::Element * inDOMElement);

        virtual ~NativeControl();

        bool initStyleControllers();

        virtual void rebuildLayout();

        virtual Rect clientRect() const;

        virtual void move(int x, int y, int w, int h);

        // Gets a NativeComponent object from this object. This
        // is only needed in constructors of s, because
        // they need to know which is their native parent handle object.
        // If this is a NativeComponent, return this.
        // If this is a VirtualComponent, return first parent that is a NativeComponent.
        // If this is a Decorator, resolve until a NativeComponent is found.
        static NativeComponent * GetThisOrParent(Component * inElement);
        static const NativeComponent * GetThisOrParent(const Component * inElement);
    };


    /**
     * VirtualComponent
     *
     * A virtual component is a widget that does not map to a native HWND.
     * Layout elements (box, grid, deck ..) are implemented as virtual components.
     */
    class VirtualComponent : public ConcreteComponent
    {
    public:
        typedef ConcreteComponent Super;

        VirtualComponent(Component * inParent, Poco::XML::Element * inDOMElement);

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


    /**
     * PassiveComponent
     *
     * A passive component represents XUL elements that do not map to
     * a native HWND and that also do not ask for layout space. Often
     * they are not a widget at all, for example "listcols".
     */
    class PassiveComponent : public VirtualComponent
    {
    public:
        typedef VirtualComponent Super;

        PassiveComponent(Component * inParent, Poco::XML::Element * inDOMElement);

        virtual ~PassiveComponent();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const
        {
            return 0;
        }

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const
        {
            return 0;
        }
    };


    /**
     * Button
     *
     * Native implementation for a XUL button.
     */
    class Button : public NativeControl
    {
    public:
        typedef NativeControl Super;

        static const char * TagName() { return "button"; }

        static ComponentPtr Create(Component * inParentComponent, Poco::XML::Element * inDOMElement)
        { return Component::Create<Button>(inParentComponent, inDOMElement); }

        Button(Component * inParent, Poco::XML::Element * inDOMElement);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    /**
    * Description
    *
    * Native implementation for a XUL description element.
    */
    class Description : public NativeControl,
        public virtual StringValueController
    {
    public:
        typedef NativeControl Super;

        static const char * TagName() { return "description"; }

        static ComponentPtr Create(Component * inParentComponent, Poco::XML::Element * inDOMElement)
        { return Component::Create<Description>(inParentComponent, inDOMElement); }

        Description(Component * inParent, Poco::XML::Element * inDOMElement);

        // StringValueController methods
        virtual std::string getValue() const;

        virtual void setValue(const std::string & inStringValue);

        virtual bool initAttributeControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    class TextBox : public NativeControl,
        public virtual StringValueController,
        public virtual ReadOnlyController,
        public virtual RowsController
    {
    public:
        typedef NativeControl Super;

        static const char * TagName() { return "textbox"; }

        static ComponentPtr Create(Component * inParentComponent, Poco::XML::Element * inDOMElement)
        { return Component::Create<TextBox>(inParentComponent, inDOMElement); }

        TextBox(Component * inParent, Poco::XML::Element * inDOMElement);

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

        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    private:
        int mRows;
        static DWORD GetFlags(Poco::XML::Element * inDOMElement);
    };


    class CheckBox : public NativeControl,
        public virtual CheckedController
    {
    public:
        typedef NativeControl Super;

        static const char * TagName() { return "checkbox"; }

        static ComponentPtr Create(Component * inParentComponent, Poco::XML::Element * inDOMElement)
        { return Component::Create<CheckBox>(inParentComponent, inDOMElement); }

        CheckBox(Component * inParent, Poco::XML::Element * inDOMElement);

        // CheckedController methods
        virtual bool isChecked() const;

        virtual void setChecked(bool inChecked);

        virtual bool initAttributeControllers();

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    };


    //class VirtualBox : public VirtualComponent,
    //    public BoxLayouter::ContentProvider
    //{
    //public:
    //    typedef VirtualComponent Super;

    //    VirtualBox(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual Orient getOrient() const;

    //    virtual Align getAlign() const;

    //    virtual bool initAttributeControllers();

    //    virtual void rebuildLayout();

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const
    //    {
    //        return mBoxLayouter.calculateWidth(inSizeConstraint);
    //    }

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const
    //    {
    //        return mBoxLayouter.calculateHeight(inSizeConstraint);
    //    }

    //    virtual Rect clientRect() const
    //    {
    //        return Super::clientRect();
    //    }

    //    virtual void rebuildChildLayouts()
    //    {
    //        return Super::rebuildChildLayouts();
    //    }

    //    virtual Orient BoxLayouter_getOrient() const
    //    {
    //        return getOrient();
    //    }

    //    virtual Align BoxLayouter_getAlign() const
    //    {
    //        return getAlign();
    //    }

    //    virtual size_t BoxLayouter_getChildCount() const
    //    {
    //        return getChildCount();
    //    }

    //    virtual const Component * BoxLayouter_getChild(size_t idx) const
    //    {
    //        return getChild(idx);
    //    }

    //    virtual Component * BoxLayouter_getChild(size_t idx)
    //    {
    //        return getChild(idx);
    //    }

    //    virtual Rect BoxLayouter_clientRect() const
    //    {
    //        return clientRect();
    //    }

    //    virtual void BoxLayouter_rebuildChildLayouts()
    //    {
    //        rebuildChildLayouts();
    //    }

    //protected:
    //    BoxLayouter mBoxLayouter;
    //};


    //class Box : public NativeControl,
    //    public BoxLayouter::ContentProvider
    //{
    //public:
    //    typedef NativeControl Super;

    //    static const char * TagName() { return "box"; }

    //    static ComponentPtr Create(Component * inParentComponent, Poco::XML::Element * inDOMElement)
    //    { return Component::Create<Box>(inParentComponent, inDOMElement); }

    //    Box(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual Orient getOrient() const;

    //    virtual Align getAlign() const;

    //    virtual void rebuildLayout();

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    virtual Rect clientRect() const;

    //    virtual const Component * getChild(size_t idx) const;

    //    virtual Component * getChild(size_t idx);

    //    virtual Orient BoxLayouter_getOrient() const
    //    {
    //        return getOrient();
    //    }

    //    virtual Align BoxLayouter_getAlign() const
    //    {
    //        return getAlign();
    //    }

    //    virtual size_t BoxLayouter_getChildCount() const
    //    {
    //        return getChildCount();
    //    }

    //    virtual const Component * BoxLayouter_getChild(size_t idx) const
    //    {
    //        return getChild(idx);
    //    }

    //    virtual Component * BoxLayouter_getChild(size_t idx)
    //    {
    //        return getChild(idx);
    //    }

    //    virtual Rect BoxLayouter_clientRect() const
    //    {
    //        return clientRect();
    //    }

    //    virtual void BoxLayouter_rebuildChildLayouts()
    //    {
    //        rebuildChildLayouts();
    //    }

    //private:
    //    BoxLayouter mBoxLayouter;
    //};


    //class MenuPopupContainer
    //{
    //public:
    //    virtual void showPopupMenu(RECT inToolbarButtonRect) = 0;
    //};


    //class MenuList : public NativeControl
    //{
    //public:
    //    typedef NativeControl Super;

    //    static const char * TagName() { return "menulist"; }

    //    static ComponentPtr Create(Component * inParentComponent, Poco::XML::Element * inDOMElement)
    //    { return Component::Create<MenuList>(inParentComponent, inDOMElement); }

    //    MenuList(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual bool init();

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    virtual void move(int x, int y, int w, int h);

    //    virtual void onContentChanged();

    //private:
    //    void fillComboBox();
    //};


    //class Separator : public NativeControl
    //{
    //public:
    //    typedef NativeControl Super;

    //    static const char * TagName() { return "separator"; }

    //    static ComponentPtr Create(Component * inParentComponent, Poco::XML::Element * inDOMElement)
    //    { return Component::Create<Separator>(inParentComponent, inDOMElement); }

    //    Separator(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    //};


    //class Spacer : public VirtualComponent
    //{
    //public:
    //    typedef VirtualComponent Super;

    //    static const char * TagName() { return "spacer"; }

    //    static ComponentPtr Create(Component * inParentComponent, Poco::XML::Element * inDOMElement)
    //    { return Component::Create<Spacer>(inParentComponent, inDOMElement); }

    //    Spacer(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    //};


    //class MenuButton : public NativeControl
    //{
    //public:
    //    typedef NativeControl Super;

    //    static const char * TagName() { return "menubutton"; }

    //    static ComponentPtr Create(Component * inParentComponent, Poco::XML::Element * inDOMElement)
    //    { return Component::Create<MenuButton>(inParentComponent, inDOMElement); }

    //    MenuButton(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    //};


    //class VirtualGrid : public VirtualComponent
    //{
    //public:
    //    typedef VirtualComponent Super;

    //    static const char * TagName()
    //    {
    //        return "grid";
    //    }

    //    VirtualGrid(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    virtual void rebuildLayout();
    //};


    //class Grid : public NativeControl
    //{
    //public:
    //    typedef NativeControl Super;

    //    static const char * TagName()
    //    {
    //        return "grid";
    //    }

    //    Grid(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    virtual void rebuildLayout();
    //};


    //class Rows : public VirtualComponent
    //{
    //public:
    //    typedef VirtualComponent Super;

    //    static const char * TagName()
    //    {
    //        return "rows";
    //    }

    //    Rows(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    //};


    //class Row : public VirtualComponent
    //{
    //public:
    //    typedef VirtualComponent Super;

    //    static const char * TagName()
    //    {
    //        return "row";
    //    }

    //    Row(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    //};


    //class Columns : public VirtualComponent
    //{
    //public:
    //    typedef VirtualComponent Super;

    //    static const char * TagName()
    //    {
    //        return "columns";
    //    }

    //    Columns(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    //};


    //class Column : public VirtualComponent
    //{
    //public:
    //    typedef VirtualComponent Super;

    //    static const char * TagName()
    //    {
    //        return "column";
    //    }

    //    Column(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual Align getAlign() const;

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    //};


    //class RadioGroup : public VirtualBox
    //{
    //public:
    //    typedef VirtualBox Super;

    //    RadioGroup(Component * inParent, Poco::XML::Element * inDOMElement);
    //};


    //class Radio : public NativeControl
    //{
    //public:
    //    typedef NativeControl Super;

    //    Radio(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    //};


    //class ProgressMeter : public NativeControl,
    //    public virtual IntValueController
    //{
    //public:
    //    typedef NativeControl Super;

    //    ProgressMeter(Component * inParent, Poco::XML::Element * inDOMElement);

    //    // IntValueController methods
    //    virtual int getValue() const;

    //    virtual void setValue(int inValue);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    bool initAttributeControllers();
    //};


    //class Deck : public VirtualComponent,
    //    public virtual SelectedIndexController
    //{
    //public:
    //    typedef VirtualComponent Super;

    //    Deck(Component * inParent, Poco::XML::Element * inDOMElement);

    //    // SelectedIndexController methods
    //    virtual int getSelectedIndex() const;

    //    virtual void setSelectedIndex(int inSelectedIndex);

    //    virtual void rebuildLayout();

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    bool initAttributeControllers();

    //private:
    //    int mSelectedIndex;
    //};


    //class Scrollbar : public NativeControl,
    //    public virtual ScrollbarCurrentPositionController,
    //    public virtual ScrollbarMaxPositionController,
    //    public virtual ScrollbarIncrementController,
    //    public virtual ScrollbarPageIncrementController
    //{
    //public:
    //    typedef NativeControl Super;

    //    Scrollbar(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int getCurrentPosition() const;

    //    virtual void setCurrentPosition(int inCurrentPosition);

    //    virtual int getMaxPosition() const;

    //    virtual void setMaxPosition(int inMaxPosition);

    //    virtual int getIncrement() const;

    //    virtual void setIncrement(int inIncrement);

    //    virtual int getPageIncrement() const;

    //    virtual void setPageIncrement(int inPageIncrement);

    //    class EventHandler
    //    {
    //    public:
    //        virtual bool curposChanged(Scrollbar * inSender, int inOldPos, int inNewPos) = 0;
    //    };

    //    EventHandler * eventHandler()
    //    {
    //        return mEventHandler;
    //    }

    //    void setEventHandler(EventHandler * inEventHandler)
    //    {
    //        mEventHandler = inEventHandler;
    //    }

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    bool initAttributeControllers();

    //    virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    //private:
    //    static DWORD GetFlags(Poco::XML::Element * inDOMElement);

    //    EventHandler * mEventHandler;
    //    ScopedEventListener mEventListener;
    //    int mIncrement;
    //};


    //class Tabs : public PassiveComponent
    //{
    //public:
    //    typedef PassiveComponent Super;

    //    static const char * TagName()
    //    {
    //        return "tabs";
    //    }

    //    Tabs(Component * inParent, Poco::XML::Element * inDOMElement);
    //};


    //class Tab : public PassiveComponent
    //{
    //public:
    //    typedef PassiveComponent Super;

    //    Tab(Component * inParent, Poco::XML::Element * inDOMElement);
    //};


    //class TabPanel;
    //class Tab;
    //class TabPanels : public VirtualComponent
    //{
    //public:
    //    typedef VirtualComponent Super;

    //    TabPanels(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual ~TabPanels();

    //    void addTabPanel(TabPanel * inPanel);

    //    virtual void rebuildLayout();

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    static LRESULT MessageHandler(HWND inHandle, UINT inMessage, WPARAM wParam, LPARAM lParam);

    //private:
    //    void update();

    //    Tab * getCorrespondingTab(size_t inIndex);
    //    HWND mParentHandle;
    //    HWND mTabBarHandle;
    //    typedef std::map<HWND, TabPanels *> Instances;
    //    static Instances sInstances;
    //    WNDPROC mOrigProc;
    //    size_t mChildCount;
    //    int mSelectedIndex;
    //};


    //class TabPanel : public VirtualBox
    //{
    //public:
    //    typedef VirtualComponent Super;

    //    TabPanel(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual bool init();
    //};


    //class GroupBox : public VirtualBox
    //{
    //public:
    //    typedef VirtualBox Super;

    //    GroupBox(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual ~GroupBox();

    //    void setCaption(const std::string & inLabel);

    //    virtual void rebuildLayout();

    //    virtual Orient getOrient() const;

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    virtual size_t getChildCount() const;

    //    virtual const Component * getChild(size_t idx) const;

    //    virtual Component * getChild(size_t idx);

    //    virtual Rect clientRect() const;

    //private:
    //    HWND mGroupBoxHandle;
    //    int mMarginLeft;
    //    int mMarginTop;
    //    int mMarginRight;
    //    int mMarginBottom;
    //};


    //class Caption : public VirtualComponent
    //{
    //public:
    //    typedef VirtualComponent Super;

    //    Caption(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    virtual bool init();
    //};


    //typedef GenericNode<std::string, ContainerPolicy_Vector, PointerPolicy_Normal_NoOwnership> TreeItemInfo;


    //class TreeCell;
    //class Tree : public NativeControl
    //{
    //public:
    //    typedef NativeControl Super;

    //    Tree(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    void addInfo(const TreeItemInfo & inInfo);

    //    HTREEITEM addInfo(HTREEITEM inRoot, HTREEITEM inPrev, const TreeItemInfo & inInfo);

    //    virtual bool init();
    //};


    //class TreeItem;
    //class TreeChildren : public PassiveComponent
    //{
    //public:
    //    typedef PassiveComponent Super;

    //    TreeChildren(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    //};


    //class TreeRow;
    //class TreeItem : public PassiveComponent
    //{
    //public:
    //    typedef PassiveComponent Super;

    //    TreeItem(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual bool init();

    //    const TreeItemInfo & itemInfo() const
    //    {
    //        return mItemInfo;
    //    }

    //    TreeItemInfo & itemInfo()
    //    {
    //        return mItemInfo;
    //    }

    //    bool isOpened() const;

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //private:
    //    TreeItemInfo mItemInfo;
    //};


    //class TreeCols : public PassiveComponent
    //{
    //public:
    //    typedef PassiveComponent Super;

    //    TreeCols(Component * inParent, Poco::XML::Element * inDOMElement);
    //};


    //class TreeCol : public PassiveComponent
    //{
    //public:
    //    typedef PassiveComponent Super;

    //    TreeCol(Component * inParent, Poco::XML::Element * inDOMElement);
    //};


    //class TreeCell;
    //class TreeRow : public PassiveComponent
    //{
    //public:
    //    typedef PassiveComponent Super;

    //    TreeRow(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    //};


    //class TreeCell : public PassiveComponent,
    //    public LabelController
    //{
    //public:
    //    typedef PassiveComponent Super;

    //    TreeCell(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual bool initAttributeControllers();

    //    virtual std::string getLabel() const;

    //    virtual void setLabel(const std::string & inLabel);

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //private:
    //    std::string mLabel;
    //};



    //class Statusbar : public NativeControl,
    //    public BoxLayouter::ContentProvider
    //{
    //public:
    //    typedef NativeControl Super;

    //    Statusbar(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual bool initAttributeControllers();

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    virtual Orient getOrient() const;

    //    virtual Align getAlign() const;

    //    virtual Rect clientRect() const;

    //    virtual void rebuildChildLayouts();

    //    virtual void rebuildLayout();

    //    virtual Orient BoxLayouter_getOrient() const
    //    {
    //        return getOrient();
    //    }

    //    virtual Align BoxLayouter_getAlign() const
    //    {
    //        return getAlign();
    //    }

    //    virtual size_t BoxLayouter_getChildCount() const
    //    {
    //        return getChildCount();
    //    }

    //    virtual const Component * BoxLayouter_getChild(size_t idx) const
    //    {
    //        return getChild(idx);
    //    }

    //    virtual Component * BoxLayouter_getChild(size_t idx)
    //    {
    //        return getChild(idx);
    //    }

    //    virtual Rect BoxLayouter_clientRect() const
    //    {
    //        return clientRect();
    //    }

    //    virtual void BoxLayouter_rebuildChildLayouts()
    //    {
    //        rebuildChildLayouts();
    //    }

    //private:
    //    BoxLayouter mBoxLayouter;
    //};



    //class StatusbarPanel : public NativeControl
    //{
    //public:
    //    typedef NativeControl Super;

    //    StatusbarPanel(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual bool initAttributeControllers();

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
    //};


    //class Toolbar : public NativeControl,
    //    public Windows::Toolbar::EventHandler,
    //    public GdiplusLoader
    //{
    //public:
    //    typedef NativeControl Super;

    //    Toolbar(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual ~Toolbar();

    //    virtual bool init();

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    virtual void rebuildLayout();

    //    // ::EventHandler methods
    //    virtual void onRequestFocus() {}

    //    boost::shared_ptr<Windows::Toolbar> nativeToolbar() const
    //    {
    //        return mToolbar;
    //    }

    //private:
    //    boost::shared_ptr<Windows::Toolbar> mToolbar;
    //};


    //class ToolbarButton : public PassiveComponent,
    //    public Windows::ToolbarDropDown::EventHandler,
    //    public MenuPopupContainer,
    //    public virtual DisabledController,
    //    public virtual LabelController,
    //    public virtual CSSListStyleImageController
    //{
    //public:
    //    typedef PassiveComponent Super;

    //    ToolbarButton(Component * inParent, Poco::XML::Element * inDOMElement);

    //    virtual bool initAttributeControllers();

    //    virtual bool initStyleControllers();

    //    virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

    //    virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

    //    // From ToolbarDropDown::EventHandler
    //    virtual void showToolbarMenu(RECT inToolbarButtonRect);

    //    // From MenuPopupContainer
    //    virtual void showPopupMenu(RECT inToolbarButtonRect);

    //    virtual std::string getLabel() const;

    //    virtual void setLabel(const std::string & inLabel);

    //    virtual bool isDisabled() const;

    //    virtual void setDisabled(bool inDisabled);

    //    virtual void setCSSListStyleImage(const std::string & inURL);

    //    virtual const std::string & getCSSListStyleImage() const;

    //    Windows::ConcreteToolbarItem * nativeItem()
    //    {
    //        return mButton;
    //    }

    //private:
    //    Windows::ConcreteToolbarItem * mButton;
    //    bool mDisabled;
    //    std::string mLabel;
    //    std::string mCSSListStyleImage;
    //};


} // namespace XULWin


#endif // COMPONENT_H_INCLUDED
