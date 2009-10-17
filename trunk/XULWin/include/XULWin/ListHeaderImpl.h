#ifndef LISTHEADERIMPL_H_INCLUDED
#define LISTHEADERIMPL_H_INCLUDED


#include "XULWin/ElementImpl.h"


namespace XULWin
{

    class ListHeaderImpl : public PassiveComponent,
                           public LabelController
    {
    public:
        typedef PassiveComponent Super;

        ListHeaderImpl(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool initImpl();

        bool initAttributeControllers();

        int calculateWidth(SizeConstraint inSizeConstraint) const;

        int calculateHeight(SizeConstraint inSizeConstraint) const;

        virtual std::string getLabel() const;

        virtual void setLabel(const std::string & inLabel);

    private:
        std::string mLabel;
    };

} // namespace XULWin


#endif // LISTHEADERIMPL_H_INCLUDED
