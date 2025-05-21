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
#include "OptionsBattlescapeState.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/ToggleTextButton.h"
#include "../Interface/Text.h"
#include "../Interface/Slider.h"
#include "../Interface/ComboBox.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"
#include "../Engine/Game.h"       
#include "../Engine/Screen.h"     
#include "../Interface/TextList.h" 


namespace OpenXcom
{

OptionsBattlescapeState::OptionsBattlescapeState(OptionsOrigin origin) : OptionsBaseState(origin), _focusedControl(nullptr), _focusedIndex(-1)
{
	setCategory(_btnBattlescape);

	_txtEdgeScroll = new Text(114, 9, 94, 8);
	_cbxEdgeScroll = new ComboBox(this, 104, 16, 94, 18);
	_txtDragScroll = new Text(114, 9, 206, 8);
	_cbxDragScroll = new ComboBox(this, 104, 16, 206, 18);
	_txtScrollSpeed = new Text(114, 9, 94, 40);
	_slrScrollSpeed = new Slider(104, 16, 94, 50);
	_txtFireSpeed = new Text(114, 9, 206, 40);
	_slrFireSpeed = new Slider(104, 16, 206, 50);
	_txtXcomSpeed = new Text(114, 9, 94, 72);
	_slrXcomSpeed = new Slider(104, 16, 94, 82);
	_txtAlienSpeed = new Text(114, 9, 206, 72);
	_slrAlienSpeed = new Slider(104, 16, 206, 82);
	_txtPathPreview = new Text(114, 9, 94, 100);
	_btnArrows = new ToggleTextButton(104, 16, 94, 110);
	_btnTuCost = new ToggleTextButton(104, 16, 94, 128);
	_txtOptions = new Text(114, 9, 206, 100);
	_btnTooltips = new ToggleTextButton(104, 16, 206, 110);
	_btnDeaths = new ToggleTextButton(104, 16, 206, 128);

	add(_txtEdgeScroll, "text", "battlescapeMenu"); add(_txtDragScroll, "text", "battlescapeMenu");
	add(_txtScrollSpeed, "text", "battlescapeMenu"); add(_slrScrollSpeed, "button", "battlescapeMenu");
	add(_txtFireSpeed, "text", "battlescapeMenu"); add(_slrFireSpeed, "button", "battlescapeMenu");
	add(_txtXcomSpeed, "text", "battlescapeMenu"); add(_slrXcomSpeed, "button", "battlescapeMenu");
	add(_txtAlienSpeed, "text", "battlescapeMenu"); add(_slrAlienSpeed, "button", "battlescapeMenu");
	add(_txtPathPreview, "text", "battlescapeMenu");
	add(_btnArrows, "button", "battlescapeMenu"); add(_btnTuCost, "button", "battlescapeMenu");
	add(_txtOptions, "text", "battlescapeMenu");
	add(_btnTooltips, "button", "battlescapeMenu"); add(_btnDeaths, "button", "battlescapeMenu");
	add(_cbxEdgeScroll, "button", "battlescapeMenu"); add(_cbxDragScroll, "button", "battlescapeMenu");
	centerAllSurfaces();

	_txtEdgeScroll->setText(tr("STR_EDGE_SCROLL"));
	std::vector<std::string> edgeScrolls;
	edgeScrolls.push_back(tr("STR_DISABLED")); edgeScrolls.push_back(tr("STR_TRIGGER_SCROLL")); edgeScrolls.push_back(tr("STR_AUTO_SCROLL"));
	_cbxEdgeScroll->setOptions(edgeScrolls); _cbxEdgeScroll->setSelected(Options::battleEdgeScroll);
	_cbxEdgeScroll->onChange((ActionHandler)&OptionsBattlescapeState::cbxEdgeScrollChange);
	_cbxEdgeScroll->setTooltip("STR_EDGE_SCROLL_DESC"); _cbxEdgeScroll->onMouseIn((ActionHandler)&OptionsBattlescapeState::txtTooltipIn); _cbxEdgeScroll->onMouseOut((ActionHandler)&OptionsBattlescapeState::txtTooltipOut);
	_txtDragScroll->setText(tr("STR_DRAG_SCROLL"));
	std::vector<std::string> dragScrolls;
	dragScrolls.push_back(tr("STR_DISABLED")); dragScrolls.push_back(tr("STR_LEFT_MOUSE_BUTTON"));
	dragScrolls.push_back(tr("STR_MIDDLE_MOUSE_BUTTON")); dragScrolls.push_back(tr("STR_RIGHT_MOUSE_BUTTON"));
	_cbxDragScroll->setOptions(dragScrolls); _cbxDragScroll->setSelected(Options::battleDragScrollButton);
	_cbxDragScroll->onChange((ActionHandler)&OptionsBattlescapeState::cbxDragScrollChange);
	_cbxDragScroll->setTooltip("STR_DRAG_SCROLL_DESC"); _cbxDragScroll->onMouseIn((ActionHandler)&OptionsBattlescapeState::txtTooltipIn); _cbxDragScroll->onMouseOut((ActionHandler)&OptionsBattlescapeState::txtTooltipOut);
	_txtScrollSpeed->setText(tr("STR_SCROLL_SPEED"));
	_slrScrollSpeed->setRange(2, 20); _slrScrollSpeed->setValue(Options::battleScrollSpeed);
	_slrScrollSpeed->onChange((ActionHandler)&OptionsBattlescapeState::slrScrollSpeedChange);
	_slrScrollSpeed->setTooltip("STR_SCROLL_SPEED_BATTLE_DESC"); _slrScrollSpeed->onMouseIn((ActionHandler)&OptionsBattlescapeState::txtTooltipIn); _slrScrollSpeed->onMouseOut((ActionHandler)&OptionsBattlescapeState::txtTooltipOut);
	_txtFireSpeed->setText(tr("STR_FIRE_SPEED"));
	_slrFireSpeed->setRange(1, 20); _slrFireSpeed->setValue(Options::battleFireSpeed);
	_slrFireSpeed->onChange((ActionHandler)&OptionsBattlescapeState::slrFireSpeedChange);
	_slrFireSpeed->setTooltip("STR_FIRE_SPEED_DESC"); _slrFireSpeed->onMouseIn((ActionHandler)&OptionsBattlescapeState::txtTooltipIn); _slrFireSpeed->onMouseOut((ActionHandler)&OptionsBattlescapeState::txtTooltipOut);
	_txtXcomSpeed->setText(tr("STR_PLAYER_MOVEMENT_SPEED"));
	_slrXcomSpeed->setRange(40, 1); _slrXcomSpeed->setValue(Options::battleXcomSpeed);
	_slrXcomSpeed->onChange((ActionHandler)&OptionsBattlescapeState::slrXcomSpeedChange);
	_slrXcomSpeed->setTooltip("STR_PLAYER_MOVEMENT_SPEED_DESC"); _slrXcomSpeed->onMouseIn((ActionHandler)&OptionsBattlescapeState::txtTooltipIn); _slrXcomSpeed->onMouseOut((ActionHandler)&OptionsBattlescapeState::txtTooltipOut);
	_txtAlienSpeed->setText(tr("STR_COMPUTER_MOVEMENT_SPEED"));
	_slrAlienSpeed->setRange(40, 1); _slrAlienSpeed->setValue(Options::battleAlienSpeed);
	_slrAlienSpeed->onChange((ActionHandler)&OptionsBattlescapeState::slrAlienSpeedChange);
	_slrAlienSpeed->setTooltip("STR_COMPUTER_MOVEMENT_SPEED_DESC"); _slrAlienSpeed->onMouseIn((ActionHandler)&OptionsBattlescapeState::txtTooltipIn); _slrAlienSpeed->onMouseOut((ActionHandler)&OptionsBattlescapeState::txtTooltipOut);
	_txtPathPreview->setText(tr("STR_PATH_PREVIEW"));
	_btnArrows->setText(tr("STR_PATH_ARROWS")); _btnArrows->setPressed((Options::battleNewPreviewPath & PATH_ARROWS) != 0); _btnArrows->onMouseClick((ActionHandler)&OptionsBattlescapeState::btnPathPreviewClick);
	_btnArrows->setTooltip("STR_PATH_ARROWS_DESC"); _btnArrows->onMouseIn((ActionHandler)&OptionsBattlescapeState::txtTooltipIn); _btnArrows->onMouseOut((ActionHandler)&OptionsBattlescapeState::txtTooltipOut);
	_btnTuCost->setText(tr("STR_PATH_TIME_UNIT_COST")); _btnTuCost->setPressed((Options::battleNewPreviewPath & PATH_TU_COST) != 0); _btnTuCost->onMouseClick((ActionHandler)&OptionsBattlescapeState::btnPathPreviewClick);
	_btnTuCost->setTooltip("STR_PATH_TIME_UNIT_COST_DESC"); _btnTuCost->onMouseIn((ActionHandler)&OptionsBattlescapeState::txtTooltipIn); _btnTuCost->onMouseOut((ActionHandler)&OptionsBattlescapeState::txtTooltipOut);
	_txtOptions->setText(tr("STR_USER_INTERFACE_OPTIONS"));
	_btnTooltips->setText(tr("STR_TOOLTIPS")); _btnTooltips->setPressed(Options::battleTooltips); _btnTooltips->onMouseClick((ActionHandler)&OptionsBattlescapeState::btnTooltipsClick);
	_btnTooltips->setTooltip("STR_TOOLTIPS_DESC"); _btnTooltips->onMouseIn((ActionHandler)&OptionsBattlescapeState::txtTooltipIn); _btnTooltips->onMouseOut((ActionHandler)&OptionsBattlescapeState::txtTooltipOut);
	_btnDeaths->setText(tr("STR_DEATH_NOTIFICATIONS")); _btnDeaths->setPressed(Options::battleNotifyDeath); _btnDeaths->onMouseClick((ActionHandler)&OptionsBattlescapeState::btnDeathsClick);
	_btnDeaths->setTooltip("STR_DEATH_NOTIFICATIONS_DESC"); _btnDeaths->onMouseIn((ActionHandler)&OptionsBattlescapeState::txtTooltipIn); _btnDeaths->onMouseOut((ActionHandler)&OptionsBattlescapeState::txtTooltipOut);

	_navigableControls.push_back(_cbxEdgeScroll); _navigableControls.push_back(_cbxDragScroll);
	_navigableControls.push_back(_slrScrollSpeed); _navigableControls.push_back(_slrFireSpeed);
	_navigableControls.push_back(_slrXcomSpeed); _navigableControls.push_back(_slrAlienSpeed);
	_navigableControls.push_back(_btnArrows); _navigableControls.push_back(_btnTuCost);
	_navigableControls.push_back(_btnTooltips); _navigableControls.push_back(_btnDeaths);
}

OptionsBattlescapeState::~OptionsBattlescapeState() { }

void OptionsBattlescapeState::init() {
	OptionsBaseState::init();
	_focusedIndex = -1; _focusedControl = nullptr;
	for (size_t i = 0; i < _navigableControls.size(); ++i) {
		if (_navigableControls[i] && _navigableControls[i]->getVisible()) { // Removed getEnabled() check
			_focusedIndex = i; setFocusOn(_navigableControls[i]); break;
		}
	}
}

void OptionsBattlescapeState::setFocusedControlVisuals(InteractiveSurface* control, bool focused) {
    if (!control) return;
    ToggleTextButton* ttb = dynamic_cast<ToggleTextButton*>(control); ComboBox* cb = dynamic_cast<ComboBox*>(control);
    if (ttb) ttb->setPressed(focused); else if (cb) cb->setFocus(focused); // Corrected
    control->setFocus(focused); // Corrected
}

void OptionsBattlescapeState::setFocusOn(InteractiveSurface* control) {
    if (_focusedControl && _focusedControl != control) setFocusedControlVisuals(_focusedControl, false);
    _focusedControl = control; _focusedIndex = -1;
    if (_focusedControl) {
        for(size_t i=0; i < _navigableControls.size(); ++i) { if (_navigableControls[i] == _focusedControl) { _focusedIndex = i; break; } }
        setFocusedControlVisuals(_focusedControl, true);
    }
}

void OptionsBattlescapeState::cycleFocus(bool forward) {
	if (_navigableControls.empty()) return;
	int startIndex = (_focusedIndex == -1 && forward) ? (_navigableControls.size() - 1) : _focusedIndex;
	if (_focusedIndex == -1 && !forward) startIndex = 0;
	int newIndex = startIndex; int attempts = 0; 
	do {
		if (forward) newIndex = (newIndex + 1) % _navigableControls.size();
		else newIndex = (newIndex - 1 + _navigableControls.size()) % _navigableControls.size();
		attempts++;
	} while ((!_navigableControls[newIndex] || !_navigableControls[newIndex]->getVisible()) && attempts < (int)_navigableControls.size() * 2); // Removed getEnabled() check
    if (_navigableControls[newIndex] && _navigableControls[newIndex]->getVisible()) setFocusOn(_navigableControls[newIndex]);
    else if (startIndex != -1 && _navigableControls[startIndex] && _navigableControls[startIndex]->getVisible()) setFocusOn(_navigableControls[startIndex]);
    else { if(_focusedControl) setFocusedControlVisuals(_focusedControl, false); _focusedControl = nullptr; _focusedIndex = -1; }
}

void OptionsBattlescapeState::handle(Action *action) {
	bool handled = false;
	ComboBox* openComboBox = nullptr;
	if (_focusedControl) { openComboBox = dynamic_cast<ComboBox*>(_focusedControl); if (openComboBox && !openComboBox->isDropped()) openComboBox = nullptr; } // Corrected: isDropped
	
	if (openComboBox) { 
		SDLKey sym = action->getDetails()->key.keysym.sym;
        if (sym == Options::keyMenuSelect || sym == Options::keyMenuCancel || sym == Options::keyMenuUp || sym == Options::keyMenuDown) {
            openComboBox->handle(action, this); // Delegate to ComboBox
            if (!openComboBox->isDropped()) setFocusOn(openComboBox); // Corrected: isDropped
            handled = true; 
        }
	} else if (action->getDetails()->type == SDL_KEYDOWN) {
		SDLKey sym = action->getDetails()->key.keysym.sym;
		int sliderStep = 1; 
		if (sym == Options::keyMenuUp) { cycleFocus(false); handled = true; }
		else if (sym == Options::keyMenuDown) { cycleFocus(true); handled = true; }
		else if (sym == Options::keyMenuSelect) {
			if (_focusedControl) {
				SDL_Event sdlEvent; sdlEvent.type = SDL_MOUSEBUTTONDOWN; sdlEvent.button.button = SDL_BUTTON_LEFT; sdlEvent.button.state = SDL_PRESSED;
				Action clickAction(&sdlEvent, _game->getScreen()->getXScale(), _game->getScreen()->getYScale(), 0, 0); // Corrected: 0,0 for black bands
				clickAction.setSender(_focusedControl);
				if (_focusedControl == _btnArrows) btnPathPreviewClick(&clickAction);
				else if (_focusedControl == _btnTuCost) btnPathPreviewClick(&clickAction);
				else if (_focusedControl == _btnTooltips) btnTooltipsClick(&clickAction);
				else if (_focusedControl == _btnDeaths) btnDeathsClick(&clickAction);
				else if (dynamic_cast<ComboBox*>(_focusedControl)) _focusedControl->handle(&clickAction, this);
			}
			handled = true;
		} else if (sym == SDLK_LEFT) { // Use SDLK_LEFT for sliders
			Slider* sl = dynamic_cast<Slider*>(_focusedControl);
			if (sl) {
				SDL_Event dummyEvent; dummyEvent.type = SDL_USEREVENT; Action sliderAction(&dummyEvent, 1.0,1.0,0,0); sliderAction.setSender(sl);
				sl->decrement(this, &sliderAction, sliderStep);
			}
			handled = true;
		} else if (sym == SDLK_RIGHT) { // Use SDLK_RIGHT for sliders
			Slider* sl = dynamic_cast<Slider*>(_focusedControl);
			if (sl) {
				SDL_Event dummyEvent; dummyEvent.type = SDL_USEREVENT; Action sliderAction(&dummyEvent, 1.0,1.0,0,0); sliderAction.setSender(sl);
				sl->increment(this, &sliderAction, sliderStep);
			}
			handled = true;
		}
	}
	if (!handled) OptionsBaseState::handle(action);
}

void OptionsBattlescapeState::cbxEdgeScrollChange(Action *) { Options::battleEdgeScroll = (ScrollType)_cbxEdgeScroll->getSelected(); }
void OptionsBattlescapeState::cbxDragScrollChange(Action *) { Options::battleDragScrollButton = _cbxDragScroll->getSelected(); }
void OptionsBattlescapeState::slrScrollSpeedChange(Action *) { Options::battleScrollSpeed = _slrScrollSpeed->getValue(); }
void OptionsBattlescapeState::slrFireSpeedChange(Action *) { Options::battleFireSpeed = _slrFireSpeed->getValue(); }
void OptionsBattlescapeState::slrXcomSpeedChange(Action *) { Options::battleXcomSpeed = _slrXcomSpeed->getValue(); }
void OptionsBattlescapeState::slrAlienSpeedChange(Action *) { Options::battleAlienSpeed = _slrAlienSpeed->getValue(); }
void OptionsBattlescapeState::btnPathPreviewClick(Action *) {
	int mode = PATH_NONE;
	if (_btnArrows->getPressed()) mode |= PATH_ARROWS;
	if (_btnTuCost->getPressed()) mode |= PATH_TU_COST;
	Options::battleNewPreviewPath = (PathPreview)mode;
}
void OptionsBattlescapeState::btnTooltipsClick(Action *) { Options::battleTooltips = _btnTooltips->getPressed(); }
void OptionsBattlescapeState::btnDeathsClick(Action *) { Options::battleNotifyDeath = _btnDeaths->getPressed(); }

}
