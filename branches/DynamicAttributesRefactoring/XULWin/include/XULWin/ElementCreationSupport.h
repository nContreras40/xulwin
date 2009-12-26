#ifndef ELEMENTCREATIONSUPPORT_H_INCLUDED
#define ELEMENTCREATIONSUPPORT_H_INCLUDED


#include "XULWin/Component.h"
#include "XULWin/ToolbarCustomWindowDecorator.h"
#include "Poco/StringTokenizer.h"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>


namespace XULWin
{    

    template<class ControlType>
    static Component * CreateControl(Element * inParent, const AttributesMapping & inAttributesMapping)
    {
        if (!inParent)
        {
            return new ControlType(0, inAttributesMapping);
        }
        
        if (Toolbar * toolbar = inParent->component()->downcast<Toolbar>())
        {
                ControlType * control = new ControlType(inParent->component(), inAttributesMapping);
                boost::weak_ptr<Windows::ToolbarElement> weakToolbar(toolbar->nativeToolbar());
                return new ToolbarCustomWindowDecorator(control, weakToolbar);
        }
        else
        {
            return new ControlType(inParent->component(), inAttributesMapping);
        }
    }


    static void GetStyles(const AttributesMapping & inAttributesMapping, StylesMapping & styles)
    {        
        StylesMapping::const_iterator it = inAttributesMapping.find("style");
        if (it != inAttributesMapping.end())
        {
            Poco::StringTokenizer tok(it->second, ";:", Poco::StringTokenizer::TOK_IGNORE_EMPTY | Poco::StringTokenizer::TOK_TRIM);
            Poco::StringTokenizer::Iterator it = tok.begin(), end = tok.end();
            std::string key, value;
            int counter = 0;
            for (; it != end; ++it)
            {
                if (counter%2 == 0)
                {
                    key = *it;
                }
                else
                {
                    value = *it;
                    styles.insert(std::make_pair(key, value));
                }
                counter++;
            }
        }
    }


    static CSSOverflow GetOverflow(const StylesMapping & inStyles,
                                   const std::string & inOverflow)
    {
        StylesMapping::const_iterator it = inStyles.find(inOverflow);
        if (it == inStyles.end())
        {
            it = inStyles.find("overflow");
        }
        if (it != inStyles.end())
        {
            return String2CSSOverflow(it->second, CSSOverflow_Visible);
        }
        return CSSOverflow_Hidden;
    }


    template<class VirtualType, class Type>
    static Component * CreateContainer(Element * inParent, const AttributesMapping & inAttributesMapping)
    {
        StylesMapping styles;
        GetStyles(inAttributesMapping, styles);
        CSSOverflow overflowX = GetOverflow(styles, "overflow-x");
        CSSOverflow overflowY = GetOverflow(styles, "overflow-y");
        if (overflowX != CSSOverflow_Hidden || overflowY != CSSOverflow_Hidden)
        {
            Component * box = CreateControl<Type>(inParent, inAttributesMapping);
            return new ScrollDecorator(inParent->component(), box, overflowX, overflowY);
        }
        else
        {
            return new Decorator(CreateControl<VirtualType>(inParent, inAttributesMapping));
        }
    }
}


#endif //  ELEMENTCREATIONSUPPORT_H_INCLUDED