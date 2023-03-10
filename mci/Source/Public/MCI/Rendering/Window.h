#pragma once

#include "MCI/Common.h"
#include "MCI/ActionPool.h"
#include "MCI/Input/Keyboard.h"
#include "MCI/Input/Mouse.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <unordered_map>

namespace MCI
{

    class Monitor
    {
    public:

        struct VideoMode
        {
            glm::vec2 Size;
            uint16 RefreshRate;
        };

        Monitor(uint8 index = 0);
        ~Monitor();

        static uint8 GetCount();

        std::string GetName() const;
        glm::vec2 GetPhysicalSize() const;
        VideoMode GetVideoMode() const;
        std::vector<VideoMode> GetAvailableVideoModes() const;
        glm::vec4 GetWorkArea() const;

        bool IsValid() const;

        void* GetHandle() const;

    private:

        void* m_handle = nullptr;

    };

    class Window
    {
    public:

        enum class CursorMode
        {
            Normal = 0x00034001,
            Hidden = 0x00034002,
            Disabled = 0x00034003
        };

        enum class ContextRobustness
        {
            NoRobustness = 0,
            NoResetNotification = 0x00031001,
            LoseContextOnReset = 0x00031002
        };

        enum class ContextReleaseBehavior
        {
            Any = 0,
            Flush = 0x00035001,
            None = 0x00035002
        };

        struct Context
        {
            bool Resizable = false;
            bool Visible = true;
            bool Decorated = true;
            bool Focused = true;
            bool AutoIconify = true;
            bool Floating = false;
            bool Maximized = false;

            bool CenterCursor = true;
            bool FocusOnShow = true;
            bool ScaleToMonitor = false;

            int32 RedBits = 8;
            int32 GreenBits = 8;
            int32 BlueBits = 8;
            int32 AlphaBits = 8;
            int32 DepthBits = 24;
            int32 StencilBits = 8;
            int32 AccumRedBits = 0;
            int32 AccumGreenBits = 0;
            int32 AccumBlueBits = 0;
            int32 AccumAlphaBits = 0;

            int32 AuxBuffers = 0;
            int32 RefreshRate = -1;
            bool Stereo = false;
            bool SrgbCapable = false;
            bool DoubleBuffer = true;
        };

        Window(const Context& context);
        ~Window();

        void Open(uint16 width, uint16 height, const std::string_view& title, bool fullscreen = false, const Monitor& monitor = Monitor(0));

        bool IsValid() const noexcept;

        bool IsOpen() const noexcept;
        void Close() noexcept;

        std::string_view GetTitle() const noexcept;
        void SetTitle(std::string_view title) noexcept;

        void Maximize() noexcept;
        void Show() noexcept;
        void Hide() noexcept;
        void Focus() noexcept;
        void RequestAttention() noexcept;
        void Center() noexcept;
        void AllowResize() noexcept;
        void DeclineResize() noexcept;
        void Resize(glm::vec2 newSize) noexcept;

        bool IsVisible() const noexcept;
        bool IsResizable() const noexcept;
        bool IsMaximized() const noexcept;

        glm::vec2 GetSize() const noexcept;
        glm::vec2 GetFramebufferSize() const noexcept;

        void PollEvents() noexcept;
        void SwapBuffers() const noexcept;

        void EnableVSync() noexcept;
        void DisableVSync() noexcept;

        void LockCursor() noexcept;
        void UnlockCursor() noexcept;
        void ShowCursor() noexcept;
        void HideCursor() noexcept;
        void SetMousePosition(glm::vec2 position) const noexcept;

        float GetAspectRatio() const noexcept;

        bool IsVSyncEnabled() const noexcept { return m_isVSyncEnabled; }

        bool IsKeyDown(KeyCode key) const noexcept { return m_justPressedKeys[(uint64)key]; }
        bool IsKeyPressed(KeyCode key) const noexcept { return m_keys[(uint64)key]; }
        bool IsKeyUp(KeyCode key) const noexcept { return m_justReleasedKeys[(uint64)key]; }
        bool IsMouseButtonDown(MouseButton mouseButton) const noexcept { return m_justPressedMouseButtons[(uint64)mouseButton]; }
        bool IsMouseButtonPressed(MouseButton mouseButton) const noexcept { return m_mouseButtons[(uint64)mouseButton]; }
        bool IsMouseButtonUp(MouseButton mouseButton) const noexcept { return m_justReleasedMouseButtons[(uint64)mouseButton]; }

        glm::vec2 GetMousePosition() const noexcept { return m_mousePosition; }
        glm::vec2 GetMouseMoveDelta() const noexcept { return m_mouseMoveDelta; }
        glm::vec2 GetMouseMoveDeltaNormalized() const noexcept { return m_normalizedMouseMoveDelta; }
        glm::vec2 GetMouseScrollDelta() const noexcept { return m_mouseScrollDelta; }
        glm::vec2 GetLastMouseScrollDelta() const noexcept { return m_lastMouseScrollDelta; }

        void* GetHandle() const { return m_handle; }

        static void InitSubsystem() noexcept;
        static void ShutdownSubsystem() noexcept;

        Window& operator=(const Window&) = delete;
        Window(const Window&) = delete;

    private:

        Monitor m_monitor;
        void* m_handle = nullptr;
        bool m_isVSyncEnabled = false;

        bool m_keys[(uint64)KeyCode::MaxValue]{ false };
        bool m_justPressedKeys[(uint64)KeyCode::MaxValue]{ false };
        bool m_justReleasedKeys[(uint64)KeyCode::MaxValue]{ false };
        bool m_mouseButtons[(uint64)MouseButton::MaxValue]{ false };
        bool m_justPressedMouseButtons[(uint64)MouseButton::MaxValue]{ false };
        bool m_justReleasedMouseButtons[(uint64)MouseButton::MaxValue]{ false };

        glm::vec2 m_mousePosition;
        glm::vec2 m_mouseMoveDelta;
        glm::vec2 m_normalizedMouseMoveDelta;
        glm::vec2 m_mouseScrollDelta;
        glm::vec2 m_lastMouseScrollDelta;

        std::string m_title;

        ActionPool m_pendingActions;
        int32 m_threadId;

        void OnKeyDown(KeyCode key);
        void OnKeyUp(KeyCode key);

        void OnMouseButtonDown(MouseButton mouseButton);
        void OnMouseButtonUp(MouseButton mouseButton);

        void OnMouseMove(glm::vec2 mousePosition);
        void OnMouseScroll(glm::vec2 scrollDelta);

        void ResetJustPressedKeys();

        void MakeContextCurrent() const noexcept;

    };

}