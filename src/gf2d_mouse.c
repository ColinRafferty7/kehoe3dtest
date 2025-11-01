#include <SDL.h>

#include "gf3d_vgraphics.h"
#include "gf3d_camera.h"

#include "gf2d_actor.h"
#include "gf2d_mouse.h"


typedef struct
{
    Uint32 buttons;         /**<buttons mask*/
    GFC_Vector2D delta;  /**<position of mouse*/
}MouseState;

typedef struct
{
    MouseState  mouse[2];   /**<mouse state for the current and last frames*/
    Actor      *actor;      /**<mouse actor*/
    GFC_Action *action;
    float       frame;
    Uint8       hidden;     /**<if true, don't show mouse or use its inputs*/
}Mouse;

static Mouse _mouse = {0};

int gf2d_mouse_hidden()
{
    return (_mouse.hidden > 0);
}

void gf2d_mouse_hide()
{
    _mouse.hidden++;
}

void gf2d_mouse_show()
{
    _mouse.hidden--;
}

void gf2d_mouse_set_action(const char *action)
{
    _mouse.action = gf2d_actor_get_action(_mouse.actor, action,&_mouse.frame);
}

void gf2d_mouse_load(const char *actorFile)
{
    gf2d_actor_free(_mouse.actor);
    _mouse.actor = gf2d_actor_load(actorFile);
    gf2d_mouse_set_action("default");
}

void gf2d_mouse_update()
{
    int dx, dy;
    gfc_action_next_frame(_mouse.action, &_mouse.frame);
    memcpy(&_mouse.mouse[1], &_mouse.mouse[0], sizeof(MouseState));

    if (SDL_GetRelativeMouseMode())
    {
        _mouse.mouse[0].buttons = SDL_GetRelativeMouseState(&dx, &dy);
        gfc_vector2d_set(_mouse.mouse[0].delta, dx, dy);
    }
    else
    {
        int x, y;
        _mouse.mouse[0].buttons = SDL_GetMouseState(&x, &y);
        gfc_vector2d_set(_mouse.mouse[0].delta, x - _mouse.mouse[1].delta.x, y - _mouse.mouse[1].delta.y);
    }
}

void gf2d_mouse_draw()
{
    if (_mouse.hidden)return;
    gf2d_actor_draw(
        _mouse.actor,
        _mouse.frame,
        _mouse.mouse[0].delta,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
}

int gf2d_mouse_moved()
{
    if ((_mouse.mouse[0].delta.x != _mouse.mouse[1].delta.x) ||
        (_mouse.mouse[0].delta.y != _mouse.mouse[1].delta.y) ||
        (_mouse.mouse[0].buttons != _mouse.mouse[1].buttons))
    {
        return 1;
    }
    return 0;
}

int gf2d_mouse_button_pressed(int button)
{
    int mask;
    if (_mouse.hidden)return 0;
    mask = 1 << button;
    if ((_mouse.mouse[0].buttons & mask) &&
        !(_mouse.mouse[1].buttons & mask))
    {
        return 1;
    }
    return 0;
}

int gf2d_mouse_button_held(int button)
{
    int mask;
    if (_mouse.hidden)return 0;
    mask = 1 << button;
    if ((_mouse.mouse[0].buttons & mask) &&
        (_mouse.mouse[1].buttons & mask))
    {
        return 1;
    }
    return 0;
}

int gf2d_mouse_button_released(int button)
{
    int mask;
    if (_mouse.hidden)return 0;
    mask = 1 << button;
    if (!(_mouse.mouse[0].buttons & mask) &&
        (_mouse.mouse[1].buttons & mask))
    {
        return 1;
    }
    return 0;
}

int gf2d_mouse_button_state(int button)
{
    int mask;
    mask = 1 << button;
    return (_mouse.mouse[0].buttons & mask);
}

float gf2d_mouse_get_angle_to(GFC_Vector2D point)
{
    GFC_Vector2D delta;
    gfc_vector2d_sub(delta,_mouse.mouse[0].delta,point);
    return gfc_vector2d_angle(delta);
}

GFC_Vector2D gf2d_mouse_get_position()
{
    return _mouse.mouse[0].delta;
}

GFC_Vector2D gf2d_mouse_get_movement()
{
    return _mouse.mouse[0].delta;
}

int gf2d_mouse_in_rect(GFC_Rect r)
{
    return gfc_point_in_rect(_mouse.mouse[0].delta,r);
}

/*eol@eof*/
