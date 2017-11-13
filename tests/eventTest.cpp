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
        bool closed = false;

        {
            EventNotifier<> notifier;

            notifier.add( [](){}, [&](){ closed = true; } );

            Assert::IsFalse( closed );
        }

        Assert::IsTrue( closed );
    }

    TEST_METHOD( removeWithCloser )
    {
        bool closed = false;

        EventNotifier<> notifier;
        auto con = notifier.add( [](){}, [&](){ closed = true; } );

        notifier.remove( con );

        Assert::IsTrue( closed );
    }

    TEST_METHOD( autoCloser )
    {
        bool closed = false;

        EventNotifier<> notifier;

        {
            AutoConnection<> con = notifier.add( [](){}, [&](){ closed = true; } );
        }

        Assert::IsTrue( closed );
    }

    TEST_METHOD( emitWithRemoves )
    {
        int a{};

        EventNotifier<> notifier;

        auto conA = notifier.add( [&](){ a += 1; } );
        notifier.add( [&](){ a += 2; } );

        notifier.emit();

        Assert::AreEqual( 3, a );

        notifier.emit();

        Assert::AreEqual( 6, a );

        notifier.remove( conA );
        notifier.emit();

        Assert::AreEqual( 8, a );
    }
};