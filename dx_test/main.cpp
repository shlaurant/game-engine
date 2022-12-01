#include <Windows.h>
#include <WinUser.h>
#include "debug.h"
#include "directx_12.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int
WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine,
                int nCmdShow) {
// Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

// Create the window.

    HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            nullptr,    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,       // Parent window
            NULL,       // Menu
            hInstance,  // Instance handle
            NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    fuse::directx::directx_12 dx12;
    static const float d = 0.1f;
    fuse::directx::geometry t;
    DirectX::SimpleMath::Vector4 white = { 1.f, 1.f, 1.f, 1.f };
    t.vertices = { {{-d, d,  -d}, white},
                   {{d,  d,  -d}, white},
                   {{-d, -d, -d}, white},
                   {{d,  -d, -d}, white},
                   {{-d, d,  d},  white},
                   {{d,  d,  d},  white},
                   {{-d, -d, d},  white},
                   {{d,  -d, d},  white} };
    t.indices = { 0, 1, 2, 1, 3, 2,
                  4, 1, 0, 4, 5, 1,
                  4, 6, 5, 5, 6, 7,
                  6, 2, 3, 6, 3, 7,
                  1, 7, 3, 5, 7, 1,
                  2, 6, 0, 0, 6, 4 };
    std::vector<fuse::directx::geometry> test;
    test.push_back(t);

    MSG msg = { 0 };
    try {
        dx12.init({ hwnd, 1920, 1080, true });
        dx12.init_geometries(test);
        while (msg.message != WM_QUIT) {
            if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else {
                dx12.render_begin();
                for (const auto& e : test) {
                    dx12.render(e);
                }
                dx12.render_end();
            }
        }
    } catch (DxException& e) {
        MessageBoxExW(nullptr, e.ToString().c_str(), nullptr, MB_OK, 0);
    }

    return 0;
}

LRESULT
CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

// All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));

            EndPaint(hwnd, &ps);
        }
            return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}