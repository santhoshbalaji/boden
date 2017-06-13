#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/ScrollViewLayoutHelper.h>
#include <bdn/ScrollView.h>

#include <bdn/test/MockUiProvider.h>
#include <bdn/test/ScrollViewLayoutTesterBase.h>

using namespace bdn;




class ScrollViewLayoutHelperTester : public bdn::test::ScrollViewLayoutTesterBase
{
public:
    ScrollViewLayoutHelperTester(bool horzScrollingEnabled, bool vertScrollingEnabled)
        : bdn::test::ScrollViewLayoutTesterBase(horzScrollingEnabled, vertScrollingEnabled)
        , _helper(13, 7)
    {        
        _pUiProvider = newObj<bdn::test::MockUiProvider>();
        _pWindow = newObj<Window>( _pUiProvider );

        _pScrollView = newObj<ScrollView>();

        _pScrollView->horizontalScrollingEnabled() = horzScrollingEnabled;
        _pScrollView->verticalScrollingEnabled() = vertScrollingEnabled;

        _pWindow->setContentView(_pScrollView);
    }

    P<bdn::test::MockUiProvider>    _pUiProvider;
    P<Window>                       _pWindow;
    P<ScrollView>                   _pScrollView;
    
    ScrollViewLayoutHelper          _helper;
            
    P<ScrollView> getScrollView() override
    {
        return _pScrollView;
    }

    double getVertBarWidth() override
    {
        return 13;
    }
     
    double getHorzBarHeight() override
    {
        return 7;
    }

    Size callCalcPreferredSize( const Size& availableSpace = Size::none() ) override
    {
        return _helper.calcPreferredSize( _pScrollView, availableSpace );
    }  

    void calcLayout(const Size& viewPortSize) override
    {
        _helper.calcLayout(_pScrollView, viewPortSize );
    }

    

    void verifyHorzBarVisible( bool expectedVisible) override
    {
        REQUIRE( _helper.getHorizontalScrollBarVisible() == expectedVisible );
    }

    void verifyVertBarVisible( bool expectedVisible) override
    {
        REQUIRE( _helper.getVerticalScrollBarVisible() == expectedVisible );
    }

    void verifyContentViewBounds( const Rect& expectedBounds, double maxDeviation=0) override
    {
        if(maxDeviation==0)
            REQUIRE( _helper.getContentViewBounds() == expectedBounds );
        else
        {
            Rect contentViewBounds = _helper.getContentViewBounds();

            REQUIRE_ALMOST_EQUAL(  contentViewBounds.x, expectedBounds.x, maxDeviation );
            REQUIRE_ALMOST_EQUAL(  contentViewBounds.y, expectedBounds.y, maxDeviation );
            REQUIRE_ALMOST_EQUAL(  contentViewBounds.width, expectedBounds.width, maxDeviation );
            REQUIRE_ALMOST_EQUAL(  contentViewBounds.height, expectedBounds.height, maxDeviation );
        }
    }

    void verifyScrolledAreaSize( const Size& expectedSize) override
    {
        REQUIRE( _helper.getScrolledAreaSize() == expectedSize );
    }

    void verifyViewPortSize( const Size& expectedSize) override
    {
        REQUIRE( _helper.getViewPortSize() == expectedSize );
    }               
                                
                
};



static void testScrollViewLayoutHelper(bool horzScrollingEnabled, bool vertScrollingEnabled)
{
    SECTION("scrollview null")
    {
        SECTION("preferred size")
        {
            ScrollViewLayoutHelper helper(13, 7);

            Size prefSize = helper.calcPreferredSize(nullptr, Size::none());
            REQUIRE( prefSize == Size(0,0) );
        }

        SECTION("layout")
        {
            ScrollViewLayoutHelper helper(13, 7);

            helper.calcLayout(nullptr, Size(1000, 1000) );

            REQUIRE( helper.getHorizontalScrollBarVisible() == false );
            REQUIRE( helper.getVerticalScrollBarVisible() == false );
            REQUIRE( helper.getContentViewBounds() == Rect(0, 0, 1000, 1000) );
            REQUIRE( helper.getScrolledAreaSize() == Size(1000, 1000) );
            REQUIRE( helper.getViewPortSize() == Size(1000, 1000) );
        }
    }

    SECTION("scrollview not null")
    {
        P<ScrollViewLayoutHelperTester> pTester = newObj<ScrollViewLayoutHelperTester>(horzScrollingEnabled, vertScrollingEnabled);

        pTester->doTests();
    }
}

TEST_CASE("ScrollViewLayoutHelper")
{
    SECTION("scrollable in both directions")
        testScrollViewLayoutHelper(true, true);
    SECTION("only horz scrolling")
        testScrollViewLayoutHelper(true, false);
    SECTION("only vert scrolling")
        testScrollViewLayoutHelper(false, true);
    SECTION("no scrolling")
        testScrollViewLayoutHelper(false, false);
}

