#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Toggle.h>
#include <bdn/Window.h>
#include <bdn/test/TestToggleCore.h>
#include <bdn/ios/SwitchCore.hh>

#import "TestIosViewCoreMixin.hh"

using namespace bdn;

class TestIosToggleCore : public bdn::test::TestIosViewCoreMixin<bdn::test::TestToggleCore>
{
  protected:
    void initCore() override
    {
        bdn::test::TestIosViewCoreMixin<bdn::test::TestToggleCore>::initCore();
        _switchComposite = (BdnIosSwitchComposite *)_uIView;
        REQUIRE(_switchComposite != nullptr);
    }

    void verifyCoreOn() override
    {
        bool expectedOn = _toggle->on();
        bool on = _switchComposite.uiSwitch.on;
        REQUIRE(on == expectedOn);
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _toggle->label();
        String label = bdn::ios::iosStringToString(_switchComposite.uiLabel.text);
        REQUIRE(label == expectedLabel);
    }

  protected:
    BdnIosSwitchComposite *_switchComposite;
};

TEST_CASE("ios.ToggleCore")
{
    P<TestIosToggleCore> test = newObj<TestIosToggleCore>();

    test->runTests();
}
