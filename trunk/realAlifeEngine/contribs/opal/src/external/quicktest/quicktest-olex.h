/*************************************************************************
*                                                                       *
* QuickTest                                                             *
* Copyright (C) 2005                                                    *
* Tyler Streeter  tylerstreeter@gmail.com                               *
* All rights reserved.                                                  *
* Web: quicktest.sourceforge.net                                        *
*                                                                       *
* This library is free software; you can redistribute it and/or         *
* modify it under the terms of EITHER:                                  *
*   (1) The GNU Lesser General Public License as published by the Free  *
*       Software Foundation; either version 2.1 of the License, or (at  *
*       your option) any later version. The text of the GNU Lesser      *
*       General Public License is included with this library in the     *
*       file license-LGPL.txt.                                          *
*   (2) The BSD-style license that is included with this library in     *
*       the file license-BSD.txt.                                       *
*                                                                       *
* This library is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
* license-LGPL.txt and license-BSD.txt for more details.                *
*                                                                       *
*************************************************************************/

// Note: This is a modified version of QuickTest for OPAL.

// Credits:
// Thanks to Noel Llopis for his helpful comparison of various C++ unit
// testing frameworks and ideas for an ideal simple testing framework:
// http://www.gamesfromwithin.com/articles/0412/000061.html  Thanks to
// Michael Feathers for developing CppUnitLite.  Many of the macros in
// Quicktest were modeled after those in CppUnitLite.

#ifndef QUICK_TEST_H
#define QUICK_TEST_H

#include <string>
#include <sstream>

#include <vector>
#include <math.h>

#include <iostream>


// -----------------------------------------------------------------------
// Design Notes
// -----------------------------------------------------------------------
// * Each test automatically registers itself by accessing the TestManager
// singleton.
//
// * There are no formal fixtures.  Fixtures are simply user-defined
// objects.  setup and teardown occur in the user-defined object's
// constructor and destructor.  Tests that need fixtures should staticly
// allocate one of these objects at the beginning of the test.  This method
// is flexible and conceptually simple.

namespace quicktest
{
    typedef std::vector<std::string> TestResult;

    class Test
    {
        public:
            Test( const std::string& testGroup, const std::string& testName )
            {
                mTestName = testName;
                mTestGroup = testGroup;
            }

            virtual ~Test() {}

            const std::string & getGroup() { return mTestGroup; }

            virtual void run( TestResult& result ) = 0;

        protected:
            void recordFailure( TestResult& result, const std::string& file,
                                unsigned long int line, const std::string& message )
            {
                // If the full filename is too long, only use the last part.
                std::string fileStr = file;
                size_t maxLength = 40;
                size_t fileLength = file.size();
                if ( fileLength > maxLength )
                {
                    // Get the last maxLength characters - 3 (leave room for
                    // three ellipses at the beginning).
                    fileStr = "...";
                    fileStr += file.substr( fileLength - maxLength + 3,
                                            fileLength - 1 );
                }

                std::ostringstream oss;
                oss << fileStr << "(" << line << "): '" << mTestName
                << "' FAILED: " << message;
                result.push_back( oss.str() );
            }

            /// The unique name of this test.
            std::string mTestName;

            /// The group name of this test
            std::string mTestGroup;

			/// helper method
			static bool areEqual(double x, double y)
			{
				const double EPSILON = 0.00001;
				double maxVal = 1;

				if (fabs(static_cast<double>(x)) > maxVal)
				{
					maxVal = fabs(x);
				}

				if (fabs(static_cast<double>(y)) > maxVal)
				{
					maxVal = fabs(static_cast<double>(y));
				}

				if (fabs(static_cast<double>(x - y)) <= EPSILON * maxVal)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
    };

    class TestManager
    {
        public:
            static TestManager& instance()
            {
                static TestManager self;
                return self;
            }

            void addTest( Test* test )
            {
                mTests.push_back( test );
            }

            void setOutputStream( std::ostream* stream )
            {
                mOutputStream = stream;
            }

            std::ostream* getOutputStream()
            {
                return mOutputStream;
            }

            void runTests()
            {
                unsigned int numFailures = 0;

                *getOutputStream()
                << "[---------------- RUNNING TESTS ----------------]"
                << std::endl;

                unsigned int numLocalFailures = 0;
                unsigned int numLocalSuccesses = 0;

                std::string currentGroup;
                std::vector<Test*>::iterator iter;
                for ( iter = mTests.begin(); iter != mTests.end(); ++iter )
                {
                    if ( currentGroup != ( *iter ) ->getGroup() )
                    {
                        if ( !currentGroup.empty() )
                        {
                            *getOutputStream() << "Group results: "
                            << numLocalSuccesses << " succeeded, "
                            << numLocalFailures << " failed"
                            << std::endl;
                        }

                        currentGroup = ( *iter ) ->getGroup();

                        *getOutputStream() <<
                        "Group: " << ( *iter ) ->getGroup()
                        << std::endl;

                        numLocalFailures = 0;
                        numLocalSuccesses = 0;
                    }

                    ( *iter ) ->run( mResult );

                    bool testFailed = false;

                    size_t size = mResult.size();
                    for ( size_t i = 0; i < size; ++i )
                    {
                        *getOutputStream() << mResult.at( i ) << std::endl;
                        testFailed = true;
                    }
                    mResult.clear();

                    if ( testFailed )
                    {
                        ++numFailures;
                        ++numLocalFailures;
                    }
                    else
                    {
                        ++numLocalSuccesses;
                    }
                }

                if ( !currentGroup.empty() )
                {
                    *getOutputStream() << "Group results: "
                    << numLocalSuccesses << " succeeded, "
                    << numLocalFailures << " failed"
                    << std::endl;
                }

                *getOutputStream() << "Overall results: " << ( unsigned int ) mTests.size()
                - numFailures << " succeeded, " << numFailures << " failed"
                << std::endl;

                *getOutputStream()
                << "[---------------- TESTS FINISHED ---------------]"
                << std::endl;
            }

            ~TestManager()
            {}

        private:
            TestManager()
            {
                mOutputStream = &std::cout;
            }

            /// List of pointers to Tests.  All tests are staticly allocated,
            /// so we don't need to destroy them manually.
            std::vector<Test*> mTests;

            std::ostream* mOutputStream;

            TestResult mResult;
    };
}

/// Macro to define a single test without using a fixture.
#define QT_TEST(testName)\
    class testName##Test : public quicktest::Test\
    {\
    public:\
        testName##Test()\
        : Test(__FILE__, #testName)\
        {\
            quicktest::TestManager::instance().addTest(this);\
        }\
        void run(quicktest::TestResult& _result);\
    }testName##Instance;\
    void testName##Test::run(quicktest::TestResult& _result)


/// Macro that runs all tests.
#define QT_RUN_TESTS quicktest::TestManager::instance().runTests()



/// Macro that sets the output stream to use.
#define QT_SET_OUTPUT(stream)\
    quicktest::TestManager::instance().setOutputStream(stream)


/// Checks whether the given condition is true.
#define QT_CHECK(condition)\
    {\
        if (!(condition))\
        {\
            recordFailure(_result, __FILE__, __LINE__, #condition);\
            return;\
        }\
    }



/// Checks whether the first parameter is equal to the second.
#define QT_CHECK_EQUAL(value1, value2)\
    {\
        if ((value1) != (value2))\
        {\
            std::ostringstream oss;\
            oss << "value1:" << std::endl << "(" << (value1) << ")" \
                << std::endl << "should equal value2:" \
                << std::endl << "(" << (value2) << ")";\
            recordFailure(_result, __FILE__, __LINE__, oss.str());\
            return;\
        }\
    }


/// Checks whether the first parameter is not equal to the second.
#define QT_CHECK_NOT_EQUAL(value1, value2)\
    {\
        if ((value1) == (value2))\
        {\
            std::ostringstream oss;\
            oss << "value1 (" << (value1) << ") should not equal "\
                << "value2 (" << (value2) << ")";\
            recordFailure(_result, __FILE__, __LINE__, oss.str());\
            return;\
        }\
    }


/// Checks whether the first parameter is within the given tolerance from
/// the second parameter.  This is useful for comparing floating point
/// values.
#define QT_CHECK_CLOSE(value1, value2)\
    {\
        if (areEqual((value1),(value2)) == false )\
        {\
            std::ostringstream oss;\
            oss << "value1 (" << (value1) << ") should be close to "\
                << "value2 (" << (value2) << ")";\
            recordFailure(_result, __FILE__, __LINE__, oss.str());\
            return;\
        }\
    }

/// Checks whether the first parameter is within the given tolerance from 
/// the second parameter.  This is useful for comparing floating point 
/// values.
#define QT_CHECK_CLOSE_CUSTOM(value1, value2, tolerance)\
    {\
		if (fabs((value1)-(value2)) > tolerance)\
        {\
			std::ostringstream oss;\
			oss << "value1 (" << (value1) << ") should be close to "\
				<< "value2 (" << (value2) << ")";\
			recordFailure(_result, __FILE__, __LINE__, oss.str());\
			return;\
        }\
    }

/// Checks whether the first parameter is less than the second.
#define QT_CHECK_LESS(value1, value2)\
    {\
        if ((value1) >= (value2))\
        {\
            std::ostringstream oss;\
            oss << "value1 (" << (value1) << ") should be less than "\
                << "value2 (" << (value2) << ")";\
            recordFailure(_result, __FILE__, __LINE__, oss.str());\
            return;\
        }\
    }


/// Checks whether the first parameter is less than or equal to the second.
#define QT_CHECK_LESS_OR_EQUAL(value1, value2)\
    {\
        if ((value1) > (value2))\
        {\
            std::ostringstream oss;\
            oss << "value1 (" << (value1) << ") should be less than or "\
                << "equal to " << "value2 (" << (value2) << ")";\
            recordFailure(_result, __FILE__, __LINE__, oss.str());\
            return;\
        }\
    }

/// Checks whether the first parameter is greater than the second.
#define QT_CHECK_GREATER(value1, value2)\
    {\
        if ((value1) <= (value2))\
        {\
            std::ostringstream oss;\
            oss << "value1 (" << (value1) << ") should be greater than "\
                << "value2 (" << (value2) << ")";\
            recordFailure(_result, __FILE__, __LINE__, oss.str());\
            return;\
        }\
    }


/// Checks whether the first parameter is greater than or equal to the
/// second.
#define QT_CHECK_GREATER_OR_EQUAL(value1, value2)\
    {\
        if ((value1) < (value2))\
        {\
            std::ostringstream oss;\
            oss << "value1 (" << (value1) << ") should be greater than or "\
                << "equal to " << "value2 (" << (value2) << ")";\
            recordFailure(_result, __FILE__, __LINE__, oss.str());\
            return;\
        }\
    }

/// Fails unconditionally and prints the given message.
#define QT_FAIL(message)\
    {\
        recordFailure(_result, __FILE__, __LINE__, (message));\
        return;\
    }\

/// Prints the given message, followed by a carriage return.
#define QT_PRINT(message)\
    {\
        *(quicktest::TestManager::instance().getOutputStream()) << (message)\
            << std::endl;\
    }\

#endif
