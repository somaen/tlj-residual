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

#ifndef STARK_SCENE_H
#define STARK_SCENE_H

#include "common/array.h"
#include "engines/stark/sceneelement.h"

namespace Stark {

/** 
 * Manager for the current game scene
 */
class Scene {
public:
	Scene(GfxDriver *gfx);
	~Scene();

	/**
	 * Render the scene
	 *
	 * @param delta Time offset (in ms) since last frame render
	 */
	void render(uint32 delta);

private:
	GfxDriver *_gfx;
	Common::Array<SceneElement *> _elements;
};

} // End of namespace Stark

#endif // STARK_SCENE_H
