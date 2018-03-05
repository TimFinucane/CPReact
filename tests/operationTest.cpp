#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <cmath>

#include "bindings/Operation.h"
#include "Constant.h"
using namespace react;

TEST_CLASS( OperationTest )
{
    TEST_METHOD( simpleHypotenuse )
    {
        Constant<int> opposite = 3;
        Constant<int> adjacent = 5;

        auto hypotenuseSquared = (opposite * opposite) + (adjacent * adjacent);

        Assert::AreEqual( 34, hypotenuseSquared.get() );

        opposite = 5;

        Assert::AreEqual( 50, hypotenuseSquared() );
    }

    TEST_METHOD( simpleExternalFunction )
    {
        Constant<float> angle = 3.14159f;

        auto ratio = make_operation( std::sinf, angle );

        Assert::AreEqual( 0.0f, ratio(), 0.1f );

        angle = 3.14159f / 2.0f;

        Assert::AreEqual( 1.0f, ratio(), 0.1f );
    }
};