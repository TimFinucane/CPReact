#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Reactive.h"
using namespace react;

TEST_CLASS( reactiveTest )
{
    TEST_METHOD( createBinding )
    {
        Observable<int> a = 2;

        Reactive<int> copy;

        copy.bind( []( int val ){ return val; }, a );
    }

    TEST_METHOD( bindingPrematureDeletion )
    { // :) I made this work :)
        Observable<int>* a = new Observable<int>{ 2 };

        Reactive<int> copy;

        copy.bind( []( int val ){ return val; }, *a );

        delete a;
        // Should not crash now when copy closes
    }

    TEST_METHOD( bindSimple )
    {
        Observable<int> a = 2;

        Reactive<int> copy;

        copy.bind( []( int val ){ return val; }, a );

        Assert::AreEqual( 2, copy.get() );

        a = 5;

        Assert::AreEqual( 5, copy.get() );
    }

    TEST_METHOD( reactiveChain )
    {
        Reactive<int> a = 2;

        Reactive<int> b;
        b.bind( []( int val ){ return val + 1; }, a );

        Reactive<int> c;
        c.bind( []( int val ){ return val + 2; }, b );

        Assert::AreEqual( 5, c.get() );

        a = 4;

        Assert::AreEqual( 7, c.get() );
    }
};