#include "XULWin/Element.h"
#include "XULWin/Component.h"
#include "XULWin/ComponentFactory.h"
#include "XULWin/Decorator.h"
#include "XULWin/Defaults.h"
#include "XULWin/ElementFactory.h"
#include "XULWin/ErrorReporter.h"
#include "XULWin/ToolbarCustomWindowDecorator.h"
#include "XULWin/WinUtils.h"
#include <boost/bind.hpp>


namespace XULWin
{

    Element::Element(const std::string & inType, Element * inParent, Component * inNative) :
        mType(inType),
        mParent(inParent),
        mComponent(inNative)
    {
        if (mComponent)
        {
            mComponent->setOwningElement(this);
        }
    }


    Element::~Element()
    {
        // Children require parent access while destructing.
        // So we destruct them while parent still alive.
        mChildren.clear();
    }

    
    bool Element::init()
    {
        return component()->init();
    }

    
    const std::string & Element::type() const
    {
        return mType;
    }


    void Element::setInnerText(const std::string & inText)
    {
        mInnerText = inText;
    }


    const std::string & Element::innerText() const
    {
        return mInnerText;
    }

    
    Element * Element::getElementById(const std::string & inId)
    {
        struct Helper
        {
            static Element * findChildById(const Children & inChildren, const std::string & inId)
            {
                Element * result(0);
                for (size_t idx = 0; idx != inChildren.size(); ++idx)
                {
                    ElementPtr child = inChildren[idx];
                    if (child->getAttribute("id") == inId)
                    {
                        result = child.get();
                        break;
                    }
                    else
                    {
                        result = findChildById(child->children(), inId);
                        if (result)
                        {
                            break;
                        }
                    }
                }
                return result;
            }
        };
        Element * result = Helper::findChildById(children(), inId);
        if (!result)
        {
            ReportError("Element with id '" + inId + "' was not found.");
        }
        return result;
    }
    
    
    void Element::getElementsByType(const std::string & inType, std::vector<Element*> & outElements)
    {
        if (type() == inType)
        {
            outElements.push_back(this);
        }
        for (size_t idx = 0; idx != mChildren.size(); ++idx)
        {
            mChildren[idx]->getElementsByType(inType, outElements);
        }
    }


    void Element::removeChild(const Element * inChild)
    {
        Children::iterator it = std::find_if(mChildren.begin(), mChildren.end(), boost::bind(&ElementPtr::get, _1) == inChild);
        if (it != mChildren.end())
        {
            ElementPtr keepAlive = *it;
            mChildren.erase(it);
            mComponent->rebuildLayout();            
            mComponent->onChildRemoved(keepAlive->component());
            // keepAlive loses scope here and destroys child
        }
        else
        {
            ReportError("Remove child failed because it wasn't found.");
        }
    }


    void Element::removeAllChildren()
    {
        while (!mChildren.empty())
        {
            ElementPtr keepAlive = *mChildren.begin();
            mChildren.erase(mChildren.begin());
            mComponent->rebuildLayout();            
            mComponent->onChildRemoved(keepAlive->component());
            // keepAlive loses scope here and destroys child
        }
    }


    void Element::setStyles(const AttributesMapping & inAttributes)
    {
        AttributesMapping::const_iterator it = inAttributes.find("style");
        if (it != inAttributes.end())
        {
            Poco::StringTokenizer keyValuePairs(it->second, ";",
                                                Poco::StringTokenizer::TOK_IGNORE_EMPTY
                                                | Poco::StringTokenizer::TOK_TRIM);

            Poco::StringTokenizer::Iterator it = keyValuePairs.begin(), end = keyValuePairs.end();
            for (; it != end; ++it)
            {
                std::string::size_type sep = it->find(":");
                if (sep != std::string::npos && (sep + 1) < it->size())
                {
                    std::string key = it->substr(0, sep);
                    std::string value = it->substr(sep + 1, it->size() - sep - 1);
                    setStyle(key, value);
                }
            }
        }
    }


    void Element::setAttributes(const AttributesMapping & inAttributes)
    {
        mAttributes = inAttributes;
        
        if (mComponent)
        {
            AttributesMapping::iterator it = mAttributes.begin(), end = mAttributes.end();
            for (; it != end; ++it)
            {
                setAttribute(it->first, it->second);
            }
        }
    }


    void Element::initAttributeControllers()
    {
        if (mComponent)
        {
            mComponent->initAttributeControllers();
        }
    }


    void Element::initStyleControllers()
    {
        if (mComponent)
        {
            mComponent->initStyleControllers();
        }
    }

    
    std::string Element::getStyle(const std::string & inName) const
    {
        std::string result;
        if (!mComponent || !mComponent->getStyle(inName, result))
        {
            StylesMapping::const_iterator it = mStyles.find(inName);
            if (it != mStyles.end())
            {
                result = it->second;
            }
        }
        return result;

    }


    std::string Element::getAttribute(const std::string & inName) const
    {
        std::string result;
        if (!mComponent || !mComponent->getAttribute(inName, result))
        {
            AttributesMapping::const_iterator it = mAttributes.find(inName);
            if (it != mAttributes.end())
            {
                result = it->second;
            }
        }
        return result;
    }
    
    
    std::string Element::getDocumentAttribute(const std::string & inName) const
    {
        std::string result;
        AttributesMapping::const_iterator it = mAttributes.find(inName);
        if (it != mAttributes.end())
        {
            result = it->second;
        }
        return result;
    }
    
    
    void Element::setStyle(const std::string & inName, const std::string & inValue)
    {
        std::string type = this->type();
        if (!mComponent || !mComponent->setStyle(inName, inValue))
        {
            mStyles[inName] = inValue;
        }
    }
    
    
    void Element::setAttribute(const std::string & inName, const std::string & inValue)
    {
        if (!mComponent || !mComponent->setAttribute(inName, inValue))
        {
            mAttributes[inName] = inValue;
        }
    }
    
    
    bool Element::addEventListener(EventListener * inEventListener)
    {
        if (!mComponent)
        {
            return false;
        }

        if (NativeComponent * comp = mComponent->downcast<NativeComponent>())
        {
            comp->addEventListener(inEventListener);
            return true;
        }
        return false;
    }


    bool Element::removeEventListener(EventListener * inEventListener)
    {
        if (!mComponent)
        {
            return false;
        }

        if (NativeComponent * comp = mComponent->downcast<NativeComponent>())
        {
            comp->removeEventListener(inEventListener);
            return true;
        }
        return false;
    }
    
    
    Component * Element::component() const
    {
        return mComponent.get();
    }
    
    
    void Element::addChild(ElementPtr inChild)
    {
        mChildren.push_back(inChild);
        component()->onChildAdded(inChild->component());
    }


    WindowElement::WindowElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(WindowElement::Type(),
                inParent,
                new Window(inAttributesMapping))
    {
    }


    void WindowElement::show(Positioning inPositioning)
    {
        if (Window * nativeWindow = component()->downcast<Window>())
        {
            nativeWindow->show(inPositioning);
        }
    }


    void WindowElement::showModal(Positioning inPositioning)
    {
        if (Window * nativeWindow = component()->downcast<Window>())
        {
            nativeWindow->showModal(inPositioning);
        }
    }


    void WindowElement::close()
    {
        if (Window * nativeWindow = component()->downcast<Window>())
        {
            nativeWindow->close();
        }
    }


    // We want the dialog to have a parent because an owning window is needed
    // in order to prevent our dialog from showing in the Windows taskbar.
    // MSDN Article ID 205158: How To Prevent a WindowElement from Appearing on the Taskbar
    static Window * GetDialogHelper()
    {
        AttributesMapping attr;
        static Window fDialogHelper(attr);
        return &fDialogHelper;
    }


    DialogElement::DialogElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(DialogElement::Type(),
                inParent,
                new Dialog(inParent ? inParent->component() : GetDialogHelper(), inAttributesMapping))
    {
    }


    DialogResult DialogElement::showModal(WindowElement * inInvoker)
    {
        if (Dialog * nativeDialog = component()->downcast<Dialog>())
        {
            return nativeDialog->showModal(inInvoker->component()->downcast<Window>());
        }
        return DialogResult_Cancel;
    }


    void DialogElement::endModal(DialogResult inDialogResult)
    {
        if (Dialog * nativeDialog = component()->downcast<Dialog>())
        {
            nativeDialog->endModal(inDialogResult);
        }
    }


    ButtonElement::ButtonElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(ButtonElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<MarginDecorator, Button>(inParent->component(), inAttributesMapping))
    {
    }


    LabelElement::LabelElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(LabelElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<MarginDecorator, Label>(inParent->component(), inAttributesMapping))
    {
    }


    DescriptionElement::DescriptionElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(DescriptionElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<MarginDecorator, Description>(inParent->component(), inAttributesMapping))
    {
    }


    bool DescriptionElement::init()
    {
        setAttribute("value", innerText());
        return Element::init();
    }


    TextElement::TextElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TextElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<MarginDecorator, Label>(inParent->component(), inAttributesMapping))
    {
    }


    TextBoxElement::TextBoxElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TextBoxElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<MarginDecorator, TextBox>(inParent->component(), inAttributesMapping))
    {
    }


    CheckBoxElement::CheckBoxElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(CheckBoxElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<MarginDecorator, CheckBox>(inParent->component(), inAttributesMapping))
    {
    }


    BoxElement::BoxElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(BoxElement::Type(),
                inParent,
                ComponentFactory::Instance().createContainer<Decorator, VirtualBox, Box>(inParent->component(), inAttributesMapping))
    {
    }


    HBoxElement::HBoxElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(HBoxElement::Type(),
                inParent,
                ComponentFactory::Instance().createContainer<Decorator, VirtualBox, Box>(inParent->component(), inAttributesMapping))
    {
        component()->setOrient(Horizontal);
    }


    VBoxElement::VBoxElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(VBoxElement::Type(),
                inParent,
                ComponentFactory::Instance().createContainer<Decorator, VirtualBox, Box>(inParent->component(), inAttributesMapping))
    {
        component()->setOrient(Vertical);
    }


    MenuListElement::MenuListElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(MenuListElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<MarginDecorator, MenuList>(inParent->component(), inAttributesMapping))
    {
    }


    SeparatorElement::SeparatorElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(SeparatorElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<MarginDecorator, Separator>(inParent->component(), inAttributesMapping))
    {
    }


    SeparatorElement::~SeparatorElement()
    {
    }


    SpacerElement::SpacerElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(SpacerElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<Decorator, Spacer>(inParent->component(), inAttributesMapping))
    {
    }


    SpacerElement::~SpacerElement()
    {
    }


    MenuButtonElement::MenuButtonElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(MenuButtonElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<MarginDecorator, MenuButton>(inParent->component(), inAttributesMapping))
    {
    }


    MenuButtonElement::~MenuButtonElement()
    {
    }


    GridElement::GridElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(GridElement::Type(),
                inParent,
                ComponentFactory::Instance().createContainer<Decorator, VirtualGrid, Grid>(inParent->component(), inAttributesMapping))
    {
    }


    GridElement::~GridElement()
    {
    }


    RowsElement::RowsElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(RowsElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<Decorator, Rows>(inParent->component(), inAttributesMapping))
    {
    }


    RowsElement::~RowsElement()
    {
    }


    ColumnsElement::ColumnsElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(ColumnsElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<Decorator, Columns>(inParent->component(), inAttributesMapping))
    {
    }


    ColumnsElement::~ColumnsElement()
    {
    }


    RowElement::RowElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(RowElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<Decorator, Row>(inParent->component(), inAttributesMapping))
    {
    }


    RowElement::~RowElement()
    {
    }


    ColumnElement::ColumnElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(ColumnElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<Decorator, Column>(inParent->component(), inAttributesMapping))
    {
    }


    ColumnElement::~ColumnElement()
    {
    }


    RadioGroupElement::RadioGroupElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(RadioGroupElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<Decorator, RadioGroup>(inParent->component(), inAttributesMapping))
    { 
    }


    RadioGroupElement::~RadioGroupElement()
    {
    }


    RadioElement::RadioElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(RadioElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<MarginDecorator, Radio>(inParent->component(), inAttributesMapping))
    {
    }


    RadioElement::~RadioElement()
    {
    }


    ProgressMeterElement::ProgressMeterElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(ProgressMeterElement::Type(),
                inParent,
                ComponentFactory::Instance().createComponent<MarginDecorator, ProgressMeter>(inParent->component(), inAttributesMapping))
    {
    }


    ProgressMeterElement::~ProgressMeterElement()
    {
    }


    DeckElement::DeckElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(DeckElement::Type(),
                inParent,
                ComponentFactory::Instance().createContainer<Decorator, Deck, Deck>(inParent->component(), inAttributesMapping))
    {
    }


    DeckElement::~DeckElement()
    {
    }


    ScrollbarElement::ScrollbarElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(ScrollbarElement::Type(),
                inParent,
                new Decorator(new Scrollbar(inParent->component(), inAttributesMapping)))
    {
    }


    ScrollbarElement::~ScrollbarElement()
    {
    }


    TabBoxElement::TabBoxElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TabBoxElement::Type(),
                inParent,
                ComponentFactory::Instance().createContainer<Decorator, VirtualBox, Box>(inParent->component(), inAttributesMapping))
    { 
    }


    TabBoxElement::~TabBoxElement()
    {
    }


    TabsElement::TabsElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TabsElement::Type(),
                inParent,
                new Tabs(inParent->component(), inAttributesMapping))
    {
    }


    TabsElement::~TabsElement()
    {
    }


    TabElement::TabElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TabElement::Type(),
                inParent,
                new Tab(inParent->component(), inAttributesMapping))
    {
    }


    TabElement::~TabElement()
    {
    }


    TabPanelsElement::TabPanelsElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TabPanelsElement::Type(),
                inParent,
                new TabPanels(inParent->component(), inAttributesMapping))
    { 
    }


    TabPanelsElement::~TabPanelsElement()
    {
    }


    TabPanelElement::TabPanelElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TabPanelElement::Type(),
                inParent,
                new TabPanel(inParent->component(), inAttributesMapping))
    { 
    }


    TabPanelElement::~TabPanelElement()
    {
    }


    GroupBoxElement::GroupBoxElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(GroupBoxElement::Type(),
                inParent,
                new MarginDecorator(new GroupBox(inParent->component(), inAttributesMapping)))
    { 
    }


    CaptionElement::CaptionElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(CaptionElement::Type(),
                inParent,
                new Caption(inParent->component(), inAttributesMapping))
    {
    }


    TreeElement::TreeElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TreeElement::Type(),
                inParent,
                new Tree(inParent->component(), inAttributesMapping))
    {
    }


    TreeElement::~TreeElement()
    {
    }


    TreeChildrenElement::TreeChildrenElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TreeChildrenElement::Type(),
                inParent,
                new TreeChildren(inParent->component(), inAttributesMapping))
    {
    }


    TreeChildrenElement::~TreeChildrenElement()
    {
    }


    TreeItemElement::TreeItemElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TreeItemElement::Type(),
                inParent,
                new TreeItem(inParent->component(), inAttributesMapping))
    {
    }


    TreeItemElement::~TreeItemElement()
    {
    }


    TreeColsElement::TreeColsElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TreeColsElement::Type(),
                inParent,
                new TreeCols(inParent->component(), inAttributesMapping))
    {
    }


    TreeColsElement::~TreeColsElement()
    {
    }


    TreeColElement::TreeColElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TreeColElement::Type(),
                inParent,
                new TreeCol(inParent->component(), inAttributesMapping))
    {
    }


    TreeColElement::~TreeColElement()
    {
    }    
    
    
    TreeRowElement::TreeRowElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TreeRowElement::Type(),
                inParent,
                new TreeRow(inParent->component(), inAttributesMapping))
    {
    }


    TreeRowElement::~TreeRowElement()
    {
    }

    
    TreeCellElement::TreeCellElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(TreeCellElement::Type(),
                inParent,
                new TreeCell(inParent->component(), inAttributesMapping))
    {
    }


    TreeCellElement::~TreeCellElement()
    {
    }

    
    StatusbarElement::StatusbarElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(StatusbarElement::Type(),
                inParent,
                new Statusbar(inParent->component(), inAttributesMapping))
    {
    }


    StatusbarElement::~StatusbarElement()
    {
    }

    
    StatusbarPanelElement::StatusbarPanelElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(StatusbarPanelElement::Type(),
                inParent,
                new StatusbarPanel(inParent->component(), inAttributesMapping))
    {
    }


    StatusbarPanelElement::~StatusbarPanelElement()
    {
    }

    
    ToolbarElement::ToolbarElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(ToolbarElement::Type(),
                inParent,
                new Toolbar(inParent->component(), inAttributesMapping))
    {
    }


    ToolbarElement::~ToolbarElement()
    {
    }

    
    ToolbarButtonElement::ToolbarButtonElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(ToolbarButtonElement::Type(),
                inParent,
                new ToolbarButton(inParent->component(), inAttributesMapping))
    {
    }


    ToolbarButtonElement::~ToolbarButtonElement()
    {
    }


} // namespace XULWin