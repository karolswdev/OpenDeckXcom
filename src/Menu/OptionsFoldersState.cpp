/*
 * Copyright 2010-2019 OpenXcom Developers.
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
#include "OptionsFoldersState.h"
#include "../Engine/Action.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/FileMap.h"
#include "../Engine/Options.h"
#include "../Interface/Text.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Folders Options screen.
 * @param origin Game section that originated this state.
 */
OptionsFoldersState::OptionsFoldersState(OptionsOrigin origin) : OptionsBaseState(origin)
{
	setCategory(_btnFolders);

	// Create object
	_txtDataFolder = new Text(218, 9, 94, 8);
	_txtUserFolder = new Text(218, 9, 94, 59);
	_txtSaveFolder = new Text(218, 9, 94, 86);
	_txtConfigFolder = new Text(218, 9, 94, 121);

	_txtDataFolderPath1 = new Text(218, 17, 94, 18);
	_txtDataFolderPath2 = new Text(218, 17, 94, 38);
	_txtUserFolderPath = new Text(218, 17, 94, 69);
	_txtSaveFolderPath = new Text(218, 25, 94, 96);
	_txtConfigFolderPath = new Text(218, 17, 94, 131);

	add(_txtDataFolder, "text1", "foldersMenu");
	add(_txtUserFolder, "text1", "foldersMenu");
	add(_txtSaveFolder, "text1", "foldersMenu");
	add(_txtConfigFolder, "text1", "foldersMenu");

	add(_txtDataFolderPath1, "text2", "foldersMenu");
	add(_txtDataFolderPath2, "text2", "foldersMenu");
	add(_txtUserFolderPath, "text2", "foldersMenu");
	add(_txtSaveFolderPath, "text2", "foldersMenu");
	add(_txtConfigFolderPath, "text2", "foldersMenu");

	centerAllSurfaces();

	// Set up object
	_txtDataFolder->setText(tr("STR_DATA_FOLDER"));
	_txtUserFolder->setText(tr("STR_USER_FOLDER"));
	_txtSaveFolder->setText(tr("STR_SAVE_FOLDER"));
	_txtConfigFolder->setText(tr("STR_CONFIG_FOLDER"));

	std::string file1 = "GEODATA/BACKPALS.DAT";
	std::string file2 = "openxcom.png";

	std::string origDataFolder = FileMap::getFilePath(file1); // OXC-specific: we are assuming it exists! otherwise `getFilePath` returns a dummy result.
	origDataFolder = origDataFolder.substr(0, origDataFolder.find(file1));

	std::string oxceDataFolder = FileMap::getFilePath(file2); // OXC-specific: we are assuming it exists! otherwise `getFilePath` returns a dummy result.
	oxceDataFolder = oxceDataFolder.substr(0, oxceDataFolder.find(file2));

	_txtDataFolderPath1->setText(origDataFolder);
	_txtDataFolderPath1->setWordWrap(true);
	_txtDataFolderPath1->setTooltip("STR_DATA_FOLDER_DESC_1");
	_txtDataFolderPath1->onMouseIn((ActionHandler)&OptionsFoldersState::txtTooltipIn);
	_txtDataFolderPath1->onMouseOut((ActionHandler)&OptionsFoldersState::txtTooltipOut);
	_txtDataFolderPath1->onMouseClick((ActionHandler)&OptionsFoldersState::txtClick);

	_txtDataFolderPath2->setText(oxceDataFolder);
	_txtDataFolderPath2->setWordWrap(true);
	_txtDataFolderPath2->setTooltip("STR_DATA_FOLDER_DESC_2");
	_txtDataFolderPath2->onMouseIn((ActionHandler)&OptionsFoldersState::txtTooltipIn);
	_txtDataFolderPath2->onMouseOut((ActionHandler)&OptionsFoldersState::txtTooltipOut);
	_txtDataFolderPath2->onMouseClick((ActionHandler)&OptionsFoldersState::txtClick);

	_txtUserFolderPath->setText(Options::getUserFolder());
	_txtUserFolderPath->setWordWrap(true);
	_txtUserFolderPath->setTooltip("STR_USER_FOLDER_DESC");
	_txtUserFolderPath->onMouseIn((ActionHandler)&OptionsFoldersState::txtTooltipIn);
	_txtUserFolderPath->onMouseOut((ActionHandler)&OptionsFoldersState::txtTooltipOut);
	_txtUserFolderPath->onMouseClick((ActionHandler)&OptionsFoldersState::txtClick);

	_txtSaveFolderPath->setText(Options::getMasterUserFolder());
	_txtSaveFolderPath->setWordWrap(true);
	_txtSaveFolderPath->setTooltip("STR_SAVE_FOLDER_DESC");
	_txtSaveFolderPath->onMouseIn((ActionHandler)&OptionsFoldersState::txtTooltipIn);
	_txtSaveFolderPath->onMouseOut((ActionHandler)&OptionsFoldersState::txtTooltipOut);
	_txtSaveFolderPath->onMouseClick((ActionHandler)&OptionsFoldersState::txtClick);

	_txtConfigFolderPath->setText(Options::getConfigFolder());
	_txtConfigFolderPath->setWordWrap(true);
	_txtConfigFolderPath->setTooltip("STR_CONFIG_FOLDER_DESC");
	_txtConfigFolderPath->onMouseIn((ActionHandler)&OptionsFoldersState::txtTooltipIn);
	_txtConfigFolderPath->onMouseOut((ActionHandler)&OptionsFoldersState::txtTooltipOut);
	_txtConfigFolderPath->onMouseClick((ActionHandler)&OptionsFoldersState::txtClick);

	// Populate navigable controls
	_navigableControls.push_back(_txtDataFolderPath1);
	_navigableControls.push_back(_txtDataFolderPath2);
	_navigableControls.push_back(_txtUserFolderPath);
	_navigableControls.push_back(_txtSaveFolderPath);
	_navigableControls.push_back(_txtConfigFolderPath);
	// OK, Cancel, Default buttons are not part of this cycle for now.

	_focusedControl = nullptr;
	_focusedIndex = -1;
}

/**
 * Initializes UI colors according to origin and sets up initial focus.
 */
void OptionsFoldersState::init()
{
	OptionsBaseState::init();

	_focusedIndex = -1;
	_focusedControl = nullptr;
	for (size_t i = 0; i < _navigableControls.size(); ++i)
	{
		if (_navigableControls[i] && _navigableControls[i]->getVisible() && _navigableControls[i]->getEnabled())
		{
			_focusedIndex = i;
			setFocusOn(_navigableControls[i]);
			break;
		}
	}
}

/**
 * Sets the visual state of a control based on focus.
 * @param control The control.
 * @param focused Whether it's gaining or losing focus.
 */
void OptionsFoldersState::setFocusedControlVisuals(InteractiveSurface* control, bool focused)
{
    if (!control) return;
    // Text objects don't have a standard 'down' or 'focused' visual state like buttons.
    // We can try changing their color, but this requires knowing their original color to revert.
    // For now, we'll just use the base InteractiveSurface::setHasFocus().
    // A more advanced implementation might involve a temporary border or color change.
    Text* textControl = dynamic_cast<Text*>(control);
    if (textControl) {
        // Example: if (focused) textControl->setColor(NEW_FOCUS_COLOR);
        // else textControl->setColor(ORIGINAL_COLOR);
        // This needs original color storage and appropriate focus color.
        // For now, no explicit visual change beyond what setHasFocus might enable.
    }
    control->setHasFocus(focused);
}

/**
 * Sets focus to a specific control.
 * @param control The control to focus.
 */
void OptionsFoldersState::setFocusOn(InteractiveSurface* control)
{
    if (_focusedControl && _focusedControl != control) {
        setFocusedControlVisuals(_focusedControl, false);
    }
    
    _focusedControl = control;
    _focusedIndex = -1;
    if (_focusedControl) {
        for(size_t i=0; i < _navigableControls.size(); ++i) {
            if (_navigableControls[i] == _focusedControl) {
                _focusedIndex = i;
                break;
            }
        }
        setFocusedControlVisuals(_focusedControl, true);
    }
}

/**
 * Cycles focus to the next or previous control.
 * @param forward True to cycle forward, false for backward.
 */
void OptionsFoldersState::cycleFocus(bool forward)
{
	if (_navigableControls.empty()) return;

	int startIndex = (_focusedIndex == -1 && forward) ? _navigableControls.size() - 1 : _focusedIndex;
	if (_focusedIndex == -1 && !forward) startIndex = 0;
	
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
	} while ((!_navigableControls[newIndex] || !_navigableControls[newIndex]->getVisible() || !_navigableControls[newIndex]->getEnabled()) && attempts < (int)_navigableControls.size() * 2);

    if (_navigableControls[newIndex] && _navigableControls[newIndex]->getVisible() && _navigableControls[newIndex]->getEnabled()) {
        setFocusOn(_navigableControls[newIndex]);
    } else if (startIndex != -1 && _navigableControls[startIndex] && _navigableControls[startIndex]->getVisible() && _navigableControls[startIndex]->getEnabled()) {
		setFocusOn(_navigableControls[startIndex]);
	} else {
		if(_focusedControl) setFocusedControlVisuals(_focusedControl, false);
		_focusedControl = nullptr;
		_focusedIndex = -1;
	}
}

/**
 * Handles any events.
 * @param action Pointer to an action.
 */
void OptionsFoldersState::handle(Action *action)
{
	bool handled = false;

	if (action->getDetails()->type == SDL_KEYDOWN) {
		SDLKey sym = action->getDetails()->key.keysym.sym;

		if (sym == Options::keyMenuUp) {
			cycleFocus(false);
			handled = true;
		} else if (sym == Options::keyMenuDown) {
			cycleFocus(true);
			handled = true;
		} else if (sym == Options::keyMenuSelect) {
			if (_focusedControl) {
				// All navigable controls in this state are Text objects that use txtClick
				SDL_Event sdlEvent;
				sdlEvent.type = SDL_MOUSEBUTTONDOWN;
				sdlEvent.button.button = SDL_BUTTON_LEFT;
				sdlEvent.button.state = SDL_PRESSED;
				Action clickAction(&sdlEvent, _game->getScreen()->getXScale(), _game->getScreen()->getYScale(), _game->getScreen()->getTopBlackBand(), _game->getScreen()->getLeftBlackBand());
				clickAction.setSender(_focusedControl);
				txtClick(&clickAction); // Call the existing click handler
			}
			handled = true;
		}
	}

	if (!handled) {
		OptionsBaseState::handle(action); // Handles keyMenuCancel
	}
}

/**
 *
 */
OptionsFoldersState::~OptionsFoldersState()
{

}

/**
 * Opens the location of the appropriate folder.
 * @param action Pointer to an action.
 */
void OptionsFoldersState::txtClick(Action* action)
{
	Text* sender = (Text*)action->getSender();
	std::string path = sender->getText();
	// Hint: whether the data folder is empty or not (i.e. using relative path or absolute path) depends on the compilation/environment
	// for example, you can test:
	// 1. absolute path: when running from Visual Studio 2022
	// 2. relative path: when running a MXE cross-compiled executable
	if (Options::getDataFolder().empty() && path.size() < 10)
	{
		// "UFO/", "TFTD/", "common/", "standard/"
		path = CrossPlatform::getExeFolder() + path;
	}
	CrossPlatform::openExplorer(path);
}

}
