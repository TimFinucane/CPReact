#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Observable.h"
using namespace react;

TEST_CLASS( ObservableTest )
{
public:
    TEST_METHOD( creation )
    {
        Observable<int> a = 3;
        Observable<int> b{ 4 };
    }

    TEST_METHOD( setAndGet )
    {
        Observable<int> a = 3;

        a.set( 5 );

        Assert::AreEqual( 5, a.get() );
    }

    TEST_METHOD( observeChange )
    {
        bool changed = 0;

        Observable<int> a = 3;

        a.addListener( [&](){ changed = true; } );

        Assert::IsFalse( changed );

        a = 4;

        Assert::IsTrue( changed );
    }

    TEST_METHOD( observeValue )
    {
        int val = 0;
        int times = 0;

        Observable<int> a = 3;

        a.addListener( 
            [&]( int prev, int next )
            { 
                val += (next - prev); times++; 
            } );

        Assert::AreEqual( 0, val );

        a = 5;

        Assert::AreEqual( 2, val );

        a = 3;

        Assert::AreEqual( 0, val );
        Assert::AreEqual( 2, times );
    }

    TEST_METHOD( observeValueMultipleWithRemoval )
    {
        int val = 0;

        Observable<int> a = 3;

        auto aCon = a.addListener( [&]( int prev, int next ){ val += 1; } );
        a.addListener( [&]( int prev, int next ){ val += 2; } );
        
        a = 5;

        Assert::AreEqual( 3, val );

        a.removeListener( aCon );

        a = 3;

        Assert::AreEqual( 5, val );
    }
};