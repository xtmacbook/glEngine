

#include "imgui.h"
#include "imgui_impl.h"

#define GLFW_HAS_WINDOW_TOPMOST       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ GLFW_FLOATING
#define GLFW_HAS_WINDOW_HOVERED       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ GLFW_HOVERED
#define GLFW_HAS_WINDOW_ALPHA         (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwSetWindowOpacity
#define GLFW_HAS_PER_MONITOR_DPI      (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwGetMonitorContentScale
#define GLFW_HAS_VULKAN               (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ glfwCreateWindowSurface
#ifdef GLFW_RESIZE_NESW_CURSOR  // let's be nice to people who pulled GLFW between 2019-04-16 (3.4 define) and 2019-11-29 (cursors defines) // FIXME: Remove when GLFW 3.4 is released?
#define GLFW_HAS_NEW_CURSORS          (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3400) // 3.4+ GLFW_RESIZE_ALL_CURSOR, GLFW_RESIZE_NESW_CURSOR, GLFW_RESIZE_NWSE_CURSOR, GLFW_NOT_ALLOWED_CURSOR
#else
#define GLFW_HAS_NEW_CURSORS          (0)
#endif

// Data
enum GlfwClientApi
{
    GlfwClientApi_Unknown,
    GlfwClientApi_OpenGL,
    GlfwClientApi_Vulkan
};
static GLUwindow*          g_Window = NULL;    // Main window
static GlfwClientApi        g_ClientApi = GlfwClientApi_Unknown;
static double               g_Time = 0.0;
static bool                 g_MouseJustPressed[ImGuiMouseButton_COUNT] = {};
static GLUcursor*           g_MouseCursors[ImGuiMouseCursor_COUNT] = {};
static bool                 g_InstalledCallbacks = false;

// Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
static GLUmousebuttonfun   g_PrevUserCallbackMousebutton = NULL;
static GLUscrollfun        g_PrevUserCallbackScroll = NULL;
static GLUkeyfun           g_PrevUserCallbackKey = NULL;
static GLUcharfun          g_PrevUserCallbackChar = NULL;

static const char* ImGui_ImplGlfw_GetClipboardText(void* user_data)
{
    return gluGetClipboardString((GLUwindow*)user_data);
}

static void ImGui_ImplGlfw_SetClipboardText(void* user_data, const char* text)
{
    gluSetClipboardString((GLUwindow*)user_data, text);
}

void ImGui_ImplGlfw_MouseButtonCallback(GLUwindow* window, int button, int action, int mods)
{

	if(!ImGui::BeOverImgui())
		if (g_PrevUserCallbackMousebutton != NULL)
			g_PrevUserCallbackMousebutton(window, button, action, mods);

    if (action == GLU_PRESS && button >= 0 && button < IM_ARRAYSIZE(g_MouseJustPressed))
        g_MouseJustPressed[button] = true;
}

void ImGui_ImplGlfw_ScrollCallback(GLUwindow* window, double xoffset, double yoffset)
{
    if (g_PrevUserCallbackScroll != NULL)
        g_PrevUserCallbackScroll(window, xoffset, yoffset);

    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)xoffset;
    io.MouseWheel += (float)yoffset;
}

void ImGui_ImplGlfw_KeyCallback(GLUwindow* window, int key, int scancode, int action, int mods)
{
    if (g_PrevUserCallbackKey != NULL)
        g_PrevUserCallbackKey(window, key, scancode, action, mods);

    ImGuiIO& io = ImGui::GetIO();
    if (action == GLU_PRESS)
        io.KeysDown[key] = true;
    if (action == GLU_RELEASE)
        io.KeysDown[key] = false;

    // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[GLU_KEY_LEFT_CONTROL] || io.KeysDown[GLU_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLU_KEY_LEFT_SHIFT] || io.KeysDown[GLU_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLU_KEY_LEFT_ALT] || io.KeysDown[GLU_KEY_RIGHT_ALT];
#ifdef _WIN32
    io.KeySuper = false;
#else
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
#endif
}

void ImGui_ImplGlfw_CharCallback(GLUwindow* window, unsigned int c)
{
    if (g_PrevUserCallbackChar != NULL)
        g_PrevUserCallbackChar(window, c);

    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(c);
}

static bool ImGui_ImplGlfw_Init(GLUwindow* window, bool install_callbacks, GlfwClientApi client_api)
{
    g_Window = window;
    g_Time = 0.0;

    // Setup backend capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "imgui_impl_glfw";

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab] = GLU_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLU_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLU_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLU_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLU_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLU_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLU_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLU_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLU_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLU_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLU_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLU_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLU_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLU_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLU_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = GLU_KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = GLU_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLU_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLU_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLU_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLU_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLU_KEY_Z;

    io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
    io.ClipboardUserData = g_Window;
#if defined(_WIN32)
    io.ImeWindowHandle = (void*)gluGetWin32Window(g_Window);
#endif

    // Create mouse cursors
    // (By design, on X11 cursors are user configurable and some cursors may be missing. When a cursor doesn't exist,
    // GLFW will emit an error which will often be printed by the app, so we temporarily disable error reporting.
    // Missing cursors will return NULL and our _UpdateMouseCursor() function will use the Arrow cursor instead.)
    g_MouseCursors[ImGuiMouseCursor_Arrow] = gluCreateStandardCursor(GLU_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_TextInput] = gluCreateStandardCursor(GLU_IBEAM_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNS] = gluCreateStandardCursor(GLU_VRESIZE_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeEW] = gluCreateStandardCursor(GLU_HRESIZE_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_Hand] = gluCreateStandardCursor(GLU_HAND_CURSOR);
#if GLFW_HAS_NEW_CURSORS
    g_MouseCursors[ImGuiMouseCursor_ResizeAll] = gluCreateStandardCursor(GLU_RESIZE_ALL_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = gluCreateStandardCursor(GLU_RESIZE_NESW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = gluCreateStandardCursor(GLU_RESIZE_NWSE_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_NotAllowed] = gluCreateStandardCursor(GLU_NOT_ALLOWED_CURSOR);
#else
    g_MouseCursors[ImGuiMouseCursor_ResizeAll] = gluCreateStandardCursor(GLU_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = gluCreateStandardCursor(GLU_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = gluCreateStandardCursor(GLU_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_NotAllowed] = gluCreateStandardCursor(GLU_ARROW_CURSOR);
#endif
    // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
    g_PrevUserCallbackMousebutton = NULL;
    g_PrevUserCallbackScroll = NULL;
    g_PrevUserCallbackKey = NULL;
    g_PrevUserCallbackChar = NULL;
    if (install_callbacks)
    {
        g_InstalledCallbacks = true;
        g_PrevUserCallbackMousebutton = gluSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
        g_PrevUserCallbackScroll = gluSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
        g_PrevUserCallbackKey = gluSetKeyCallback(window, ImGui_ImplGlfw_KeyCallback);
        g_PrevUserCallbackChar = gluSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
    }

    g_ClientApi = client_api;
    return true;
}

bool ImGui_ImplGlfw_InitForOpenGL(GLUwindow* window, bool install_callbacks)
{
    return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_OpenGL);
}

bool ImGui_ImplGlfw_InitForVulkan(GLUwindow* window, bool install_callbacks)
{
    return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_Vulkan);
}

void ImGui_ImplGlfw_Shutdown()
{
    if (g_InstalledCallbacks)
    {
        gluSetMouseButtonCallback(g_Window, g_PrevUserCallbackMousebutton);
        gluSetScrollCallback(g_Window, g_PrevUserCallbackScroll);
        gluSetKeyCallback(g_Window, g_PrevUserCallbackKey);
        gluSetCharCallback(g_Window, g_PrevUserCallbackChar);
        g_InstalledCallbacks = false;
    }

    for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
    {
        gluDestroyCursor(g_MouseCursors[cursor_n]);
        g_MouseCursors[cursor_n] = NULL;
    }
    g_ClientApi = GlfwClientApi_Unknown;
}

static void ImGui_ImplGlfw_UpdateMousePosAndButtons()
{
    // Update buttons
    ImGuiIO& io = ImGui::GetIO();
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
        io.MouseDown[i] = g_MouseJustPressed[i] || gluGetMouseButton(g_Window, i) != 0;
        g_MouseJustPressed[i] = false;
    }

    // Update mouse position
    const ImVec2 mouse_pos_backup = io.MousePos;
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
#ifdef __EMSCRIPTEN__
    const bool focused = true; // Emscripten
#else
    const bool focused = gluGetWindowAttrib(g_Window, GLU_FOCUSED) != 0;
#endif
    if (focused)
    {
        if (io.WantSetMousePos)
        {
			gluSetCursorPos(g_Window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
        }
        else
        {
            double mouse_x, mouse_y;
            gluGetCursorPos(g_Window, &mouse_x, &mouse_y);
            io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
        }
    }
}

static void ImGui_ImplGlfw_UpdateMouseCursor()
{
     
}

static void ImGui_ImplGlfw_UpdateGamepads()
{
    ImGuiIO& io = ImGui::GetIO();
    memset(io.NavInputs, 0, sizeof(io.NavInputs));
    if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
        return;
    
}

void ImGui_ImplGlfw_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer backend. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    gluGetWindowSize(g_Window, &w, &h);
    gluGetFramebufferSize(g_Window, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    if (w > 0 && h > 0)
        io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);

    // Setup time step
    double current_time = gluGetTime();
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;

    ImGui_ImplGlfw_UpdateMousePosAndButtons();
    ImGui_ImplGlfw_UpdateMouseCursor();

    // Update game controllers (if enabled and available)
    ImGui_ImplGlfw_UpdateGamepads();
}
