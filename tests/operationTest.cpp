#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Operation.h"
#include "Reactive.h"
using namespace react;

TEST_CLASS(OperationTest)
{
    TEST_METHOD(simpleHypotenuse)
    {
        Reactive<int> opposite = 3;
        Reactive<int> adjacent = 5;

        auto hypotenuseSquared = (opposite * opposite) + (adjacent * adjacent);

        Assert::AreEqual(34, hypotenuseSquared.get());

        opposite = 5;

        Assert::AreEqual(50, (int)hypotenuseSquared);
    }
};