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

#define FORBIDDEN_SYMBOL_EXCEPTION_printf
#define FORBIDDEN_SYMBOL_EXCEPTION_chdir
#define FORBIDDEN_SYMBOL_EXCEPTION_getcwd
#define FORBIDDEN_SYMBOL_EXCEPTION_getwd
#define FORBIDDEN_SYMBOL_EXCEPTION_mkdir
#define FORBIDDEN_SYMBOL_EXCEPTION_unlink

#include "engines/stark/gfx/opengl.h"

#include "common/system.h"

#ifdef USE_OPENGL

#define BITMAP_TEXTURE_SIZE 256

#ifdef SDL_BACKEND
#include <SDL_opengl.h>
#else
#include <GL/gl.h>
//#include <GL/glu.h>
#endif

namespace Stark {

OpenGLGfxDriver::OpenGLGfxDriver() {
}

OpenGLGfxDriver::~OpenGLGfxDriver() {
}

const char *OpenGLGfxDriver::getVideoDeviceName() {
	return "OpenGL Renderer";
}

void OpenGLGfxDriver::setupScreen(int screenW, int screenH, bool fullscreen) {
	g_system->setupScreen(screenW, screenH, fullscreen, true);

	_screenWidth = screenW;
	_screenHeight = screenH;
	_screenBPP = 24;
	/*
	_isFullscreen = g_system->getFeatureState(OSystem::kFeatureFullscreenMode);

	char GLDriver[1024];
	sprintf(GLDriver, "Residual: %s/%s", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	g_system->setWindowCaption(GLDriver);

	GLfloat ambientSource[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientSource);
	*/
}

void OpenGLGfxDriver::clearScreen() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLGfxDriver::flipBuffer() {
	g_system->updateScreen();
}

void OpenGLGfxDriver::drawSurface(const Graphics::Surface *surface, Common::Point dest, Common::Rect rect) {
	// Draw the whole surface by default
	if (rect.isEmpty())
		rect = Common::Rect(surface->w, surface->h);

	start2DMode();

	float rasterX = (2 * (float)dest.x / (float)_screenWidth);
	float rasterY = (2 * (float)dest.y / (float)_screenHeight);
	glRasterPos2f(-1.0f + rasterX, 1.0f - rasterY);
	glDrawPixels(surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	//glBegin(GL_QUADS); glVertex3i(-1, -1, -1); glVertex3i(1, -1, -1); glVertex3i(1, 1, -1); glVertex3i(-1, 1, -1); glEnd();

	end2DMode();
}


void OpenGLGfxDriver::start2DMode() {

	// Load the ModelView matrix with the identity
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Load the Projection matrix with the identity
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// Enable alpha blending
	glEnable(GL_BLEND);
	//glBlendEquation(GL_FUNC_ADD); // It's the default
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Flip the Y component
	glPixelZoom(1.0f, -1.0f);

	// Required by RGB sources, but not by RGBA
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void OpenGLGfxDriver::end2DMode() {
	// Disable alpha blending
	glDisable(GL_BLEND);

	// Pop the identity Projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	// Pop the identity ModelView matrix
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

/*
void OpenGLGfxDriver::setupCamera(float fov, float nclip, float fclip, float roll) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float right = nclip * tan(fov / 2 * (LOCAL_PI / 180));
	glFrustum(-right, right, -right * 0.75, right * 0.75, nclip, fclip);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(roll, 0, 0, -1);
}

void OpenGLGfxDriver::positionCamera(Graphics::Vector3d pos, Graphics::Vector3d interest) {
	Graphics::Vector3d up_vec(0, 0, 1);

	if (pos.x() == interest.x() && pos.y() == interest.y())
		up_vec = Graphics::Vector3d(0, 1, 0);

	gluLookAt(pos.x(), pos.y(), pos.z(), interest.x(), interest.y(), interest.z(), up_vec.x(), up_vec.y(), up_vec.z());
}

bool OpenGLGfxDriver::isHardwareAccelerated() {
	return true;
}

static void glShadowProjection(Graphics::Vector3d light, Graphics::Vector3d plane, Graphics::Vector3d normal, bool dontNegate) {
	// Based on GPL shadow projection example by
	// (c) 2002-2003 Phaetos <phaetos@gaffga.de>
	float d, c;
	float mat[16];
	float nx, ny, nz, lx, ly, lz, px, py, pz;

	// for some unknown for me reason normal need negation
	nx = -normal.x();
	ny = -normal.y();
	nz = -normal.z();
	if (dontNegate) {
		nx = -nx;
		ny = -ny;
		nz = -nz;
	}
	lx = light.x();
	ly = light.y();
	lz = light.z();
	px = plane.x();
	py = plane.y();
	pz = plane.z();

	d = nx * lx + ny * ly + nz * lz;
	c = px * nx + py * ny + pz * nz - d;

	mat[0] = lx * nx + c;
	mat[4] = ny * lx;
	mat[8] = nz * lx;
	mat[12] = -lx * c - lx * d;

	mat[1] = nx * ly;
	mat[5] = ly * ny + c;
	mat[9] = nz * ly;
	mat[13] = -ly * c - ly * d;

	mat[2] = nx * lz;
	mat[6] = ny * lz;
	mat[10] = lz * nz + c;
	mat[14] = -lz * c - lz * d;

	mat[3] = nx;
	mat[7] = ny;
	mat[11] = nz;
	mat[15] = -d;

	glMultMatrixf((GLfloat *)mat);
}
*/
void OpenGLGfxDriver::set3DMode() {
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}
/*
void OpenGLGfxDriver::translateViewpointStart(Graphics::Vector3d pos, float pitch, float yaw, float roll) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(pos.x(), pos.y(), pos.z());
	glRotatef(yaw, 0, 0, 1);
	glRotatef(pitch, 1, 0, 0);
	glRotatef(roll, 0, 1, 0);
}

void OpenGLGfxDriver::translateViewpointFinish() {
	glPopMatrix();
}
*/

void OpenGLGfxDriver::prepareMovieFrame(int width, int height, byte *bitmap) {
	// remove if already exist
	if (_movieNumTex > 0) {
		glDeleteTextures(_movieNumTex, _movieTexIds);
		delete[] _movieTexIds;
		_movieNumTex = 0;
	}
	
	// create texture
	_movieNumTex = ((width + (BITMAP_TEXTURE_SIZE - 1)) / BITMAP_TEXTURE_SIZE) *
	((height + (BITMAP_TEXTURE_SIZE - 1)) / BITMAP_TEXTURE_SIZE);
	_movieTexIds = new GLuint[_movieNumTex];
	glGenTextures(_movieNumTex, _movieTexIds);
	for (int i = 0; i < _movieNumTex; i++) {
		glBindTexture(GL_TEXTURE_2D, _movieTexIds[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BITMAP_TEXTURE_SIZE, BITMAP_TEXTURE_SIZE, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
	
	int curTexIdx = 0;
	for (int y = 0; y < height; y += BITMAP_TEXTURE_SIZE) {
		for (int x = 0; x < width; x += BITMAP_TEXTURE_SIZE) {
			int t_width = (x + BITMAP_TEXTURE_SIZE >= width) ? (width - x) : BITMAP_TEXTURE_SIZE;
			int t_height = (y + BITMAP_TEXTURE_SIZE >= height) ? (height - y) : BITMAP_TEXTURE_SIZE;
			glBindTexture(GL_TEXTURE_2D, _movieTexIds[curTexIdx]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, t_width, t_height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, bitmap + (y * 2 * width) + (2 * x));
			curTexIdx++;
		}
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	_movieWidth = width;
	_movieHeight = height;
}

void OpenGLGfxDriver::drawMovieFrame(int offsetX, int offsetY) {
	// prepare view
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, _screenWidth, _screenHeight, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	// A lot more may need to be put there : disabling Alpha test, blending, ...
	// For now, just keep this here :-)
	
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	// draw
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_SCISSOR_TEST);
	
	glScissor(offsetX, _screenHeight - (offsetY + _movieHeight), _movieWidth, _movieHeight);
	
	int curTexIdx = 0;
	for (int y = 0; y < _movieHeight; y += BITMAP_TEXTURE_SIZE) {
		for (int x = 0; x < _movieWidth; x += BITMAP_TEXTURE_SIZE) {
			glBindTexture(GL_TEXTURE_2D, _movieTexIds[curTexIdx]);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2i(x + offsetX, y + offsetY);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2i(x + offsetX + BITMAP_TEXTURE_SIZE, y + offsetY);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2i(x + offsetX + BITMAP_TEXTURE_SIZE, y + offsetY + BITMAP_TEXTURE_SIZE);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2i(x + offsetX, y + offsetY + BITMAP_TEXTURE_SIZE);
			glEnd();
			curTexIdx++;
		}
	}
	
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void OpenGLGfxDriver::releaseMovieFrame() {
	if (_movieNumTex > 0) {
		glDeleteTextures(_movieNumTex, _movieTexIds);
		delete[] _movieTexIds;
		_movieNumTex = 0;
	}
}
	
} // End of namespace Stark

#endif // USE_OPENGL
