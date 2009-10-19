#include "XULWin/ScriptElement.h"
#include "XULWin/AttributeController.h"
#include "XULWin/Decorator.h"


namespace XULWin
{

    class NativeScript : public PassiveComponent
    {
    public:
        typedef PassiveComponent Super;

        NativeScript(Component * inParent, const AttributesMapping & inAttributesMapping);
    };


    ScriptElement::ScriptElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(ScriptElement::Type(),
                inParent,
                new NativeScript(inParent->component(), inAttributesMapping))
    {
    }


    bool ScriptElement::init()
    {
        setAttribute("value", innerText());
        return Element::init();
    }

    
    NativeScript::NativeScript(Component * inParent, const AttributesMapping & inAttributesMapping) :
        PassiveComponent(inParent, inAttributesMapping)
    {
    }

} // namespace XULWin
