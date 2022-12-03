#include <Windows.h>
#include <WinUser.h>

#include "directx_12.h"
#include "debug.h"
#include "helper.h"
#include "Input.h"

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
    static const float d = 0.5f;
    fuse::directx::geometry t;
    DirectX::SimpleMath::Vector4 white = {1.f, 1.f, 1.f, 1.f};
    DirectX::SimpleMath::Vector4 red = {1.f, 0.f, 0.f, 1.f};
    DirectX::SimpleMath::Vector4 green = {0.f, 1.f, 0.f, 1.f};
    DirectX::SimpleMath::Vector4 blue = {0.f, 0.f, 1.f, 1.f};
    t.vertices = {{{-d, d,  -d}, white},
                  {{d,  d,  -d}, red},
                  {{-d, -d, -d}, green},
                  {{d,  -d, -d}, blue},
                  {{-d, d,  d},  white},
                  {{d,  d,  d},  red},
                  {{-d, -d, d},  green},
                  {{d,  -d, d},  blue}};
    t.indices = {0, 1, 2, 1, 3, 2,
                 1, 7, 3, 1, 5, 7,
                 0, 4, 1, 4, 5, 1,
                 0, 2, 6, 0, 6, 4,
                 2, 3, 6, 3, 7, 6,
                 4, 6, 5, 5, 6, 7};
    std::vector<fuse::directx::geometry> test;
    test.push_back(t);

    Input input;
    input.Init(hwnd);

    DirectX::SimpleMath::Vector3 tmp = {1.f, 0.f, 0.f};
    auto t1 = DirectX::SimpleMath::Matrix::CreateTranslation(tmp);
    transform box_transform = {{1.0f, 0.f, 3.f}};
    std::vector<DirectX::SimpleMath::Matrix> ws;
    ws.push_back(DirectX::SimpleMath::Matrix::CreateTranslation(box_transform.position));

    MSG msg = {};
    try {
        dx12.init({hwnd, 1920, 1080, true});
        camera camera;
        camera.transform.position.z = -2.f;
        dx12.init_geometries(test);
        while (msg.message != WM_QUIT) {
            if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                input.Update();
                handle_input(input, camera);
                dx12.set_vp(camera.view() * camera.projection());
                dx12.set_ojb_w(ws);
                dx12.render_begin();
                for (const auto &e: test) {
                    dx12.render(e);
                }
                dx12.render_end();
            }
        }
    } catch (DxException &e) {
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