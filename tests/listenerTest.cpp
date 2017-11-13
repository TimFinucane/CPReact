#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "events/Listener.h"
using namespace react::events;

TEST_CLASS( ListenerTest )
{
public:
    TEST_METHOD( createNormal )
    {
        Listener<> con( [](){} ); // Normal use
    }

    TEST_METHOD( createWithExitHandle )
    {
        bool closed = false;

        {
            Listener<> con( [](){}, [&](){ closed = true; } );

            Assert::IsFalse( closed );
        }

        Assert::IsTrue( closed );
    }
};