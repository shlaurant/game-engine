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
    auto tetra = create_tetra();

    std::vector<fuse::directx::geometry> test;

    DirectX::SimpleMath::Vector3 tmp = {1.f, 0.f, 3.f};
    auto t1 = DirectX::SimpleMath::Matrix::CreateTranslation(tmp);
    tetra.world_matrix = t1;
    test.push_back(tetra);

    auto cube = create_cube();
    DirectX::SimpleMath::Vector3 tmp2 = {-1.f, 0.f, 3.f};
    auto t2 = DirectX::SimpleMath::Matrix::CreateTranslation(tmp2);
    cube.world_matrix = t2;

    test.push_back(cube);

    Input input;
    input.Init(hwnd);

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
                dx12.update_geometries(test);
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