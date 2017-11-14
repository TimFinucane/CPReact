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

        copy.bind<int>( []( int val ){ return val; }, a );
    }

    TEST_METHOD( bindingPrematureDeletion )
    { // :) I made this work :)
        Observable<int>* a = new Observable<int>{ 2 };

        Reactive<int> copy;

        copy.bind<int>( []( int val ){ return val; }, *a );

        delete a;
    }
    
    TEST_METHOD( bindSimple )
    {
        Observable<int> a = 2;

        Reactive<int> copy;

        copy.bind<int>( []( int val ){ return val; }, a );

        Assert::AreEqual( 2, copy.get() );

        a = 5;

        Assert::AreEqual( 5, copy.get() );
    }    
};