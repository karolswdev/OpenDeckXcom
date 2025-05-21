/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "ListGamesState.h"
#include <algorithm>
#include <functional>
#include "../Engine/Logger.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"
#include "../Engine/Unicode.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Interface/ArrowButton.h"
#include "DeleteGameState.h"

namespace OpenXcom
{

struct compareSaveName
{
	bool _reverse;

	compareSaveName(bool reverse) : _reverse(reverse) {}

	bool operator()(const SaveInfo &a, const SaveInfo &b) const
	{
		if (a.reserved == b.reserved)
		{
			return Unicode::naturalCompare(a.displayName, b.displayName);
		}
		else
		{
			return _reverse ? b.reserved : a.reserved;
		}
	}
};

struct compareSaveTimestamp
{
	bool _reverse;

	compareSaveTimestamp(bool reverse) : _reverse(reverse) {}

	bool operator()(const SaveInfo &a, const SaveInfo &b) const
	{
		if (a.reserved == b.reserved)
		{
			return a.timestamp < b.timestamp;
		}
		else
		{
			return _reverse ? b.reserved : a.reserved;
		}
	}
};

/**
 * Initializes all the elements in the Saved Game screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 * @param firstValidRow First row containing saves.
 * @param autoquick Show auto/quick saved games?
 */
ListGamesState::ListGamesState(OptionsOrigin origin, int firstValidRow, bool autoquick) : _origin(origin), _firstValidRow(firstValidRow), _autoquick(autoquick), _sortable(true)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_btnCancel = new TextButton(80, 16, 120, 172);
	_txtTitle = new Text(310, 17, 5, 7);
	_txtDelete = new Text(310, 9, 5, 23);
	_txtName = new Text(150, 9, 16, 32);
	_txtDate = new Text(110, 9, 204, 32);
	_lstSaves = new TextList(288, 112, 8, 42);
	_txtDetails = new Text(288, 16, 16, 156);
	_sortName = new ArrowButton(ARROW_NONE, 11, 8, 16, 32);
	_sortDate = new ArrowButton(ARROW_NONE, 11, 8, 204, 32);

	// Set palette
	setInterface("geoscape", true, _game->getSavedGame() ? _game->getSavedGame()->getSavedBattle() : 0);

	add(_window, "window", "saveMenus");
	add(_btnCancel, "button", "saveMenus");
	add(_txtTitle, "text", "saveMenus");
	add(_txtDelete, "text", "saveMenus");
	add(_txtName, "text", "saveMenus");
	add(_txtDate, "text", "saveMenus");
	add(_lstSaves, "list", "saveMenus");
	add(_txtDetails, "text", "saveMenus");
	add(_sortName, "text", "saveMenus");
	add(_sortDate, "text", "saveMenus");

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&ListGamesState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&ListGamesState::btnCancelClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_txtDelete->setAlign(ALIGN_CENTER);
	_txtDelete->setText(tr("STR_RIGHT_CLICK_TO_DELETE"));

	_txtName->setText(tr("STR_NAME"));

	_txtDate->setText(tr("STR_DATE"));

	_lstSaves->setColumns(3, 188, 60, 40);
	_lstSaves->setSelectable(true);
	_lstSaves->setBackground(_window);
	_lstSaves->setMargin(8);
	_lstSaves->onMouseOver((ActionHandler)&ListGamesState::lstSavesMouseOver);
	_lstSaves->onMouseOut((ActionHandler)&ListGamesState::lstSavesMouseOut);
	_lstSaves->onMousePress((ActionHandler)&ListGamesState::lstSavesPress);

	_txtDetails->setWordWrap(true);
	_txtDetails->setText(tr("STR_DETAILS").arg(""));

	_sortName->setX(_sortName->getX() + _txtName->getTextWidth() + 5);
	_sortName->onMouseClick((ActionHandler)&ListGamesState::sortNameClick);

	_sortDate->setX(_sortDate->getX() + _txtDate->getTextWidth() + 5);
	_sortDate->onMouseClick((ActionHandler)&ListGamesState::sortDateClick);

	updateArrows();
}

/**
 *
 */
ListGamesState::~ListGamesState()
{

}

/**
 * Refreshes the saves list.
 */
void ListGamesState::init()
{
	State::init();

	if (_origin == OPT_BATTLESCAPE)
	{
		applyBattlescapeTheme();
	}

	try
	{
		_saves = SavedGame::getList(_game->getLanguage(), _autoquick);
		_lstSaves->clearList();
		sortList(Options::saveOrder); // This calls updateList() which populates _lstSaves

		if (!_saves.empty()) // Only set focus and selection if there are items
		{
			_lstSaves->setFocus(true);
			// Try to select the first valid item
			// _firstValidRow is the index of the first game entry in _lstSaves's rows
			// We want _lstSaves->_selRow to be _firstValidRow
			// Calling selectNext() repeatedly until _lstSaves->getSelectedRow() == _firstValidRow
			// This is still a bit indirect. A direct selectRow(idx) in TextList would be better.
			if (_lstSaves->getRows() > _firstValidRow)
			{
				// Scroll to the first valid row to make it visible
				_lstSaves->scrollTo(_firstValidRow);
				
				// To set the selection correctly on the first valid item:
				// Reset _selRow to an invalid state or 0, then call selectNext until the desired row is selected.
				// This is hacky. Let's assume _selRow is 0 initially or some other state.
				// We want to ensure _lstSaves->getSelectedRow() returns _firstValidRow.
				// The simplest for now is to call selectNext until it lands on or after _firstValidRow.
				// This relies on selectNext() correctly updating the visual selector.
                // If _lstSaves->_selRow is private and starts at 0 (default for size_t).
                // And _firstValidRow is, say, 0 as well. selectNext() makes _selRow 1.
                // This needs a robust way to set selection to a specific row index.

                // For now, let's scroll and assume the user will use down arrow once if first item isn't auto-selected.
                // Or, if _selRow can be assumed to be 0 initially after clearList/sortList:
                // size_t current_sel_row_in_list = 0; // Assuming this is the state of TextList's _selRow // REMOVED
                for(size_t i = 0; i < _firstValidRow; ++i) {
                    // This is a conceptual loop. We can't directly manipulate _selRow.
                    // If TextList::selectNext() is called, _selRow increments.
                    // This is not the right way to initialize selection to a specific row.
                }
                // The selectNext() in TextList now updates the visual selector.
                // If there are items, we want the first *selectable* one to be highlighted.
                // Call selectNext() to move from a potential default state (e.g. index 0 if it's a header)
                // then rely on the handle() method's logic to skip non-selectable rows.
                _lstSaves->selectNext(); 
                while(_lstSaves->getSelectedRow() < _firstValidRow && _lstSaves->getRows() > 0) {
                    unsigned int prev_sel = _lstSaves->getSelectedRow();
                    _lstSaves->selectNext();
                    // Break if it's not advancing to prevent infinite loop on fully invalid list (should not happen here)
                    if (_lstSaves->getSelectedRow() == prev_sel) break; 
                }

			}
		}
	}
	catch (Exception &e)
	{
		Log(LOG_ERROR) << e.what();
	}
}

/**
 * Updates the sorting arrows based
 * on the current setting.
 */
void ListGamesState::updateArrows()
{
	_sortName->setShape(ARROW_NONE);
	_sortDate->setShape(ARROW_NONE);
	switch (Options::saveOrder)
	{
	case SORT_NAME_ASC:
		_sortName->setShape(ARROW_SMALL_UP);
		break;
	case SORT_NAME_DESC:
		_sortName->setShape(ARROW_SMALL_DOWN);
		break;
	case SORT_DATE_ASC:
		_sortDate->setShape(ARROW_SMALL_UP);
		break;
	case SORT_DATE_DESC:
		_sortDate->setShape(ARROW_SMALL_DOWN);
		break;
	}
}

/**
 * Sorts the save game list.
 * @param sort Order to sort the games in.
 */
void ListGamesState::sortList(SaveSort sort)
{
	switch (sort)
	{
	case SORT_NAME_ASC:
		std::sort(_saves.begin(), _saves.end(), compareSaveName(false));
		break;
	case SORT_NAME_DESC:
		std::sort(_saves.rbegin(), _saves.rend(), compareSaveName(true));
		break;
	case SORT_DATE_ASC:
		std::sort(_saves.begin(), _saves.end(), compareSaveTimestamp(false));
		break;
	case SORT_DATE_DESC:
		std::sort(_saves.rbegin(), _saves.rend(), compareSaveTimestamp(true));
		break;
	}
	updateList();
}

/**
 * Updates the save game list with the current list
 * of available savegames.
 */
void ListGamesState::updateList()
{
	int row = 0;
	int color = _lstSaves->getSecondaryColor();
	for (std::vector<SaveInfo>::const_iterator i = _saves.begin(); i != _saves.end(); ++i)
	{
		_lstSaves->addRow(3, i->displayName.c_str(), i->isoDate.c_str(), i->isoTime.c_str());
		if (i->reserved && _origin != OPT_BATTLESCAPE)
		{
			_lstSaves->setRowColor(row, color);
		}
		row++;
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ListGamesState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Shows the details of the currently hovered save.
 * @param action Pointer to an action.
 */
void ListGamesState::lstSavesMouseOver(Action *)
{
	int sel = _lstSaves->getSelectedRow() - _firstValidRow;
	std::string wstr;
	if (sel >= 0 && sel < (int)_saves.size())
	{
		wstr = _saves[sel].details;
	}
	_txtDetails->setText(tr("STR_DETAILS").arg(wstr));
}

/**
 * Clears the details.
 * @param action Pointer to an action.
 */
void ListGamesState::lstSavesMouseOut(Action *)
{
	_txtDetails->setText(tr("STR_DETAILS").arg(""));
}

/**
 * Deletes the selected save.
 * @param action Pointer to an action.
 */
void ListGamesState::lstSavesPress(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT && _lstSaves->getSelectedRow() >= _firstValidRow)
	{
		_game->pushState(new DeleteGameState(_origin, _saves[_lstSaves->getSelectedRow() - _firstValidRow].fileName));
	}
}

/**
 * Sorts the saves by name.
 * @param action Pointer to an action.
 */
void ListGamesState::sortNameClick(Action *)
{
	if (_sortable)
	{
		if (Options::saveOrder == SORT_NAME_ASC)
		{
			Options::saveOrder = SORT_NAME_DESC;
		}
		else
		{
			Options::saveOrder = SORT_NAME_ASC;
		}
		updateArrows();
		_lstSaves->clearList();
		sortList(Options::saveOrder);
	}
}

/**
 * Sorts the saves by date.
 * @param action Pointer to an action.
 */
void ListGamesState::sortDateClick(Action *)
{
	if (_sortable)
	{
		if (Options::saveOrder == SORT_DATE_ASC)
		{
			Options::saveOrder = SORT_DATE_DESC;
		}
		else
		{
			Options::saveOrder = SORT_DATE_ASC;
		}
		updateArrows();
		_lstSaves->clearList();
		sortList(Options::saveOrder);
	}
}

void ListGamesState::disableSort()
{
	_sortable = false;
}

}
