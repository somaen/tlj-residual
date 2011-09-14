MODULE := engines/stark

MODULE_OBJS := \
	actor.o \
	adpcm.o \
	archive.o \
	detection.o \
	gfx/coordinate.o \
	gfx/driver.o \
	gfx/opengl.o \
	gfx/tinygl.o \
	movie/bink.o \
	movie/movie.o \
	movie/smacker.o \
	movie/codecs/smk_decoder.cpp \
	scene.o \
	skeleton.o \
	skeleton_anim.o \
	sound.o \
	stark.o \
	texture.o \
	xmg.o \
	xrc.o

# Include common rules
include $(srcdir)/rules.mk
