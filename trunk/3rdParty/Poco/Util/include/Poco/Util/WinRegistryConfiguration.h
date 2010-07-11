//
// WinRegistryConfiguration.h
//
// $Id: //poco/1.3/Util/include/Poco/Util/WinRegistryConfiguration.h#1 $
//
// Library: Util
// Package: Windows
// Module:  WinRegistryConfiguration
//
// Definition of the WinRegistryConfiguration class.
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
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


#ifndef Util_WinRegistryConfiguration_INCLUDED
#define Util_WinRegistryConfiguration_INCLUDED


#include "Poco/Util/Util.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/String.h"


namespace Poco {
namespace Util {


class Util_API WinRegistryConfiguration: public AbstractConfiguration
	/// An implementation of AbstractConfiguration that stores configuration data
	/// in the Windows registry.
{
public:
	WinRegistryConfiguration(const std::string& rootPath);
		/// Creates the WinRegistryConfiguration. 
		/// The rootPath must start with one of the root key names
		/// like HKEY_CLASSES_ROOT, e.g. HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services.
		/// All further keys are relativ to the root path and can be
		/// dot seperated, e.g. the path MyService.ServiceName will be converted to
		/// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\MyService\ServiceName.

	bool getRaw(const std::string& key, std::string& value) const;
		/// If the property with the given key exists, stores the property's value
		/// in value and returns true. Otherwise, returns false.
		///
		/// Must be overridden by subclasses.

	void setRaw(const std::string& key, const std::string& value);
		/// Sets the property with the given key to the given value.
		/// An already existing value for the key is overwritten.
		///
		/// Must be overridden by subclasses.
		
	void enumerate(const std::string& key, Keys& range) const;
		/// Returns in range the names of all subkeys under the given key.
		/// If an empty key is passed, all root level keys are returned.

protected:
	~WinRegistryConfiguration();
		/// Destroys the WinRegistryConfiguration.

	std::string ConvertToRegFormat(const std::string& key, std::string& keyName) const;
		/// takes a key in the format of A.B.C and converts it to
		/// registry format A\B\C, the last entry is the keyName, the rest is returned as path

private:
	std::string _rootPath;
};


} } // namespace Poco::Util


#endif // Util_WinRegistryConfiguration_INCLUDED