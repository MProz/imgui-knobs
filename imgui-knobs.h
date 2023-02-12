#pragma once

#include <cstdlib>
#include <imgui.h>

typedef int ImGuiKnobFlags;

enum ImGuiKnobFlags_ {
    ImGuiKnobFlags_NoTitle = 1 << 0,
    ImGuiKnobFlags_NoInput = 1 << 1,
    ImGuiKnobFlags_ValueTooltip = 1 << 2,
    ImGuiKnobFlags_DragHorizontal = 1 << 3,
};

typedef int ImGuiKnobButtonStyle;

enum ImGuiKnobButtonStyle_ {
    ImGuiKnobButtonStyle_None = 1 << 0,
    ImGuiKnobButtonStyle_Blank = 1 << 1,
    ImGuiKnobButtonStyle_Tick = 1 << 2,
    ImGuiKnobButtonStyle_Dot = 1 << 3,
};

typedef int ImGuiKnobBorderStyle;

enum ImGuiKnobBorderStyle_ {
    ImGuiKnobBorderStyle_None = 1 << 0,
    ImGuiKnobBorderStyle_Ticks = 1 << 1,
    ImGuiKnobBorderStyle_WiperDot = 1 << 3,
    ImGuiKnobBorderStyle_WiperTick = 1 << 4,
};

typedef int ImGuiKnobFillStyle;

enum ImGuiKnobFillStyle_ {
    ImGuiKnobFillStyle_NoFill = 1 << 0,
    ImGuiKnobFillStyle_LeftFill = 1 << 1,
    ImGuiKnobFillStyle_CenterFill = 1 << 2,
    ImGuiKnobFillStyle_RightFill = 1 << 3,
};

namespace ImGuiKnobs {

    struct color_set {
        ImColor base;
        ImColor hovered;
        ImColor active;

        color_set(ImColor base, ImColor hovered, ImColor active) : base(base), hovered(hovered), active(active) {}

        color_set(ImColor color) {
            base = color;
            hovered = color;
            active = color;
        }
    };

    bool Knob(const char *label, float *p_value, float v_min, float v_max, float speed = 0, const char *format = NULL, float size = 0,
     ImGuiKnobButtonStyle button_style = ImGuiKnobButtonStyle_Tick, ImGuiKnobBorderStyle border_style = ImGuiKnobBorderStyle_None,
     ImGuiKnobFillStyle wiper_style = ImGuiKnobFillStyle_NoFill, ImGuiKnobFlags flags = 0, int steps = 10);
    bool KnobInt(const char *label, int *p_value, int v_min, int v_max, float speed = 0, const char *format = NULL, float size = 0,
    ImGuiKnobButtonStyle button_style = ImGuiKnobButtonStyle_Tick, ImGuiKnobBorderStyle border_style = ImGuiKnobBorderStyle_None,
     ImGuiKnobFillStyle wiper_style = ImGuiKnobFillStyle_NoFill, ImGuiKnobFlags flags = 0, int steps = 10);
}// namespace ImGuiKnobs
