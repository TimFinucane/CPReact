#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <string>

#include "Constant.h"
#include "Reactive.h"
using namespace react;

TEST_CLASS( ReactiveTest )
{
    TEST_METHOD( createBinding )
    {
        Constant<int> a = 2;

        Reactive<int> copy;

        copy.bind( []( int val ){ return val; }, a );
    }

    TEST_METHOD( bindingPrematureDeletion )
    {
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

        Assert::AreEqual( 2, copy() );

        a = 5;

        Assert::AreEqual( 5, copy() );
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

    TEST_METHOD( multipleBinds )
    {
        Reactive<int> a = 2;
        Reactive<int> b = 3;

        Reactive<int> c;
        c.bind([](int first, int second) { return first + second; }, a, b);

        Assert::AreEqual(5, c());
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
            Assert::Fail(L"AssignmentToBoundException was not thrown when assigning to a bound variable");
        }
        catch ( react::AlreadyBoundException& )
        {
            Assert::AreEqual( 5, c() );
            return;
        }
    }

    TEST_METHOD( simplePrettyBind )
    {
        Constant<int> a = 3;

        Reactive<int> copy{ a * a + 4 };

        a = 4;
        Assert::AreEqual( 20, copy() );
    }

    TEST_METHOD( complexPrettyBind )
    {
        Constant a = 1.0f;
        Constant b = 1.0f;

        Reactive hypot_squared{ a * a + b * b };

        Reactive cosine{ b / Operation{ &std::sqrtf, hypot_squared } };

        a = 3.0f;
        b = 4.0f;

        Assert::AreEqual( 25.0f, hypot_squared.get() );
        Assert::AreEqual( 0.8f, cosine() ); // Should even be exact precision. How lovely.
    }
};