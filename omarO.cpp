//Omar Thor
//Group 4
//DeepSea Survival Game

#include "game.h"

void handlePlayerMovement(const XEvent& event)
{
    if (event.type == KeyPress) {
        int key = XLookupKeysym(const_cast<XKeyEvent*> (&event.xkey), 0);
        if (key == XK_Up) {
            game.playerMovementDirectionMask |= DirUp;
        }
        if (key == XK_Down) {
            game.playerMovementDirectionMask |= DirDown;
        }
        if (key == XK_Left) {
            game.playerMovementDirectionMask |= DirLeft;
        }
        if (key == XK_Right) {
            game.playerMovementDirectionMask |= DirRight;
        }
    } else if (event.type == KeyRelease) {
        int key = XLookupKeysym(const_cast<XKeyEvent*> (&event.xkey), 0);
        if (key == XK_Up) {
            game.playerMovementDirectionMask &= ~DirUp;
        }
        if (key == XK_Down) {
            game.playerMovementDirectionMask &= ~DirDown;
        }
        if (key == XK_Left) {
            game.playerMovementDirectionMask &= ~DirLeft;
        }
        if (key == XK_Right) {
            game.playerMovementDirectionMask &= ~DirRight;
        }
    }
}
