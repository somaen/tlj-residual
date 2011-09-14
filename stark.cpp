/* Residual - A 3D game interpreter
 *
 * Residual is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the AUTHORS
 * file distributed with this source distribution.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
 * $URL$
 * $Id$
 *
 */

#include "engines/stark/stark.h"
#include "engines/stark/debug.h"

#include "common/config-manager.h"
#include "common/events.h"
#include "common/system.h"
#include "audio/mixer.h"

namespace Stark {
	
StarkEngine *g_stark = NULL;

StarkEngine::StarkEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst), _gameDescription(gameDesc), _gfx(NULL), _scene(NULL) {
	_mixer->setVolumeForSoundType(Audio::Mixer::kPlainSoundType, 127);
	_mixer->setVolumeForSoundType(Audio::Mixer::kSFXSoundType, ConfMan.getInt("sfx_volume"));
	_mixer->setVolumeForSoundType(Audio::Mixer::kSpeechSoundType, ConfMan.getInt("speech_volume"));
	_mixer->setVolumeForSoundType(Audio::Mixer::kMusicSoundType, ConfMan.getInt("music_volume"));

	g_stark = this;
	_isPlayingFMV = false;
	_binkPlayer = CreateBinkPlayer();
	
	// Add the available debug channels
	DebugMan.addDebugChannel(kDebugArchive, "Archive", "Debug the archive loading");
	DebugMan.addDebugChannel(kDebugXMG, "XMG", "Debug the loading of XMG images");
	DebugMan.addDebugChannel(kDebugXRC, "XRC", "Debug the loading of XRC resource trees");
	DebugMan.addDebugChannel(kDebugUnknown, "Unknown", "Debug unknown values on the data");
}

StarkEngine::~StarkEngine() {
	delete _scene;
}

Common::Error StarkEngine::run() {
	_gfx = GfxDriver::create();

	// Get the screen prepared
	_gfx->setupScreen(640, 480, ConfMan.getBool("fullscreen"));

	// FIXME: This is just here to strap up the Prologue-video for now
	//_binkPlayer->play("global/0602.bbb", false, 0, 0);
	//_isPlayingFMV = true;
	
	// Start running
	mainLoop();

	return Common::kNoError;
}

void StarkEngine::mainLoop() {
	// Load the initial scene
	_scene = new Scene(_gfx);

	while (!shouldQuit()) {
		// Process events
		Common::Event e;
		while (g_system->getEventManager()->pollEvent(e)) {
			// Handle any buttons, keys and joystick operations
			if (e.type == Common::EVENT_KEYDOWN) {
				if (e.kbd.ascii == 'q') {
					quitGame();
					break;
				} else {
					//handleChars(event.type, event.kbd.keycode, event.kbd.flags, event.kbd.ascii);
				}
			}
			/*if (event.type == Common::EVENT_KEYDOWN || event.type == Common::EVENT_KEYUP) {
				handleControls(event.type, event.kbd.keycode, event.kbd.flags, event.kbd.ascii);
			}*/
			// Check for "Hard" quit"
			//if (e.type == Common::EVENT_QUIT)
			//	return;
			/*if (event.type == Common::EVENT_SCREEN_CHANGED)
				_refreshDrawNeeded = true;*/
		}

		updateDisplayScene();
		g_system->delayMillis(50);
	}
}

void StarkEngine::updateDisplayScene() {
	// Get frame delay
	static uint32 lastFrame = g_system->getMillis();
	uint32 delta = g_system->getMillis() - lastFrame;
	lastFrame += delta;

	// Clear the screen
	_gfx->clearScreen();

	if (_isPlayingFMV) {
		if (_binkPlayer->isUpdateNeeded()) {
			
		}
	} else {
		// Render the current scene
		_scene->render(delta);
	}

	// Swap buffers
	_gfx->flipBuffer();
}

} // End of namespace Stark
