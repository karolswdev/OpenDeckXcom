#pragma once

namespace OpenXcom
{

// Forward declarations if needed
class Game;
class State;
class Action;
class Slider; // Forward declare Slider for the handler
class TextList; // Forward declare TextList for the handler


class TestKeyboardNavigation
{
public:
    static bool RunTests();

private:
    static bool TestTextListNavigation();
    static bool TestSliderNavigation();
    // Placeholder for a menu state test
    static bool TestMainMenuStateNavigation(); 
    static bool TestKeyRebinding();


    // Helper to simulate a key press action
    static void SimulateKeyPress(SDLKey key, State* targetState);
    // Helper to simulate a key press action for components not in a state
    static Action SimulateKeyPressAction(SDLKey key);


    // Mock game instance or provide access to the main one if safe for tests
    static Game* _testGame; 
    static bool _textListClickHandlerFlag;
    static int _sliderChangeHandlerValue;
    static bool _sliderChangeHandlerFlag;
    static bool _mainMenuButtonFlag; // For MainMenuState test

    // Test handlers
    static void TextListTestClickHandler(Action* action);
    static void SliderTestChangeHandler(Action* action);
    static void MainMenuButtonTestClickHandler(Action* action);


    // Assertion helpers
    template<typename T>
    static bool AssertEquals(T expected, T actual, const std::string& message);
    static bool AssertTrue(bool condition, const std::string& message);

};

} // namespace OpenXcom
