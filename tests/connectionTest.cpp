#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "events/Connection.h"
using namespace react::events;

TEST_CLASS( ConnectionTest )
{
public:
    TEST_METHOD( createNormal )
    {
        Connection<> con( [](){} ); // Normal use
    }

    TEST_METHOD( createWithExitHandle )
    {
        bool closed = false;

        {
            Connection<> con( [](){}, [&](){ closed = true; } );

            Assert::IsFalse( closed );
        }

        Assert::IsTrue( closed );
    }
};