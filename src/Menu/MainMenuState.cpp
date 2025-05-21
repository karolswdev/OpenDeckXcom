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
#include "MainMenuState.h"
#include <sstream>
#include "../version.h"
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Screen.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "NewGameState.h"
#include "NewBattleState.h"
#include "ListLoadState.h"
#include "OptionsVideoState.h" // For OptionsOrigin::OPT_MENU
#include "ModListState.h"
#include "../Engine/Options.h" // For Options::keyMenuUp, etc.
#include "../Engine/Action.h"
#include "../Interface/InteractiveSurface.h"


namespace OpenXcom
{

void GoToMainMenuState::init()
{
	if (_game && _game->getScreen()) { // Null check
        Screen::updateScale(Options::geoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, true);
	    _game->getScreen()->resetDisplay(false);
    }
	if (_game) _game->setState(new MainMenuState); // Null check
}

/**
 * Initializes all the elements in the Main Menu window.
 * @param game Pointer to the core game.
 */
MainMenuState::MainMenuState() : _focusedControl(nullptr), _focusedIndex(-1)
{
	// Create objects
	_window = new Window(this, 256, 160, 32, 20, POPUP_BOTH);
	_btnNewGame = new TextButton(92, 20, 64, 90);
	_btnNewBattle = new TextButton(92, 20, 164, 90);
	_btnLoad = new TextButton(92, 20, 64, 118);
	_btnOptions = new TextButton(92, 20, 164, 118);
	_btnMods = new TextButton(92, 20, 64, 146);
	_btnQuit = new TextButton(92, 20, 164, 146);
	_txtTitle = new Text(256, 30, 32, 45);

	// Set palette
	setInterface("mainMenu");

	add(_window, "window", "mainMenu");
	add(_btnNewGame, "button", "mainMenu");
	add(_btnNewBattle, "button", "mainMenu");
	add(_btnLoad, "button", "mainMenu");
	add(_btnOptions, "button", "mainMenu");
	add(_btnMods, "button", "mainMenu");
	add(_btnQuit, "button", "mainMenu");
	add(_txtTitle, "text", "mainMenu");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_btnNewGame->setText(tr("STR_NEW_GAME"));
	_btnNewGame->onMouseClick((ActionHandler)&MainMenuState::btnNewGameClick);

	_btnNewBattle->setText(tr("STR_NEW_BATTLE"));
	_btnNewBattle->onMouseClick((ActionHandler)&MainMenuState::btnNewBattleClick);

	_btnLoad->setText(tr("STR_LOAD_SAVED_GAME"));
	_btnLoad->onMouseClick((ActionHandler)&MainMenuState::btnLoadClick);

	_btnOptions->setText(tr("STR_OPTIONS"));
	_btnOptions->onMouseClick((ActionHandler)&MainMenuState::btnOptionsClick);

	_btnMods->setText(tr("STR_MODS"));
	_btnMods->onMouseClick((ActionHandler)&MainMenuState::btnModsClick);

	_btnQuit->setText(tr("STR_QUIT"));
	_btnQuit->onMouseClick((ActionHandler)&MainMenuState::btnQuitClick);

	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	std::ostringstream title;
	title << tr("STR_OPENXCOM") << Unicode::TOK_NL_SMALL;
	title << OPENXCOM_VERSION_SHORT << OPENXCOM_VERSION_GIT;
	_txtTitle->setText(title.str());

	// Populate navigable controls (simple top-to-bottom, left-to-right order)
	_navigableControls.push_back(_btnNewGame);
	_navigableControls.push_back(_btnNewBattle);
	_navigableControls.push_back(_btnLoad);
	_navigableControls.push_back(_btnOptions);
	_navigableControls.push_back(_btnMods);
	_navigableControls.push_back(_btnQuit);
	
    // Initial focus will be set in init()
}

MainMenuState::~MainMenuState()
{
	// _navigableControls only holds pointers, buttons are deleted by State destructor
}

void MainMenuState::init()
{
    State::init(); // Call base class init if it exists and does something useful

    _focusedIndex = -1; // Reset focus index
    _focusedControl = nullptr; // Reset focused control

    // Set initial focus to the first visible and enabled control
    if (!_navigableControls.empty()) // Ensure vector is not empty
    {
        for (size_t i = 0; i < _navigableControls.size(); ++i)
        {
            if (_navigableControls[i] && _navigableControls[i]->getVisible() && _navigableControls[i]->getEnabled())
            {
                setFocusOn(_navigableControls[i]);
                break;
            }
        }
    }
}

void MainMenuState::setFocusedControlVisuals(InteractiveSurface* control, bool focused)
{
    if (!control) return;

    TextButton* tb = dynamic_cast<TextButton*>(control);
    if (tb)
    {
        tb->setPressed(focused); 
    }    
    control->setFocus(focused); 
}

void MainMenuState::setFocusOn(InteractiveSurface* control)
{
    if (_focusedControl && _focusedControl != control)
    {
        setFocusedControlVisuals(_focusedControl, false);
    }
    
    _focusedControl = control;
    _focusedIndex = -1; 
    if (_focusedControl)
    {
        for(size_t i = 0; i < _navigableControls.size(); ++i)
        {
            if (_navigableControls[i] == _focusedControl)
            {
                _focusedIndex = i;
                break;
            }
        }
        setFocusedControlVisuals(_focusedControl, true);
    }
}

void MainMenuState::cycleFocus(bool forward)
{
    if (_navigableControls.empty()) return;

    int startIndex = _focusedIndex; // Keep current index if valid
    if (_focusedIndex == -1) { // If no focus, determine starting point based on direction
        startIndex = forward ? (_navigableControls.size() - 1) : 0;
    }
    
    int newIndex = startIndex;
    int attempts = 0; 

    do {
        if (forward)
        {
            newIndex = (newIndex + 1) % _navigableControls.size();
        }
        else
        {
            newIndex = (newIndex - 1 + _navigableControls.size()) % _navigableControls.size();
        }
        attempts++;
    } while ((!_navigableControls[newIndex] || 
              !_navigableControls[newIndex]->getVisible() || 
              !_navigableControls[newIndex]->getEnabled()) && 
             attempts < (int)_navigableControls.size() * 2); 

    if (_navigableControls[newIndex] && _navigableControls[newIndex]->getVisible() && _navigableControls[newIndex]->getEnabled()) {
        setFocusOn(_navigableControls[newIndex]);
    } else if (startIndex != -1 && _navigableControls[startIndex] && _navigableControls[startIndex]->getVisible() && _navigableControls[startIndex]->getEnabled()) {
        // Fallback to current/start index if no other focusable control is found
        setFocusOn(_navigableControls[startIndex]);
    } else {
        // If still no focusable control, clear focus
        if(_focusedControl) setFocusedControlVisuals(_focusedControl, false);
        _focusedControl = nullptr;
        _focusedIndex = -1;
    }
}

void MainMenuState::handle(Action *action)
{
	bool handled = false;
	if (action->getDetails()->type == SDL_KEYDOWN)
	{
		SDLKey sym = action->getDetails()->key.keysym.sym;

		if (!_navigableControls.empty()) {
            // Ensure something is focused if starting with no focus
            if (_focusedIndex == -1 && !_navigableControls.empty()) { 
                 for(size_t i=0; i < _navigableControls.size(); ++i) {
                     if(_navigableControls[i] && _navigableControls[i]->getVisible() && _navigableControls[i]->getEnabled()){
                         setFocusOn(_navigableControls[i]);
                         break;
                     }
                 }
            }

			if (sym == Options::keyMenuUp) { cycleFocus(false); handled = true; }
			else if (sym == Options::keyMenuDown) { cycleFocus(true); handled = true; }
			// Left/Right navigation removed as Options::keyMenuLeft/Right (or Options::keyLeft/Right) are not defined options
			else if (sym == Options::keyMenuSelect) {
				if (_focusedControl) {
					SDL_Event sdlEvent; sdlEvent.type = SDL_MOUSEBUTTONDOWN; sdlEvent.button.button = SDL_BUTTON_LEFT; sdlEvent.button.state = SDL_PRESSED;
					sdlEvent.button.x = 0; sdlEvent.button.y = 0;
					Action clickAction(&sdlEvent, 
                                       _game->getScreen()->getXScale(), 
                                       _game->getScreen()->getYScale(), 
                                       _game->getScreen()->getCursorTopBlackBand(), 
                                       _game->getScreen()->getCursorLeftBlackBand());
					clickAction.setSender(_focusedControl); 
                    TextButton* tb = dynamic_cast<TextButton*>(_focusedControl);
                    if (tb) { 
                        if (tb == _btnNewGame) btnNewGameClick(&clickAction);
                        else if (tb == _btnNewBattle) btnNewBattleClick(&clickAction);
                        else if (tb == _btnLoad) btnLoadClick(&clickAction);
                        else if (tb == _btnOptions) btnOptionsClick(&clickAction); 
                        else if (tb == _btnMods) btnModsClick(&clickAction);     
                        else if (tb == _btnQuit) btnQuitClick(&clickAction);
                    }    
				}
				handled = true;
			}
		}
		if (sym == Options::keyMenuCancel) {
			SDL_Event sdlEvent; sdlEvent.type = SDL_MOUSEBUTTONDOWN; Action quitAction(&sdlEvent, 1.0,1.0,0,0); 
			quitAction.setSender(_btnQuit); 
			btnQuitClick(&quitAction);
			handled = true;
		}
	}
	if (!handled) State::handle(action); 
}

void MainMenuState::btnNewGameClick(Action *) { if(_game) _game->pushState(new NewGameState); } // Null check
void MainMenuState::btnNewBattleClick(Action *) { if(_game) _game->pushState(new NewBattleState); } // Null check
void MainMenuState::btnLoadClick(Action *) { if(_game) _game->pushState(new ListLoadState(OPT_MENU)); } // Null check
void MainMenuState::btnOptionsClick(Action *) { Options::backupDisplay(); if(_game) _game->pushState(new OptionsVideoState(OPT_MENU)); } // Null check
void MainMenuState::btnModsClick(Action *) { if(_game) _game->pushState(new ModListState); } // Null check
void MainMenuState::btnQuitClick(Action *) { if(_game) _game->quit(); } // Null check

void MainMenuState::resize(int &dX, int &dY) {
	dX = Options::baseXResolution; dY = Options::baseYResolution;
	if (_game && _game->getScreen()) { // Null checks
		Screen::updateScale(Options::geoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, true);
		dX = Options::baseXResolution - dX; dY = Options::baseYResolution - dY;
	} else { dX = 0; dY = 0; }
	State::resize(dX, dY);
}

}
