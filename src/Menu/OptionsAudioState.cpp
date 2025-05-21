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
#include "OptionsAudioState.h"
#include <SDL_mixer.h>
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/ComboBox.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/ToggleTextButton.h"
#include "../Interface/Slider.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"
#include "../Engine/Sound.h"

namespace OpenXcom
{
/* MUS_NONE, MUS_CMD, MUS_WAV, MUS_MOD, MUS_MID, MUS_OGG, MUS_MP3, MUS_MP3_MAD, MUS_FLAC, MUS_MODPLUG */
const std::string OptionsAudioState::musFormats[] = {"Adlib", "?", "WAV", "MOD", "MIDI", "OGG", "MP3", "MP3", "FLAC", "MOD"};
const std::string OptionsAudioState::sndFormats[] = {"?", "1.4", "1.0"};

/**
 * Initializes all the elements in the Audio Options screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
OptionsAudioState::OptionsAudioState(OptionsOrigin origin) : OptionsBaseState(origin)
{
	setCategory(_btnAudio);

	// Create objects
	_txtMusicVolume = new Text(114, 9, 94, 8);
	_slrMusicVolume = new Slider(104, 16, 94, 18);

	_txtSoundVolume = new Text(114, 9, 94, 40);
	_slrSoundVolume = new Slider(104, 16, 94, 50);

	_txtUiVolume = new Text(114, 9, 94, 72);
	_slrUiVolume = new Slider(104, 16, 94, 82);

	_txtMusicFormat = new Text(114, 9, 206, 40);
	_cbxMusicFormat = new ComboBox(this, 104, 16, 206, 50);
	_txtCurrentMusic = new Text(114, 9, 206, 68);

	_txtSoundFormat = new Text(114, 9, 206, 82);
	_cbxSoundFormat = new ComboBox(this, 104, 16, 206, 92);
	_txtCurrentSound = new Text(114, 9, 206, 110);

	_txtVideoFormat = new Text(114, 9, 206, 8);
	_cbxVideoFormat = new ComboBox(this, 104, 16, 206, 18);

	_txtOptions = new Text(114, 9, 94, 104);
	_btnBackgroundMute = new ToggleTextButton(104, 16, 94, 114);

	add(_txtMusicVolume, "text", "audioMenu");
	add(_slrMusicVolume, "button", "audioMenu");

	add(_txtSoundVolume, "text", "audioMenu");
	add(_slrSoundVolume, "button", "audioMenu");

	add(_txtUiVolume, "text", "audioMenu");
	add(_slrUiVolume, "button", "audioMenu");

	add(_txtVideoFormat, "text", "audioMenu");
	add(_txtMusicFormat, "text", "audioMenu");
	add(_txtCurrentMusic, "text", "audioMenu");
	add(_txtSoundFormat, "text", "audioMenu");
	add(_txtCurrentSound, "text", "audioMenu");

	add(_cbxSoundFormat, "button", "audioMenu");
	add(_cbxMusicFormat, "button", "audioMenu");
	add(_cbxVideoFormat, "button", "audioMenu");

	add(_txtOptions, "text", "audioMenu");
	add(_btnBackgroundMute, "button", "audioMenu");

	centerAllSurfaces();

	// Set up objects
	_txtMusicVolume->setText(tr("STR_MUSIC_VOLUME"));

	_slrMusicVolume->setRange(0, SDL_MIX_MAXVOLUME);
	_slrMusicVolume->setValue(Options::musicVolume);
	_slrMusicVolume->onChange((ActionHandler)&OptionsAudioState::slrMusicVolumeChange);
	_slrMusicVolume->setTooltip("STR_MUSIC_VOLUME_DESC");
	_slrMusicVolume->onMouseIn((ActionHandler)&OptionsAudioState::txtTooltipIn);
	_slrMusicVolume->onMouseOut((ActionHandler)&OptionsAudioState::txtTooltipOut);

	_txtSoundVolume->setText(tr("STR_SFX_VOLUME"));

	_slrSoundVolume->setRange(0, SDL_MIX_MAXVOLUME);
	_slrSoundVolume->setValue(Options::soundVolume);
	_slrSoundVolume->onChange((ActionHandler)&OptionsAudioState::slrSoundVolumeChange);
	_slrSoundVolume->onMouseRelease((ActionHandler)&OptionsAudioState::slrSoundVolumeRelease);
	_slrSoundVolume->setTooltip("STR_SFX_VOLUME_DESC");
	_slrSoundVolume->onMouseIn((ActionHandler)&OptionsAudioState::txtTooltipIn);
	_slrSoundVolume->onMouseOut((ActionHandler)&OptionsAudioState::txtTooltipOut);

	_txtUiVolume->setText(tr("STR_UI_VOLUME"));

	_slrUiVolume->setRange(0, SDL_MIX_MAXVOLUME);
	_slrUiVolume->setValue(Options::uiVolume);
	_slrUiVolume->onChange((ActionHandler)&OptionsAudioState::slrUiVolumeChange);
	_slrUiVolume->onMouseRelease((ActionHandler)&OptionsAudioState::slrUiVolumeRelease);
	_slrUiVolume->setTooltip("STR_UI_VOLUME_DESC");
	_slrUiVolume->onMouseIn((ActionHandler)&OptionsAudioState::txtTooltipIn);
	_slrUiVolume->onMouseOut((ActionHandler)&OptionsAudioState::txtTooltipOut);

	std::vector<std::string> musicText, soundText, videoText;
	/* MUSIC_AUTO, MUSIC_FLAC, MUSIC_OGG, MUSIC_MP3, MUSIC_MOD, MUSIC_WAV, MUSIC_ADLIB, MUSIC_GM, MUSIC_MIDI */
	musicText.push_back(tr("STR_PREFERRED_FORMAT_AUTO"));
	musicText.push_back("FLAC");
	musicText.push_back("OGG");
	musicText.push_back("MP3");
	musicText.push_back("MOD");
	musicText.push_back("WAV");
	musicText.push_back("Adlib");
	musicText.push_back("GM");
	musicText.push_back("MIDI");

	soundText.push_back(tr("STR_PREFERRED_FORMAT_AUTO"));
	soundText.push_back("1.4");
	soundText.push_back("1.0");

	videoText.push_back(tr("STR_PREFERRED_VIDEO_ANIMATION"));
	videoText.push_back(tr("STR_PREFERRED_VIDEO_SLIDESHOW"));

	_txtMusicFormat->setText(tr("STR_PREFERRED_MUSIC_FORMAT"));

	_cbxMusicFormat->setOptions(musicText);
	_cbxMusicFormat->setSelected(Options::preferredMusic);
	_cbxMusicFormat->setTooltip("STR_PREFERRED_MUSIC_FORMAT_DESC");
	_cbxMusicFormat->onChange((ActionHandler)&OptionsAudioState::cbxMusicFormatChange);
	_cbxMusicFormat->onMouseIn((ActionHandler)&OptionsAudioState::txtTooltipIn);
	_cbxMusicFormat->onMouseOut((ActionHandler)&OptionsAudioState::txtTooltipOut);

	std::string curMusic = musFormats[Mix_GetMusicType(0)];
	_txtCurrentMusic->setText(tr("STR_CURRENT_FORMAT").arg(curMusic));

	_txtSoundFormat->setText(tr("STR_PREFERRED_SFX_FORMAT"));

	_cbxSoundFormat->setOptions(soundText);
	_cbxSoundFormat->setSelected(Options::preferredSound);
	_cbxSoundFormat->setTooltip("STR_PREFERRED_SFX_FORMAT_DESC");
	_cbxSoundFormat->onChange((ActionHandler)&OptionsAudioState::cbxSoundFormatChange);
	_cbxSoundFormat->onMouseIn((ActionHandler)&OptionsAudioState::txtTooltipIn);
	_cbxSoundFormat->onMouseOut((ActionHandler)&OptionsAudioState::txtTooltipOut);

	std::string curSound = sndFormats[Options::currentSound];
	_txtCurrentSound->setText(tr("STR_CURRENT_FORMAT").arg(curSound));

	_txtVideoFormat->setText(tr("STR_PREFERRED_VIDEO_FORMAT"));

	_cbxVideoFormat->setOptions(videoText);
	_cbxVideoFormat->setSelected(Options::preferredVideo);
	_cbxVideoFormat->setTooltip("STR_PREFERRED_VIDEO_FORMAT_DESC");
	_cbxVideoFormat->onChange((ActionHandler)&OptionsAudioState::cbxVideoFormatChange);
	_cbxVideoFormat->onMouseIn((ActionHandler)&OptionsAudioState::txtTooltipIn);
	_cbxVideoFormat->onMouseOut((ActionHandler)&OptionsAudioState::txtTooltipOut);

	// These options require a restart, so don't enable them in-game
	_txtMusicFormat->setVisible(_origin == OPT_MENU);
	_cbxMusicFormat->setVisible(_origin == OPT_MENU);
	_txtCurrentMusic->setVisible(_origin == OPT_MENU);

	// These options only apply to UFO
	_txtSoundFormat->setVisible(_origin == OPT_MENU && _game->getMod()->getSoundDefinitions()->empty());
	_cbxSoundFormat->setVisible(_origin == OPT_MENU && _game->getMod()->getSoundDefinitions()->empty());
	_txtCurrentSound->setVisible(_origin == OPT_MENU && _game->getMod()->getSoundDefinitions()->empty());

	_txtOptions->setText(tr("STR_SOUND_OPTIONS"));

	_btnBackgroundMute->setText(tr("STR_BACKGROUND_MUTE"));
	_btnBackgroundMute->setPressed(Options::backgroundMute);
	_btnBackgroundMute->onMouseClick((ActionHandler)&OptionsAudioState::btnBackgroundMuteClick);
	_btnBackgroundMute->setTooltip("STR_BACKGROUND_MUTE_DESC");
	_btnBackgroundMute->onMouseIn((ActionHandler)&OptionsAudioState::txtTooltipIn);
	_btnBackgroundMute->onMouseOut((ActionHandler)&OptionsAudioState::txtTooltipOut);

	// Populate navigable controls
	_navigableControls.push_back(_slrMusicVolume);
	_navigableControls.push_back(_slrSoundVolume);
	_navigableControls.push_back(_slrUiVolume);
	if (_cbxVideoFormat->getVisible()) _navigableControls.push_back(_cbxVideoFormat);
	if (_cbxMusicFormat->getVisible()) _navigableControls.push_back(_cbxMusicFormat);
	if (_cbxSoundFormat->getVisible()) _navigableControls.push_back(_cbxSoundFormat);
	_navigableControls.push_back(_btnBackgroundMute);
	// OK, Cancel, Default buttons are not part of this cycle for now.

	_focusedControl = nullptr;
	_focusedIndex = -1;
}

/**
 * Initializes UI colors according to origin and sets up initial focus.
 */
void OptionsAudioState::init()
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
void OptionsAudioState::setFocusedControlVisuals(InteractiveSurface* control, bool focused)
{
    if (!control) return;

    ToggleTextButton* ttb = dynamic_cast<ToggleTextButton*>(control);
    ComboBox* cb = dynamic_cast<ComboBox*>(control);
    Slider* sl = dynamic_cast<Slider*>(control);

    if (ttb) ttb->setDown(focused); // Using setDown for visual focus
    else if (cb) cb->setFocused(focused);
    else if (sl) {
        // Slider doesn't have a specific setFocused method.
        // We could change its button's color or frame color if needed,
        // but for now, just the generic setHasFocus.
        // Its internal button might get a visual cue if setDown was propagated.
    }
    
    control->setHasFocus(focused);
}

/**
 * Sets focus to a specific control.
 * @param control The control to focus.
 */
void OptionsAudioState::setFocusOn(InteractiveSurface* control)
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
void OptionsAudioState::cycleFocus(bool forward)
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
void OptionsAudioState::handle(Action *action)
{
	bool handled = false;

	ComboBox* openComboBox = nullptr;
	if (_focusedControl) { // Check only if there's a focused control that could be an open combobox
		openComboBox = dynamic_cast<ComboBox*>(_focusedControl);
		if (openComboBox && !openComboBox->isOpen()) {
			openComboBox = nullptr; // Not interested if it's not open
		}
	}
	
	if (openComboBox) { // Input goes to the open ComboBox's list
		TextList* cbList = openComboBox->getList();
		if (cbList && action->getDetails()->type == SDL_KEYDOWN) {
			SDLKey sym = action->getDetails()->key.keysym.sym;
			if (sym == Options::keyMenuUp) {
				cbList->selectPrevious();
				handled = true;
			} else if (sym == Options::keyMenuDown) {
				cbList->selectNext();
				handled = true;
			} else if (sym == Options::keyMenuSelect) {
				// ComboBox's onListClick should handle selection and closing
				// Simulate this by calling activateSelected on its list.
				cbList->activateSelected(this); // 'this' is OptionsAudioState
				// ComboBox::onListClick will be called, which calls ComboBox::setSelected and ComboBox::toggle
				setFocusOn(openComboBox); // Return focus to the ComboBox itself after selection
				handled = true;
			} else if (sym == Options::keyMenuCancel) {
				openComboBox->toggle(this); // Close the ComboBox
				setFocusOn(openComboBox); // Return focus to the ComboBox itself
				handled = true;
			}
		}
	} else if (action->getDetails()->type == SDL_KEYDOWN) {
		SDLKey sym = action->getDetails()->key.keysym.sym;
		int sliderStep = SDL_MIX_MAXVOLUME / 20; // Approx 5% step for volume sliders

		if (sym == Options::keyMenuUp) {
			cycleFocus(false);
			handled = true;
		} else if (sym == Options::keyMenuDown) {
			cycleFocus(true);
			handled = true;
		} else if (sym == Options::keyMenuSelect) {
			if (_focusedControl) {
				SDL_Event sdlEvent;
				sdlEvent.type = SDL_MOUSEBUTTONDOWN;
				sdlEvent.button.button = SDL_BUTTON_LEFT;
				sdlEvent.button.state = SDL_PRESSED;
				Action clickAction(&sdlEvent, _game->getScreen()->getXScale(), _game->getScreen()->getYScale(), _game->getScreen()->getTopBlackBand(), _game->getScreen()->getLeftBlackBand());
				clickAction.setSender(_focusedControl);

				if (_focusedControl == _btnBackgroundMute) btnBackgroundMuteClick(&clickAction);
				else if (dynamic_cast<ComboBox*>(_focusedControl)) {
					((ComboBox*)_focusedControl)->toggle(this);
					// If it opened, set focus to its list? Or let ComboBox handle it.
					// For now, ComboBox itself remains focused. Next Up/Down will navigate its list due to openComboBox check.
				}
				// Sliders don't typically respond to 'select' in this way.
			}
			handled = true;
		} else if (sym == Options::keyMenuLeft) {
			Slider* sl = dynamic_cast<Slider*>(_focusedControl);
			if (sl) {
				// Create a dummy action for the slider's onChange handler
				SDL_Event dummyEvent; dummyEvent.type = SDL_USEREVENT; 
				Action sliderAction(&dummyEvent, 1.0,1.0,0,0);
				sl->decrement(this, &sliderAction, sliderStep);
				// Manually call the specific on-release handler if needed for sound preview
				if (sl == _slrSoundVolume) slrSoundVolumeRelease(&sliderAction);
				else if (sl == _slrUiVolume) slrUiVolumeRelease(&sliderAction);
			}
			handled = true;
		} else if (sym == Options::keyMenuRight) {
			Slider* sl = dynamic_cast<Slider*>(_focusedControl);
			if (sl) {
				SDL_Event dummyEvent; dummyEvent.type = SDL_USEREVENT;
				Action sliderAction(&dummyEvent, 1.0,1.0,0,0);
				sl->increment(this, &sliderAction, sliderStep);
				if (sl == _slrSoundVolume) slrSoundVolumeRelease(&sliderAction);
				else if (sl == _slrUiVolume) slrUiVolumeRelease(&sliderAction);
			}
			handled = true;
		}
	}

	if (!handled) {
		OptionsBaseState::handle(action);
	}
}

/**
 *
 */
OptionsAudioState::~OptionsAudioState()
{

}

/**
 * Updates the music volume.
 * @param action Pointer to an action.
 */
void OptionsAudioState::slrMusicVolumeChange(Action *)
{
	Options::musicVolume = _slrMusicVolume->getValue();
	_game->setVolume(Options::soundVolume, Options::musicVolume, Options::uiVolume);
}

/**
 * Updates the sound volume with the slider.
 * @param action Pointer to an action.
 */
void OptionsAudioState::slrSoundVolumeChange(Action *)
{
	Options::soundVolume = _slrSoundVolume->getValue();
	_game->setVolume(Options::soundVolume, Options::musicVolume, Options::uiVolume);
}

/**
 * Plays a game sound for volume preview.
 * @param action Pointer to an action.
 */
void OptionsAudioState::slrSoundVolumeRelease(Action *)
{
	_game->getMod()->getSound("GEO.CAT", Mod::UFO_FIRE)->play();
}

/**
 * Updates the UI volume with the slider.
 * @param action Pointer to an action.
 */
void OptionsAudioState::slrUiVolumeChange(Action *)
{
	Options::uiVolume = _slrUiVolume->getValue();
	_game->setVolume(Options::soundVolume, Options::musicVolume, Options::uiVolume);
}

/**
 * Plays a UI sound for volume preview.
 * @param action Pointer to an action.
 */
void OptionsAudioState::slrUiVolumeRelease(Action *)
{
	TextButton::soundPress->play(Mix_GroupAvailable(0));
}

/**
 * Changes the Video Format option.
 * @param action Pointer to an action.
 */
void OptionsAudioState::cbxVideoFormatChange(Action *)
{
	Options::preferredVideo = (VideoFormat)_cbxVideoFormat->getSelected();
}

/**
 * Changes the Music Format option.
 * @param action Pointer to an action.
 */
void OptionsAudioState::cbxMusicFormatChange(Action *)
{
	Options::preferredMusic = (MusicFormat)_cbxMusicFormat->getSelected();
	Options::reload = true;
}

/**
 * Changes the Sound Format option.
 * @param action Pointer to an action.
 */
void OptionsAudioState::cbxSoundFormatChange(Action *)
{
	Options::preferredSound = (SoundFormat)_cbxSoundFormat->getSelected();
	Options::reload = true;
}

/**
 * Updates the Background Mute option.
 * @param action Pointer to an action.
 */
void OptionsAudioState::btnBackgroundMuteClick(Action*)
{
	Options::backgroundMute = _btnBackgroundMute->getPressed();
}

}
