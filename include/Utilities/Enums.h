#pragma once


enum class Mode { OBJECT, EDIT, SCULPT, WEIGHT_PAINT, TEXTURE_PAINT, UV_EDIT, SHADER_EDIT };
enum class SelectMode { VERTEX, EDGE, FACE };
enum class RenderMode { WIREFRAME, SOLID, MATERIAL_PREVIEW, RENDER };

enum class Operation { TRANSLATE, ROTATE, SCALE };

enum class CameraTypes { VIEWPORT, UV, SHADER, COUNT };

enum class ViewportBoundary { LEFT, BOTTOM, RIGHT,TOP, NONE };
