# ImGui Knobs
This is a port/adaptation of [imgui-rs-knobs](https://github.com/DGriffin91/imgui-rs-knobs), for C++.
This port contains modifications which break compatibility with the original Rust implementation.

![example image](https://github.com/MProz/imgui-knobs/blob/main/Examples.png)

## Usage
Add `imgui-knobs.cpp` and `imgui-knobs.h` to your project and include `imgui-knobs.h` in some source file.

TODO: Add image of examples of these styles in practice

```cpp
static float value = 0;

if (ImGuiKnobs::Knob("Volume", &value, -6.0f, 6.0f, 0.1f, "%.1fdB")) {
    // value was changed
}
```

Draw knobs using either `Knob` or `KnobInt`. The API is:

```
bool ImGuiKnobs::Knob(label, *value, min, max, [speed, format, size, button_style, border_style, fill_style, flags, steps])
bool ImGuiKnobs::KnobInt(label, *value, min, max, [speed, format, size, button_style, border_style, fill_style, flags, steps])
```

### Styles
`button_style` determines the visual look of the clickable part of the knob. Available button styles are: `ImGuiKnobButtonStyle_None`, `ImGuiKnobButtonStyle_Blank`, `ImGuiKnobButtonStyle_Tick`, `ImGuiKnobButtonStyle_Dot`.

`border_style` determines the visual look of the outer track of the knob. Available border styles are: `ImGuiKnobBorderStyle_None`, `ImGuiKnobBorderStyle_Ticks`, `ImGuiKnobBorderStyle_WiperDot`, `ImGuiKnobBorderStyle_WiperTick`.

`fill_style` determines how the border specified above is filled. This only applies to the wiper styles. Available fill styles are: `ImGuiKnobFillStyle_NoFill`, `ImGuiKnobFillStyle_LeftFill`, `ImGuiKnobFillStyle_CenterFill`, `ImGuiKnobFillStyle_RightFill`.

### Flags
 - `ImGuiKnobFlags_NoTitle`: Hide the top title.
 - `ImGuiKnobFlags_NoInput`: Hide the bottom drag input.
 - `ImGuiKnobFlags_ValueTooltip`: Show a tooltip with the current value on hover.
 - `ImGuiKnobFlags_DragHorizontal`: Use horizontal dragging (default is vertical).

### Size
You can specify a size with the size argument, don't use the global font scaling thing anymore

### Steps
Steps determines the number of steps draw, it is only used for the `ImGuiKnobBorderStyle_Ticks` variant to specify the number of ticks to draw.

