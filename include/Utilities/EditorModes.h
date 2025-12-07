#pragma once

enum class Mode { OBJECT, EDIT, SCULPT, WEIGHT_PAINT, TEXTURE_PAINT, UV_EDIT, SHADER_EDIT };
enum class SelectMode { VERTEX, EDGE, FACE };
enum class RenderMode { WIREFRAME, SOLID, MATERIAL_PREVIEW, RENDER };


enum class FragColor
{
    Default = 0,
    ActiveSelection = 1,
    NonActiveSelection = 2,
    Grid = 3,
    BVH = 4,
    Light = 5,
    UV = 6,
    Black = 7,
    Seams = 8
};
