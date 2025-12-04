#ifndef __GF2D_UI_H__
#define __GF2D_UI_H__

#include "gf2d_sprite.h"

typedef struct
{
	Sprite *sprite;

    Uint8 _inuse;

    GFC_Vector2D   position;
    GFC_Vector2D scale;
    GFC_Vector2D* center;
    float* rotation;
    GFC_Vector2D* flip;
    GFC_Color* colorShift;
    GFC_Vector4D* clip;
    Uint32     frame;

}UIElement;

void gf2d_ui_draw(UIElement* ui);

void gf2d_ui_draw_all();

void gf2d_ui_init(Uint32 ui_max);

UIElement* gf2d_ui_new();

void gf2d_ui_free(UIElement* ui);

void gf2d_ui_close();

#endif