#include <Windows.h>
#include <WinUser.h>

#include "directx_12.h"
#include "debug.h"
#include "helper.h"
#include "Input.h"

using namespace DirectX::SimpleMath;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

std::vector<fuse::directx::geometry> create_geometries();
std::vector<fuse::directx::object_constant> create_obj_const();
std::vector<fuse::directx::render_info> create_render_info(const std::vector<fuse::directx::geometry> &);
fuse::directx::light_info create_light_info();

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

    Input input;
    input.Init(hwnd);

    MSG msg = {};
    try {
        fuse::directx::directx_12 dx12;
        auto geo = create_geometries();
        auto consts = create_obj_const();
        auto li = create_light_info();
        camera camera;
        camera.transform.position.z = -2.f;

        dx12.init({hwnd, 1920, 1080, true});
        dx12.load_texture(L"kyaru.png");
        dx12.load_texture(L"white.png");
        dx12.bind_texture(0, 0);
        dx12.bind_texture(1, 1);
        dx12.bind_texture(2, 1);

        dx12.init_geometries(geo);
        auto infos = create_render_info(geo);

        while (msg.message != WM_QUIT) {
            if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                input.Update();
                handle_input(input, camera);
                fuse::directx::camera c;
                c.vp = camera.view() * camera.projection();
                c.position = camera.transform.position;
                dx12.update_camera(c);
                dx12.update_lights(li);
                dx12.update_obj_constants(consts);
                dx12.render_begin();
                for (const auto &e: infos) {
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

std::vector<fuse::directx::geometry> create_geometries() {
    std::vector<fuse::directx::geometry> ret;

    auto cube0 = create_cube_uv();
    ret.emplace_back(cube0);

    auto cube1 = create_cube_uv();
    ret.emplace_back(cube1);

    auto plane = create_plain(100, 100);
    ret.emplace_back(plane);

    return std::move(ret);
}

std::vector<fuse::directx::object_constant> create_obj_const() {
    std::vector<fuse::directx::object_constant> consts(3);

    DirectX::SimpleMath::Vector3 tmp = {1.f, 0.f, 3.f};
    auto t0 = DirectX::SimpleMath::Matrix::CreateTranslation(tmp);
    consts[0].world_matrix = t0;
    consts[0].material.diffuse_albedo = Vector4(0.5f, 0.5f, 0.5f, 1.f);
    consts[0].material.fresnel_r0 = Vector3(0.05f, 0.05f, 0.05f);
    consts[0].material.roughness = 0.5f;

    DirectX::SimpleMath::Vector3 tmp2 = {-1.f, 0.f, 3.f};
    auto t1 = DirectX::SimpleMath::Matrix::CreateTranslation(tmp2);
    consts[1].world_matrix = t1;
    consts[1].material.diffuse_albedo = Vector4(0.1f, 0.1f, 0.1f, 1.f);;
    consts[1].material.fresnel_r0 = Vector3(0.95f, 0.93f, 0.88f);
    consts[1].material.roughness = 0.1f;

    consts[2].world_matrix = Matrix::Identity;
    consts[2].material.diffuse_albedo = Vector4(0.1f, 0.1f, 0.1f, 1.f);;
    consts[2].material.fresnel_r0 = Vector3(0.95f, 0.93f, 0.88f);
    consts[2].material.roughness = 0.1f;

    return std::move(consts);
}

std::vector<fuse::directx::render_info> create_render_info(const std::vector<fuse::directx::geometry> &geo) {
    std::vector<fuse::directx::render_info> infos(geo.size());

    for(auto i = 0; i < geo.size(); ++i){
        infos[i].object_index = i;
        infos[i].index_count = geo[i].indices.size();
        infos[i].index_offset = geo[i].index_offset;
        infos[i].vertex_offset = geo[i].vertex_offset;
    }

    return std::move(infos);
}

fuse::directx::light_info create_light_info() {
    fuse::directx::light_info li;
    {
        li.active_count = 2;

        li.lights[0].type = 0;
        li.lights[0].color = DirectX::SimpleMath::Vector3(1.f, 1.f, 1.f);
        li.lights[0].fo_start;
        li.lights[0].direction = Vector3(0.f, -1.f, 1.f);
        li.lights[0].direction.Normalize();
        li.lights[0].fo_end;
        li.lights[0].position;
        li.lights[0].spot_pow;

        li.lights[1].type = 3;
        li.lights[1].color = DirectX::SimpleMath::Vector3(.1f, .1f, .1f);
        li.lights[1].fo_start;
        li.lights[1].direction;
        li.lights[1].fo_end;
        li.lights[1].position;
        li.lights[1].spot_pow;
    }

    return li;
}
