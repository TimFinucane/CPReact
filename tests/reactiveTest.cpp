#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Constant.h"
#include "Reactive.h"
using namespace react;

TEST_CLASS( reactiveTest )
{
    TEST_METHOD( createBinding )
    {
        Constant<int> a = 2;

        Reactive<int> copy;

        copy.bind( []( int val ){ return val; }, a );
    }

    TEST_METHOD( bindingPrematureDeletion )
    { // :) I made this work :)
        Observable<int>* a = new Constant<int>{ 2 };

        Reactive<int> copy;

        copy.bind( []( int val ){ return val; }, *a );

        delete a;
        // Should not crash now when copy closes
    }

    TEST_METHOD( bindSimple )
    {
        Constant<int> a = 2;

        Reactive<int> copy;

        copy.bind( []( int val ){ return val; }, a );

        Assert::AreEqual( 2, (int)copy );

        a = 5;

        Assert::AreEqual( 5, (int)copy );
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

    TEST_METHOD( unbinding )
    {
        Reactive<int>* a = new Reactive<int>(2);

        Reactive<int> b;
        b.bind( []( int val ){ return val + 1; }, *a );

        Reactive<int> c;
        c.bind( []( int val ){ return val + 2; }, b );

        Assert::AreEqual( 5, c.get() );

        *a = 4;

        // When a is deleted, b should recognize this and update to the latest value before severing connection
        delete a;

        // Therefore even though a is gone at this point, c will be the correct expected value
        Assert::AreEqual( 7, c.get() );
    }

    TEST_METHOD( tryChangeBound )
    {
        Reactive<int> a = 2;
        Reactive<int> b = 3;

        Reactive<int> c;
        c.bind( []( int first, int second ) { return first + second; }, a, b );

        try
        {
            c = 4;
        }
        catch ( react::AlreadyBoundException& )
        {
            Assert::AreEqual( 5, (int)c );
            return;
        }
        Assert::Fail( L"AssignmentToBoundException was not thrown when assigning to a bound variable" );
    }

    TEST_METHOD( simplePrettyBind )
    {
        Constant<int> a = 2;

        Reactive<int> copy = a;

        a = 4;
        Assert::AreEqual( 4, (int)copy );
    }

    /*
     * This test method highlights the simplified language that react should support
     *//*
    TEST_METHOD( simpleHypotEquation )
    {
        Reactive<int> opposite = 2;
        Reactive<int> adjacent = 5;

        // Auto should default to reactive
        auto hypotenuseSq = (opposite * opposite) + (adjacent * adjacent);

        opposite += 3;

        Assert::AreEqual( 50, hypotenuseSq );
    }*/
};