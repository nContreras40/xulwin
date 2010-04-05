//
// DOMImplementation.h
//
// $Id: //poco/1.3/XML/include/Poco/DOM/DOMImplementation.h#1 $
//
// Library: XML
// Package: DOM
// Module:  DOM
//
// Definition of the DOM DOMImplementation class.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef DOM_DOMImplementation_INCLUDED
#define DOM_DOMImplementation_INCLUDED


#include "Poco/XML/XML.h"
#include "Poco/XML/XMLString.h"


namespace Poco {
namespace XML {


class DocumentType;
class Document;
class NamePool;


class XML_API DOMImplementation
	/// The DOMImplementation interface provides a number of methods for
	/// performing operations that are independent of any particular instance
	/// of the document object model.
	/// In this implementation, DOMImplementation is implemented as a singleton.
{
public:
	DOMImplementation();
		/// Creates the DOMImplementation.
		
	~DOMImplementation();
		/// Destroys the DOMImplementation.

	bool hasFeature(const XMLString& feature, const XMLString& version) const;
		/// Tests if the DOM implementation implements a specific feature.
		///
		/// The only supported features are "XML", version "1.0" and "Core", 
		/// "Events", "MutationEvents" and "Traversal", version "2.0".
	
	// DOM Level 2
	DocumentType* createDocumentType(const XMLString& name, const XMLString& publicId, const XMLString& systemId) const;
		/// Creates an empty DocumentType node. Entity declarations and notations 
		/// are not made available. Entity reference expansions and default attribute 
		/// additions do not occur.

	Document* createDocument(const XMLString& namespaceURI, const XMLString& qualifiedName, DocumentType* doctype) const;
		/// Creates an XML Document object of the specified type with its document element.
		///
		/// Note: You can also create a Document directly using the new operator.

	static const DOMImplementation& instance();
		/// Returns a reference to the default DOMImplementation
		/// object.
		
private:
	static const XMLString FEATURE_XML;
	static const XMLString FEATURE_CORE;
	static const XMLString FEATURE_EVENTS;
	static const XMLString FEATURE_MUTATIONEVENTS;
	static const XMLString FEATURE_TRAVERSAL;
	static const XMLString VERSION_1_0;
	static const XMLString VERSION_2_0;	
};


} } // namespace Poco::XML


#endif // DOM_DOMImplementation_INCLUDED
