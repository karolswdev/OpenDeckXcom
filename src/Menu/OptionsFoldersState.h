#pragma once
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
#include "OptionsBaseState.h"

namespace OpenXcom
{

class Text;

/**
 * Screen that shows the user various folders used by OpenXcom.
 */
class OptionsFoldersState : public OptionsBaseState
{
private:
	Text *_txtDataFolder, *_txtUserFolder, *_txtSaveFolder, *_txtConfigFolder;
	Text* _txtDataFolderPath1, *_txtDataFolderPath2, *_txtUserFolderPath, *_txtSaveFolderPath, *_txtConfigFolderPath;

	std::vector<InteractiveSurface*> _navigableControls;
	InteractiveSurface* _focusedControl;
	int _focusedIndex;

	/// Sets the currently focused control and updates its visual state.
	void setFocusedControlVisuals(InteractiveSurface* control, bool focused);
	/// Cycles focus to the next/previous control.
	void cycleFocus(bool forward);
	/// Sets the focus to a specific control.
	void setFocusOn(InteractiveSurface* control);

public:
	/// Creates the Folders Options state.
	OptionsFoldersState(OptionsOrigin origin);
	/// Cleans up the Folders Options state.
	~OptionsFoldersState();
	/// Initializes the state (sets up focus).
	virtual void init();
	/// Handles any events.
	virtual void handle(Action *action);
	/// Handler for opei.
	void txtClick(Action* action);
};

}
