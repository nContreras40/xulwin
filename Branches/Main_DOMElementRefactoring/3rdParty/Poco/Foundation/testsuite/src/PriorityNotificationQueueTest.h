//
// PriorityNotificationQueueTest.h
//
// $Id: //poco/1.3/Foundation/testsuite/src/PriorityNotificationQueueTest.h#1 $
//
// Definition of the PriorityNotificationQueueTest class.
//
// Copyright (c) 2009, Applied Informatics Software Engineering GmbH.
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


#ifndef PriorityNotificationQueueTest_INCLUDED
#define PriorityNotificationQueueTest_INCLUDED


#include "Poco/Foundation.h"
#include "CppUnit/TestCase.h"
#include "Poco/PriorityNotificationQueue.h"
#include "Poco/Mutex.h"
#include <set>


class PriorityNotificationQueueTest: public CppUnit::TestCase
{
public:
	PriorityNotificationQueueTest(const std::string& name);
	~PriorityNotificationQueueTest();

	void testQueueDequeue();
	void testWaitDequeue();
	void testThreads();
	void testDefaultQueue();

	void setUp();
	void tearDown();

	static CppUnit::Test* suite();

protected:
	void work();

private:
	Poco::PriorityNotificationQueue    _queue;
	std::multiset<std::string> _handled;
	Poco::FastMutex            _mutex;
};


#endif // PriorityNotificationQueueTest_INCLUDED
