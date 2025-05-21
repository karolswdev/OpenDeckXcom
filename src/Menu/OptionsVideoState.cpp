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
#include "OptionsVideoState.h"
#include "../Engine/Language.h"
#include "../Interface/TextButton.h" 
#include "../Interface/ArrowButton.h" 
#include "../Engine/Action.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Interface/ToggleTextButton.h"
#include "../Engine/Options.h"
#include "../Engine/Screen.h"
#include "../Engine/FileMap.h"
#include "../Engine/Logger.h"
#include "../Interface/ComboBox.h"
#include "../Engine/Game.h"
#include "SetWindowedRootState.h"
#include "../Interface/TextList.h" 


namespace OpenXcom
{

const std::string OptionsVideoState::GL_EXT = "OpenGL.shader";
const std::string OptionsVideoState::GL_FOLDER = "Shaders/";
const std::string OptionsVideoState::GL_STRING = "*";

OptionsVideoState::OptionsVideoState(OptionsOrigin origin) : OptionsBaseState(origin), _focusedControl(nullptr), _focusedIndex(-1)
{
	setCategory(_btnVideo);

	_displaySurface = new InteractiveSurface(110, 32, 94, 18);
	_txtDisplayResolution = new Text(114, 9, 94, 8);
	_txtDisplayWidth = new TextEdit(this, 40, 17, 94, 26);
	_txtDisplayX = new Text(16, 17, 132, 26);
	_txtDisplayHeight = new TextEdit(this, 40, 17, 144, 26);
	_btnDisplayResolutionUp = new ArrowButton(ARROW_BIG_UP, 14, 14, 186, 18);
	_btnDisplayResolutionDown = new ArrowButton(ARROW_BIG_DOWN, 14, 14, 186, 36);
	_txtLanguage = new Text(114, 9, 94, 52);
	_cbxLanguage = new ComboBox(this, 104, 16, 94, 62);
	_txtFilter = new Text(114, 9, 206, 52);
	_cbxFilter = new ComboBox(this, 104, 16, 206, 62);
	_txtMode = new Text(114, 9, 206, 22);
	_cbxDisplayMode = new ComboBox(this, 104, 16, 206, 32);
	_txtGeoScale = new Text(114, 9, 94, 82);
	_cbxGeoScale = new ComboBox(this, 104, 16, 94, 92);
	_txtBattleScale = new Text(114, 9, 94, 112);
	_cbxBattleScale = new ComboBox(this, 104, 16, 94, 122);
	_txtOptions = new Text(114, 9, 206, 82);
	_btnLetterbox = new ToggleTextButton(104, 16, 206, 92);
	_btnLockMouse = new ToggleTextButton(104, 16, 206, 110);
	_btnRootWindowedMode = new ToggleTextButton(104, 16, 206, 128);

	_res = SDL_ListModes(NULL, SDL_FULLSCREEN);
	if (_res != (SDL_Rect**)-1 && _res != (SDL_Rect**)0) {
		int i; _resCurrent = -1;
		for (i = 0; _res[i]; ++i) {
			if (_resCurrent == -1 && ((_res[i]->w == Options::displayWidth && _res[i]->h <= Options::displayHeight) || _res[i]->w < Options::displayWidth)) {
				_resCurrent = i;
			}
		}
		_resAmount = i;
	} else {
		_resCurrent = -1; _resAmount = 0;
		_btnDisplayResolutionDown->setVisible(false); _btnDisplayResolutionUp->setVisible(false);
		Log(LOG_WARNING) << "Couldn't get display resolutions";
	}

	add(_displaySurface);
	add(_txtDisplayResolution, "text", "videoMenu");
	add(_txtDisplayWidth, "resolution", "videoMenu"); add(_txtDisplayX, "resolution", "videoMenu"); add(_txtDisplayHeight, "resolution", "videoMenu");
	add(_btnDisplayResolutionUp, "button", "videoMenu"); add(_btnDisplayResolutionDown, "button", "videoMenu");
	add(_txtLanguage, "text", "videoMenu"); add(_txtFilter, "text", "videoMenu"); add(_txtMode, "text", "videoMenu");
	add(_txtOptions, "text", "videoMenu");
	add(_btnLetterbox, "button", "videoMenu"); add(_btnLockMouse, "button", "videoMenu"); add(_btnRootWindowedMode, "button", "videoMenu");
	add(_cbxFilter, "button", "videoMenu"); add(_cbxDisplayMode, "button", "videoMenu");
	add(_txtBattleScale, "text", "videoMenu"); add(_cbxBattleScale, "button", "videoMenu");
	add(_txtGeoScale, "text", "videoMenu"); add(_cbxGeoScale, "button", "videoMenu");
	add(_cbxLanguage, "button", "videoMenu");
	centerAllSurfaces();

	_txtDisplayResolution->setText(tr("STR_DISPLAY_RESOLUTION"));
	_displaySurface->setTooltip("STR_DISPLAY_RESOLUTION_DESC");
	_displaySurface->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_displaySurface->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	_txtDisplayWidth->setAlign(ALIGN_CENTER); _txtDisplayWidth->setBig(); _txtDisplayWidth->setConstraint(TEC_NUMERIC_POSITIVE); _txtDisplayWidth->onChange((ActionHandler)&OptionsVideoState::txtDisplayWidthChange);
	_txtDisplayX->setAlign(ALIGN_CENTER); _txtDisplayX->setBig(); _txtDisplayX->setText("x");
	_txtDisplayHeight->setAlign(ALIGN_CENTER); _txtDisplayHeight->setBig(); _txtDisplayHeight->setConstraint(TEC_NUMERIC_POSITIVE); _txtDisplayHeight->onChange((ActionHandler)&OptionsVideoState::txtDisplayHeightChange);
	std::ostringstream ssW, ssH; ssW << Options::displayWidth; ssH << Options::displayHeight;
	_txtDisplayWidth->setText(ssW.str()); _txtDisplayHeight->setText(ssH.str());
	_btnDisplayResolutionUp->onMouseClick((ActionHandler)&OptionsVideoState::btnDisplayResolutionUpClick);
	_btnDisplayResolutionDown->onMouseClick((ActionHandler)&OptionsVideoState::btnDisplayResolutionDownClick);
	_txtMode->setText(tr("STR_DISPLAY_MODE"));
	_txtOptions->setText(tr("STR_DISPLAY_OPTIONS"));
	_btnLetterbox->setText(tr("STR_LETTERBOXED")); _btnLetterbox->setPressed(Options::keepAspectRatio); _btnLetterbox->onMouseClick((ActionHandler)&OptionsVideoState::btnLetterboxClick);
	_btnLetterbox->setTooltip("STR_LETTERBOXED_DESC"); _btnLetterbox->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn); _btnLetterbox->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	_btnLockMouse->setText(tr("STR_LOCK_MOUSE")); _btnLockMouse->setPressed(Options::captureMouse == SDL_GRAB_ON); _btnLockMouse->onMouseClick((ActionHandler)&OptionsVideoState::btnLockMouseClick);
	_btnLockMouse->setTooltip("STR_LOCK_MOUSE_DESC"); _btnLockMouse->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn); _btnLockMouse->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	_btnRootWindowedMode->setText(tr("STR_FIXED_WINDOW_POSITION")); _btnRootWindowedMode->setPressed(Options::rootWindowedMode); _btnRootWindowedMode->onMouseClick((ActionHandler)&OptionsVideoState::btnRootWindowedModeClick);
	_btnRootWindowedMode->setTooltip("STR_FIXED_WINDOW_POSITION_DESC"); _btnRootWindowedMode->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn); _btnRootWindowedMode->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	_txtLanguage->setText(tr("STR_DISPLAY_LANGUAGE"));
	std::vector<std::string> names; Language::getList(_langs, names); _cbxLanguage->setOptions(names);
	for (size_t i = 0; i < names.size(); ++i) { if (_langs[i] == Options::language) { _cbxLanguage->setSelected(i); break; } }
	_cbxLanguage->onChange((ActionHandler)&OptionsVideoState::cbxLanguageChange);
	_cbxLanguage->setTooltip("STR_DISPLAY_LANGUAGE_DESC"); _cbxLanguage->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn); _cbxLanguage->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	std::vector<std::string> filterNames; filterNames.push_back(tr("STR_DISABLED")); filterNames.push_back("Scale"); filterNames.push_back("HQx"); filterNames.push_back("xBRZ");
	_filters.push_back(""); _filters.push_back(""); _filters.push_back(""); _filters.push_back("");
#ifndef __NO_OPENGL
	std::set<std::string> filters = FileMap::filterFiles(FileMap::getVFolderContents(GL_FOLDER), GL_EXT);
	for (std::set<std::string>::iterator i = filters.begin(); i != filters.end(); ++i) {
		std::string file = (*i); std::string path = GL_FOLDER + file; std::string name = file.substr(0, file.length() - GL_EXT.length() - 1) + GL_STRING;
		filterNames.push_back(ucWords(name)); _filters.push_back(path);
	}
#endif
	size_t selFilter = 0;
	if (Screen::useOpenGL()) {
#ifndef __NO_OPENGL
		std::string path = Options::useOpenGLShader;
		for (size_t i = 0; i < _filters.size(); ++i) { if (_filters[i] == path) { selFilter = i; break; } }
#endif
	} else if (Options::useScaleFilter) selFilter = 1; else if (Options::useHQXFilter) selFilter = 2; else if (Options::useXBRZFilter) selFilter = 3;
	_txtFilter->setText(tr("STR_DISPLAY_FILTER"));
	_cbxFilter->setOptions(filterNames); _cbxFilter->setSelected(selFilter); _cbxFilter->onChange((ActionHandler)&OptionsVideoState::cbxFilterChange);
	_cbxFilter->setTooltip("STR_DISPLAY_FILTER_DESC"); _cbxFilter->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn); _cbxFilter->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	std::vector<std::string> displayModes; displayModes.push_back(tr("STR_WINDOWED")); displayModes.push_back(tr("STR_FULLSCREEN")); displayModes.push_back(tr("STR_BORDERLESS")); displayModes.push_back(tr("STR_RESIZABLE"));
	int displayMode = 0; if (Options::fullscreen) displayMode = 1; else if (Options::borderless) displayMode = 2; else if (Options::allowResize) displayMode = 3;
	_cbxDisplayMode->setOptions(displayModes); _cbxDisplayMode->setSelected(displayMode); _cbxDisplayMode->onChange((ActionHandler)&OptionsVideoState::updateDisplayMode);
	_cbxDisplayMode->setTooltip("STR_DISPLAY_MODE_DESC"); _cbxDisplayMode->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn); _cbxDisplayMode->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	_txtGeoScale->setText(tr("STR_GEOSCAPE_SCALE"));
	std::vector<std::string> scales; scales.push_back(tr("STR_ORIGINAL")); scales.push_back(tr("STR_1_5X")); scales.push_back(tr("STR_2X")); scales.push_back(tr("STR_THIRD_DISPLAY")); scales.push_back(tr("STR_HALF_DISPLAY")); scales.push_back(tr("STR_FULL_DISPLAY"));
	_cbxGeoScale->setOptions(scales); _cbxGeoScale->setSelected(Options::geoscapeScale); _cbxGeoScale->onChange((ActionHandler)&OptionsVideoState::updateGeoscapeScale);
	_cbxGeoScale->setTooltip("STR_GEOSCAPESCALE_SCALE_DESC"); _cbxGeoScale->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn); _cbxGeoScale->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	_txtBattleScale->setText(tr("STR_BATTLESCAPE_SCALE"));
	_cbxBattleScale->setOptions(scales); _cbxBattleScale->setSelected(Options::battlescapeScale); _cbxBattleScale->onChange((ActionHandler)&OptionsVideoState::updateBattlescapeScale);
	_cbxBattleScale->setTooltip("STR_BATTLESCAPE_SCALE_DESC"); _cbxBattleScale->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn); _cbxBattleScale->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);

	_navigableControls.push_back(_txtDisplayWidth); _navigableControls.push_back(_txtDisplayHeight);
	_navigableControls.push_back(_btnDisplayResolutionUp); _navigableControls.push_back(_btnDisplayResolutionDown);
	_navigableControls.push_back(_cbxDisplayMode); _navigableControls.push_back(_cbxLanguage);
	_navigableControls.push_back(_cbxFilter); _navigableControls.push_back(_cbxGeoScale);
	_navigableControls.push_back(_cbxBattleScale); _navigableControls.push_back(_btnLetterbox);
	_navigableControls.push_back(_btnLockMouse); _navigableControls.push_back(_btnRootWindowedMode);
}

OptionsVideoState::~OptionsVideoState() { }

void OptionsVideoState::init() {
	OptionsBaseState::init(); 
	_focusedIndex = -1; _focusedControl = nullptr;
	for (size_t i = 0; i < _navigableControls.size(); ++i) {
		if (_navigableControls[i] && _navigableControls[i]->getVisible() && _navigableControls[i]->getEnabled()) { 
			_focusedIndex = i; setFocusOn(_navigableControls[i]); break;
		}
	}
}

void OptionsVideoState::setFocusedControlVisuals(InteractiveSurface* control, bool focused) {
    if (!control) return;
    TextButton* tb = dynamic_cast<TextButton*>(control); 
    ToggleTextButton* ttb = dynamic_cast<ToggleTextButton*>(control);
    ArrowButton* ab = dynamic_cast<ArrowButton*>(control);
    ComboBox* cb = dynamic_cast<ComboBox*>(control);
    TextEdit* te = dynamic_cast<TextEdit*>(control);
    if (ttb) ttb->setPressed(focused);      
    else if (ab) ab->setDown(focused, false); 
    else if (tb) tb->setPressed(focused); // Using setPressed for general TextButton focus too
    else if (cb) cb->setFocus(focused);     
    else if (te) te->setFocus(focused);     
    control->setFocus(focused);             
}

void OptionsVideoState::setFocusOn(InteractiveSurface* control) {
    if (_focusedControl && _focusedControl != control) setFocusedControlVisuals(_focusedControl, false);
    _focusedControl = control; _focusedIndex = -1; 
    if (_focusedControl) {
        for(size_t i=0; i < _navigableControls.size(); ++i) { if (_navigableControls[i] == _focusedControl) { _focusedIndex = i; break; } }
        setFocusedControlVisuals(_focusedControl, true);
    }
}

void OptionsVideoState::cycleFocus(bool forward) {
	if (_navigableControls.empty()) return;
	int startIndex = (_focusedIndex == -1 && forward) ? (_navigableControls.size() - 1) : _focusedIndex;
	if (_focusedIndex == -1 && !forward) startIndex = 0;
	int newIndex = startIndex; int attempts = 0; 
	do {
		if (forward) newIndex = (newIndex + 1) % _navigableControls.size();
		else newIndex = (newIndex - 1 + _navigableControls.size()) % _navigableControls.size();
		attempts++;
	} while ((!_navigableControls[newIndex] || 
              !_navigableControls[newIndex]->getVisible() || 
              !_navigableControls[newIndex]->getEnabled()) && 
             attempts < (int)_navigableControls.size() * 2); 
    if (_navigableControls[newIndex] && _navigableControls[newIndex]->getVisible() && _navigableControls[newIndex]->getEnabled()) setFocusOn(_navigableControls[newIndex]);
    else if (startIndex != -1 && _navigableControls[startIndex] && _navigableControls[startIndex]->getVisible() && _navigableControls[startIndex]->getEnabled()) setFocusOn(_navigableControls[startIndex]);
    else { if(_focusedControl) setFocusedControlVisuals(_focusedControl, false); _focusedControl = nullptr; _focusedIndex = -1; }
}

std::string OptionsVideoState::ucWords(std::string str) { 
	for (size_t i = 0; i < str.length(); ++i) {
		if (i == 0) str[0] = toupper(str[0]);
		else if (str[i] == ' ' || str[i] == '-' || str[i] == '_') {
			str[i] = ' '; if (str.length() > i + 1) str[i + 1] = toupper(str[i + 1]);
		}
	}
	return str;
}
void OptionsVideoState::btnDisplayResolutionUpClick(Action *) { 
	if (_resAmount == 0) return;
	if (_resCurrent <= 0) _resCurrent = _resAmount-1; else _resCurrent--;
	updateDisplayResolution();
}
void OptionsVideoState::btnDisplayResolutionDownClick(Action *) { 
	if (_resAmount == 0) return;
	if (_resCurrent >= _resAmount-1) _resCurrent = 0; else _resCurrent++;
	updateDisplayResolution();
}
void OptionsVideoState::updateDisplayResolution() { 
	std::ostringstream ssW, ssH; ssW << (int)_res[_resCurrent]->w; ssH << (int)_res[_resCurrent]->h;
	_txtDisplayWidth->setText(ssW.str()); _txtDisplayHeight->setText(ssH.str());
	Options::newDisplayWidth = _res[_resCurrent]->w; Options::newDisplayHeight = _res[_resCurrent]->h;
}
void OptionsVideoState::txtDisplayWidthChange(Action *) { 
	std::stringstream ss; int width = 0; ss << std::dec << _txtDisplayWidth->getText(); ss >> std::dec >> width; Options::newDisplayWidth = width;
	if (_res != (SDL_Rect**)-1 && _res != (SDL_Rect**)0) {
		int i; _resCurrent = -1;
		for (i = 0; _res[i]; ++i) { if (_resCurrent == -1 && ((_res[i]->w == Options::newDisplayWidth && _res[i]->h <= Options::newDisplayHeight) || _res[i]->w < Options::newDisplayWidth)) _resCurrent = i; }
	}
}
void OptionsVideoState::txtDisplayHeightChange(Action *) { 
	std::stringstream ss; int height = 0; ss << std::dec << _txtDisplayHeight->getText(); ss >> std::dec >> height; Options::newDisplayHeight = height;
	if (_res != (SDL_Rect**)-1 && _res != (SDL_Rect**)0) {
		int i; _resCurrent = -1;
		for (i = 0; _res[i]; ++i) { if (_resCurrent == -1 && ((_res[i]->w == Options::newDisplayWidth && _res[i]->h <= Options::newDisplayHeight) || _res[i]->w < Options::newDisplayWidth)) _resCurrent = i; }
	}
}
void OptionsVideoState::cbxLanguageChange(Action *) { Options::language = _langs[_cbxLanguage->getSelected()]; }
void OptionsVideoState::cbxFilterChange(Action *) { 
	switch (_cbxFilter->getSelected()) {
	case 0: Options::newOpenGL = false; Options::newScaleFilter = false; Options::newHQXFilter = false; Options::newXBRZFilter = false; break;
	case 1: Options::newOpenGL = false; Options::newScaleFilter = true; Options::newHQXFilter = false; Options::newXBRZFilter = false; break;
	case 2: Options::newOpenGL = false; Options::newScaleFilter = false; Options::newHQXFilter = true; Options::newXBRZFilter = false; break;
	case 3: Options::newOpenGL = false; Options::newScaleFilter = false; Options::newHQXFilter = false; Options::newXBRZFilter = true; break;
	default: Options::newOpenGL = true; Options::newScaleFilter = false; Options::newHQXFilter = false; Options::newXBRZFilter = false; Options::newOpenGLShader = _filters[_cbxFilter->getSelected()]; break;
	}
}
void OptionsVideoState::updateDisplayMode(Action *) { 
	switch(_cbxDisplayMode->getSelected()) {
	case 0: Options::newFullscreen = false; Options::newBorderless = false; Options::newAllowResize = false; break;
	case 1: Options::newFullscreen = true; Options::newBorderless = false; Options::newAllowResize = false; break;
	case 2: Options::newFullscreen = false; Options::newBorderless = true; Options::newAllowResize = false; break;
	case 3: Options::newFullscreen = false; Options::newBorderless = false; Options::newAllowResize = true; break;
	default: break;
	}
}
void OptionsVideoState::btnLetterboxClick(Action *) { Options::keepAspectRatio = _btnLetterbox->getPressed(); }
void OptionsVideoState::btnLockMouseClick(Action *) { Options::captureMouse = (SDL_GrabMode)_btnLockMouse->getPressed(); SDL_WM_GrabInput(Options::captureMouse); }
void OptionsVideoState::btnRootWindowedModeClick(Action *) { if (_btnRootWindowedMode->getPressed()) { _game->pushState(new SetWindowedRootState(_origin, this)); } else { Options::newRootWindowedMode = false; } }
void OptionsVideoState::updateGeoscapeScale(Action *) { Options::newGeoscapeScale = _cbxGeoScale->getSelected(); }
void OptionsVideoState::updateBattlescapeScale(Action *) { Options::newBattlescapeScale = _cbxBattleScale->getSelected(); }
void OptionsVideoState::resize(int &dX, int &dY) { 
	OptionsBaseState::resize(dX, dY); std::ostringstream ss; ss << Options::displayWidth; _txtDisplayWidth->setText(ss.str()); ss.str(""); ss << Options::displayHeight; _txtDisplayHeight->setText(ss.str());
}

void OptionsVideoState::handle(Action *action) {
	bool handled = false;
	ComboBox* openCb = nullptr; 
	if (_focusedControl) {
		openCb = dynamic_cast<ComboBox*>(_focusedControl);
		if (openCb && !openCb->isDropped()) { 
			openCb = nullptr;
		}
	}

	if (openCb) { 
		SDLKey sym = action->getDetails()->key.keysym.sym;
		if (sym == Options::keyMenuUp || sym == Options::keyMenuDown || sym == Options::keyMenuSelect || sym == Options::keyMenuCancel) {
			openCb->handle(action, this); 
			if (!openCb->isDropped()) { 
				setFocusOn(openCb); 
			}
			handled = true; 
		}
	} 
	else if (action->getDetails()->type == SDL_KEYDOWN) {
		SDLKey sym = action->getDetails()->key.keysym.sym;
		if (sym == Options::keyMenuUp) { cycleFocus(false); handled = true; }
		else if (sym == Options::keyMenuDown) { cycleFocus(true); handled = true; }
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
				if (_focusedControl == _btnDisplayResolutionUp) btnDisplayResolutionUpClick(&clickAction);
				else if (_focusedControl == _btnDisplayResolutionDown) btnDisplayResolutionDownClick(&clickAction);
				else if (_focusedControl == _btnLetterbox) btnLetterboxClick(&clickAction);
				else if (_focusedControl == _btnLockMouse) btnLockMouseClick(&clickAction);
				else if (_focusedControl == _btnRootWindowedMode) btnRootWindowedModeClick(&clickAction);
				else if (dynamic_cast<ComboBox*>(_focusedControl)) { _focusedControl->handle(&clickAction, this); }
				else if (dynamic_cast<TextEdit*>(_focusedControl)){ /* TextEdit handles its own Return via onKeyboardPress if set or no action for select */ }
			}
			handled = true;
		}
	}

	if (!handled) OptionsBaseState::handle(action); 
	
	if (!handled && action->getDetails()->type == SDL_KEYDOWN && action->getDetails()->key.keysym.sym == SDLK_g && (SDL_GetModState() & KMOD_CTRL) != 0) {
		_btnLockMouse->setPressed(Options::captureMouse == SDL_GRAB_ON);
	}
}

void OptionsVideoState::unpressRootWindowedMode() { _btnRootWindowedMode->setPressed(false); }

}
