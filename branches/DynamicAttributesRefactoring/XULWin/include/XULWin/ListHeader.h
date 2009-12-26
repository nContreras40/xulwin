#ifndef LISTHEADERIMPL_H_INCLUDED
#define LISTHEADERIMPL_H_INCLUDED


#include "XULWin/Component.h"


namespace XULWin
{

    class ListHeader : public PassiveComponent,
                       public LabelController
    {
    public:
        typedef PassiveComponent Super;

        ListHeader(Component * inParent, const AttributesMapping & inAttributesMapping);

        virtual bool init();

        bool initAttributeControllers();

        virtual std::string getLabel() const;

        virtual void setLabel(const std::string & inLabel);

    private:
        std::string mLabel;
    };

} // namespace XULWin


#endif // LISTHEADERIMPL_H_INCLUDED