#pragma once
#pragma comment(lib, "OpenGL32.lib")
#include "stdafx.h"
#include <gl/GL.h>
#include "mem.h"
#include "glDraw.h"

namespace rgb
{
	const GLubyte red[3] = { 255, 0, 0 };
	const GLubyte green[3] = { 0, 255, 0 };
	const GLubyte gray[3] = { 55, 55, 55 };
	const GLubyte lightgray[3] = { 192, 192, 192 };
	const GLubyte black[3] = { 255,0,0 };
}

namespace GL
{
	void SetupOrtho();
	void RestoreGL();

	void DrawFilledRect(float x, float y, float width, float height, const GLubyte color[3]);
	void Drawoutline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3]);
}