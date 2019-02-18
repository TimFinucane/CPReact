#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "events/Event.h"
using namespace react::events;

TEST_CLASS( EventTest )
{
public:
    TEST_METHOD( addNormal )
    {
        EventNotifier<> notifier;

        notifier.add( [](){} );
    }

    TEST_METHOD( addWithCloser )
    {
        bool severed = false;

        {
            EventNotifier<> notifier;

            notifier.add( [](){}, [&](){ severed = true; } );

            Assert::IsFalse( severed );
        }

        Assert::IsTrue( severed );
    }

    TEST_METHOD( removeWithCloser )
    {
        bool severed = false;
        {
            EventNotifier<> notifier;
            auto con = notifier.add([]() {}, [&]() { severed = true; });

            con.close();
        }
        Assert::IsFalse( severed );
    }

    TEST_METHOD( autoCloser )
    {
        bool severed = false;

        {
            EventNotifier<> notifier;
            {
                AutoConnection<> con = notifier.add([]() {}, [&]() { severed = true; });
            }
        }

        Assert::IsFalse(severed);
    }

    TEST_METHOD( notifyWithRemoves )
    {
        int a{};

        EventNotifier<> notifier;

        auto conA = notifier.add( [&](){ a += 1; } );
        notifier.add( [&](){ a += 2; } );

        notifier.notify();

        Assert::AreEqual( 3, a );

        notifier.notify();

        Assert::AreEqual( 6, a );

        conA.close();
        notifier.notify();

        Assert::AreEqual( 8, a );
    }
};