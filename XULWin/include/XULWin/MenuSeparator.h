#ifndef MENUSEPARATOR_H_INCLUDED
#define MENUSEPARATOR_H_INCLUDED


#include "XULWin/Component.h"
#include "XULWin/Components.h"


namespace XULWin
{

    class MenuSeparator : public DummyComponent
    {
    public:
        typedef DummyComponent Super;

        MenuSeparator(Component * inParent, const AttributesMapping & inAttributesMapping);
    };

} // namespace XULWin


#endif // MENUSEPARATOR_H_INCLUDED
