#include "imgui-knobs.h"

#include <cmath>
#include <cstdlib>
#include <imgui.h>
#include <imgui_internal.h>

#define IMGUIKNOBS_PI 3.14159265358979323846f

namespace ImGuiKnobs {
    namespace detail {
        template<typename DataType>
        struct knob {
            float radius;
            bool value_changed;
            ImVec2 center;
            bool is_active;
            bool is_hovered;
            float angle_min;
            float angle_max;
            float t;
            float angle;
            float angle_cos;
            float angle_sin;

            knob(const char *_label, ImGuiDataType data_type, DataType *p_value, DataType v_min, DataType v_max, float speed, float _radius, const char *format, ImGuiKnobFlags flags) {
                radius = _radius;
                t = ((float) *p_value - v_min) / (v_max - v_min);
                auto screen_pos = ImGui::GetCursorScreenPos();

                // Handle dragging
                ImGui::InvisibleButton(_label, {radius * 2.0f, radius * 2.0f});
                auto gid = ImGui::GetID(_label);
                ImGuiSliderFlags drag_flags = 0;
                if (!(flags & ImGuiKnobFlags_DragHorizontal)) {
                    drag_flags |= ImGuiSliderFlags_Vertical;
                }
                value_changed = ImGui::DragBehavior(gid, data_type, p_value, speed, &v_min, &v_max, format, drag_flags);

                angle_min = IMGUIKNOBS_PI * 0.75f;
                angle_max = IMGUIKNOBS_PI * 2.25f;
                center = {screen_pos[0] + radius, screen_pos[1] + radius};
                is_active = ImGui::IsItemActive();
                is_hovered = ImGui::IsItemHovered();
                angle = angle_min + (angle_max - angle_min) * t;
                angle_cos = cosf(angle);
                angle_sin = sinf(angle);
            }

            void draw_dot(float size, float radius, float angle, color_set color, bool filled, int segments) {
                auto dot_size = size * this->radius;
                auto dot_radius = radius * this->radius;

                ImGui::GetWindowDrawList()->AddCircleFilled(
                        {center[0] + cosf(angle) * dot_radius, center[1] + sinf(angle) * dot_radius},
                        dot_size,
                        is_active ? color.active : (is_hovered ? color.hovered : color.base),
                        segments);
            }

            void draw_tick(float start, float end, float width, float angle, color_set color) {
                auto tick_start = start * radius;
                auto tick_end = end * radius;
                auto angle_cos = cosf(angle);
                auto angle_sin = sinf(angle);

                ImGui::GetWindowDrawList()->AddLine(
                        {center[0] + angle_cos * tick_end, center[1] + angle_sin * tick_end},
                        {center[0] + angle_cos * tick_start, center[1] + angle_sin * tick_start},
                        is_active ? color.active : (is_hovered ? color.hovered : color.base),
                        width * radius);
            }

            void draw_circle(float size, color_set color, bool filled, int segments) {
                auto circle_radius = size * radius;

                ImGui::GetWindowDrawList()->AddCircleFilled(
                        center,
                        circle_radius,
                        is_active ? color.active : (is_hovered ? color.hovered : color.base));
            }

            void calculate_fill(ImGuiKnobFillStyle wiper_style, bool round)
            {
                float start_angle = 1.0f;
                float end_angle = -1.0f;


                switch(wiper_style){
                    case(ImGuiKnobFillStyle_NoFill):
                    {
                        break;
                    }
                    case(ImGuiKnobFillStyle_LeftFill):
                    {
                        start_angle = angle_min;
                        end_angle = angle;
                        break;
                    }
                    case(ImGuiKnobFillStyle_RightFill):
                    {
                        start_angle = angle;
                        end_angle = angle_max;
                        break;
                    }
                    case(ImGuiKnobFillStyle_CenterFill):
                    {
                        const float midpoint = ((angle_max - angle_min) * 0.5f) + angle_min;

                        if(angle > midpoint)
                        {
                            start_angle = midpoint;
                            end_angle = angle;
                        }
                        else
                        {
                            start_angle = angle;
                            end_angle = midpoint;
                        }
                        break;
                    }
                }

                if(round) 
                {
                    for(float ang = start_angle; ang <= end_angle; ang += 0.01f) { draw_dot(0.11, 0.8f, ang, detail::GetPrimaryColorSet(), true, 12); }
                }
                else
                {
                    for(float ang = start_angle; ang <= end_angle; ang += 0.01f) { draw_tick(0.71f, 0.9f, 0.01f, ang, detail::GetPrimaryColorSet()); }
                }
            }
        };

        template<typename DataType>
        knob<DataType> knob_with_drag(const char *label, ImGuiDataType data_type, DataType *p_value, DataType v_min, DataType v_max, float _speed, const char *format, float size, ImGuiKnobFlags flags) {
            auto speed = _speed == 0 ? (v_max - v_min) / 250.f : _speed;
            ImGui::PushID(label);
            auto width = size == 0 ? ImGui::GetTextLineHeight() * 4.0f : size * ImGui::GetIO().FontGlobalScale;
            ImGui::PushItemWidth(width);

            ImGui::BeginGroup();

            // There's an issue with `SameLine` and Groups, see https://github.com/ocornut/imgui/issues/4190.
            // This is probably not the best solution, but seems to work for now
            ImGui::GetCurrentWindow()->DC.CurrLineTextBaseOffset = 0;

            // Draw title
            if (!(flags & ImGuiKnobFlags_NoTitle)) {
                auto title_size = ImGui::CalcTextSize(label, NULL, false, width);

                // Center title
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (width - title_size[0]) * 0.5f);

                ImGui::Text("%s", label);
            }

            // Draw knob
            knob<DataType> k(label, data_type, p_value, v_min, v_max, speed, width * 0.5f, format, flags);

            // Draw tooltip
            if (flags & ImGuiKnobFlags_ValueTooltip && (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) || ImGui::IsItemActive())) {
                ImGui::BeginTooltip();
                ImGui::Text(format, *p_value);
                ImGui::EndTooltip();
            }

            // Draw input
            if (!(flags & ImGuiKnobFlags_NoInput)) {
                ImGuiSliderFlags drag_flags = 0;
                if (!(flags & ImGuiKnobFlags_DragHorizontal)) {
                    drag_flags |= ImGuiSliderFlags_Vertical;
                }
                auto changed = ImGui::DragScalar("###knob_drag", data_type, p_value, speed, &v_min, &v_max, format, drag_flags);
                if (changed) {
                    k.value_changed = true;
                }
            }

            ImGui::EndGroup();
            ImGui::PopItemWidth();
            ImGui::PopID();

            return k;
        }

        color_set GetPrimaryColorSet() {
            auto *colors = ImGui::GetStyle().Colors;

            return {colors[ImGuiCol_ButtonActive], colors[ImGuiCol_ButtonHovered], colors[ImGuiCol_ButtonHovered]};
        }

        color_set GetSecondaryColorSet() {
            auto *colors = ImGui::GetStyle().Colors;
            auto active = ImVec4(
                    colors[ImGuiCol_ButtonActive].x * 0.5f,
                    colors[ImGuiCol_ButtonActive].y * 0.5f,
                    colors[ImGuiCol_ButtonActive].z * 0.5f,
                    colors[ImGuiCol_ButtonActive].w);

            auto hovered = ImVec4(
                    colors[ImGuiCol_ButtonHovered].x * 0.5f,
                    colors[ImGuiCol_ButtonHovered].y * 0.5f,
                    colors[ImGuiCol_ButtonHovered].z * 0.5f,
                    colors[ImGuiCol_ButtonHovered].w);

            return {active, hovered, hovered};
        }

        color_set GetTrackColorSet() {
            auto *colors = ImGui::GetStyle().Colors;

            return {colors[ImGuiCol_FrameBg], colors[ImGuiCol_FrameBg], colors[ImGuiCol_FrameBg]};
        }

        color_set GetTrackColorSet(float AlphaAdjust) {
            auto *colors = ImGui::GetStyle().Colors;
            ImVec4 Color = colors[ImGuiCol_FrameBg];
            Color.w *= AlphaAdjust;
            return {Color, Color, Color};
        }
    }// namespace detail


    template<typename DataType>
    bool BaseKnob(const char *label, ImGuiDataType data_type, DataType *p_value, DataType v_min, DataType v_max, float speed, const char *format, float size,
     ImGuiKnobButtonStyle button_style, ImGuiKnobBorderStyle border_style, ImGuiKnobFillStyle wiper_style, ImGuiKnobFlags flags, int steps = 10) {
        auto knob = detail::knob_with_drag(label, data_type, p_value, v_min, v_max, speed, format, size, flags);

        float button_size = 0.85f; //Default button size
        float dot_position = 0.6f;
        float tick_position = 0.5f;

        //Frist draw the border
        switch (border_style){
            case ImGuiKnobBorderStyle_None:
            {
                break;
            }
            case ImGuiKnobBorderStyle_Ticks:
            {
                for (auto n = 0.f; n < steps; n++) {
                    auto a = n / (steps - 1);
                    auto angle = knob.angle_min + (knob.angle_max - knob.angle_min) * a;
                    knob.draw_tick(0.7f, 0.9f, 0.04f, angle, detail::GetPrimaryColorSet());
                }

                button_size = 0.6f;
                dot_position = 0.4f;
                tick_position = 0.4f;
                break;
            }
            case ImGuiKnobBorderStyle_WiperDot:
            {
                //custom arc because draw arc has weird alpha overlap issues, also it's round and that's neat (add a constant to lower alpha)
                for(float ang = knob.angle_min; ang <= knob.angle_max; ang += 0.01f) { knob.draw_dot(0.11, 0.8f, ang, detail::GetTrackColorSet(0.1f), true, 12); }
                //some jank to fix alpha blend at the ends of the round tracks
                knob.draw_dot(0.11, 0.8f, knob.angle_min, detail::GetTrackColorSet(0.2f), true, 12);
                knob.draw_dot(0.11, 0.8f, knob.angle_max, detail::GetTrackColorSet(0.2f), true, 12);
                //actually draw the tracking dot
                knob.draw_dot(0.1f, 0.8f, knob.angle, detail::GetPrimaryColorSet(), true, 12);

                //fill (round)
                knob.calculate_fill(wiper_style, true);

                button_size = 0.6f;
                dot_position = 0.4f;
                tick_position = 0.35f;
                break;
            }
            case ImGuiKnobBorderStyle_WiperTick:
            {
                //custom arc because draw arc has weird alpha overlap issues
                for(float ang = knob.angle_min; ang <= knob.angle_max; ang += 0.01f) { knob.draw_tick(0.71f, 0.9f, 0.01f, ang, detail::GetTrackColorSet()); }
                knob.draw_tick(0.71f, 0.9f, 0.055f, knob.angle, detail::GetPrimaryColorSet());

                //fill
                knob.calculate_fill(wiper_style, false);

                button_size = 0.6f;
                dot_position = 0.4f;
                tick_position = 0.35f;
                break;
            }
        }

        //Then draw the button
        switch (button_style){
            case ImGuiKnobButtonStyle_None:
            {
                break;
            }
            case ImGuiKnobButtonStyle_Blank:
            {
                knob.draw_circle(button_size, detail::GetSecondaryColorSet(), true, 32);
                break;
            }
            case ImGuiKnobButtonStyle_Tick:
            {
                knob.draw_circle(button_size, detail::GetSecondaryColorSet(), true, 32);
                knob.draw_tick(tick_position, button_size, 0.08f, knob.angle, detail::GetPrimaryColorSet());
                break;
            }
            case ImGuiKnobButtonStyle_Dot:
            {
                knob.draw_circle(button_size, detail::GetSecondaryColorSet(), true, 32);
                knob.draw_dot(0.12f, dot_position, knob.angle, detail::GetPrimaryColorSet(), true, 12);
                break;
            }
        }

        return knob.value_changed;
    }

    bool Knob(const char *label, float *p_value, float v_min, float v_max, float speed, const char *format, float size,
     ImGuiKnobButtonStyle button_style, ImGuiKnobBorderStyle border_style, ImGuiKnobFillStyle fill_style, ImGuiKnobFlags flags, int steps) {
        const char *_format = format == NULL ? "%.3f" : format;
        return BaseKnob(label, ImGuiDataType_Float, p_value, v_min, v_max, speed, _format, size, button_style, border_style, fill_style, flags, steps);
    }

    bool KnobInt(const char *label, float *p_value, float v_min, float v_max, float speed, const char *format, float size,
     ImGuiKnobButtonStyle button_style, ImGuiKnobBorderStyle border_style, ImGuiKnobFillStyle fill_style, ImGuiKnobFlags flags, int steps) {
        const char *_format = format == NULL ? "%i" : format;
        return BaseKnob(label, ImGuiDataType_S32, p_value, v_min, v_max, speed, _format, size, button_style, border_style, fill_style, flags, steps);
    }

}// namespace ImGuiKnobs
