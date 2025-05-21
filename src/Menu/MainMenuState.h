#pragma once
/*
 * Copyright 2010-2023 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "../Engine/State.h"
#include <vector> // Required for std::vector

// Forward declarations
namespace OpenXcom {
	class TextButton;
	class Window;
	class Text;
	class InteractiveSurface;
	class Action;
}

// Utility class for enqueuing a state in the stack that goes to the main menu
class GoToMainMenuState : public OpenXcom::State // Added OpenXcom::
{
public:
	virtual void init(); // Added virtual
};

/**
 * Main Menu window displayed when first
 * starting the game.
 */
class MainMenuState : public OpenXcom::State // Added OpenXcom::
{
private:
	OpenXcom::TextButton *_btnNewGame, *_btnNewBattle, *_btnLoad, *_btnOptions, *_btnMods, *_btnQuit;
	OpenXcom::Window *_window;
	OpenXcom::Text *_txtTitle;

	std::vector<OpenXcom::InteractiveSurface*> _navigableControls;
	OpenXcom::InteractiveSurface* _focusedControl;
	int _focusedIndex; 

	void setFocusedControlVisuals(OpenXcom::InteractiveSurface* control, bool focused);
	void setFocusOn(OpenXcom::InteractiveSurface* control);
	void cycleFocus(bool forward);

public:
	MainMenuState();
	~MainMenuState();
    virtual void init(); 
	virtual void handle(OpenXcom::Action *action); 
	void btnNewGameClick(OpenXcom::Action *action);
	void btnNewBattleClick(OpenXcom::Action *action);
	void btnLoadClick(OpenXcom::Action *action);
	void btnOptionsClick(OpenXcom::Action *action);
	void btnModsClick(OpenXcom::Action *action);
	void btnQuitClick(OpenXcom::Action *action);
	virtual void resize(int &dX, int &dY); // Added virtual
};
