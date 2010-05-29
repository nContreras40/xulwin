#include "XULWin/ListHeaderElement.h"
#include "XULWin/ListHeader.h"
#include "XULWin/Decorator.h"


namespace XULWin
{

    ListHeaderElement::ListHeaderElement(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(ListHeaderElement::TagName(),
                inParent,
                new ListHeader(inParent->component(), inAttributesMapping))
    {
    }


    bool ListHeaderElement::init()
    {
        return Element::init();
    }

} // namespace XULWin
