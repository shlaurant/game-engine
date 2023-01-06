#include <WinUser.h>
#include <windowsx.h>

#include "directx_12.h"
#include "debug.h"
#include "helper.h"
#include "Input.h"
#include "GameTimer.h"

using namespace DirectX::SimpleMath;

Input input;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

std::vector<fuse::directx::geometry<fuse::directx::vertex>> create_geometries();
fuse::directx::light_info create_light_info();

std::vector<std::shared_ptr<fuse::directx::renderee>> build_renderees();
void load_geometries(fuse::directx::directx_12 &dx12);
void load_materials(fuse::directx::directx_12 &dx12);
void load_textures(fuse::directx::directx_12 &dx12);
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

    input.Init(hwnd);

    MSG msg = {};
    try {
        fuse::directx::directx_12 dx12;
        GameTimer timer;
        timer.Reset();

        dx12.init({hwnd, 1920, 1080, true});
        auto li = create_light_info();
        load_geometries(dx12);
        load_textures(dx12);
        load_materials(dx12);
        std::vector<std::shared_ptr<fuse::directx::renderee>> renderees = build_renderees();
        dx12.init_renderees(renderees);
        camera camera;
        camera.transform.position.z = -2.f;
        camera.transform.position.y = 5.f;

        timer.Start();
        while (msg.message != WM_QUIT) {
            if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                std::wstring txt = L"fps: ";
                txt += std::to_wstring(1 / timer.DeltaTime());

                SetWindowText(hwnd, txt.c_str());
                timer.Tick();
                input.Update();
                handle_input(input, camera, timer);
                fuse::directx::camera c;
                c.vp = camera.view() * camera.projection();
                c.position = camera.transform.position;
                dx12.update_camera(c);
                dx12.update_lights(li);
                dx12.render_begin();
                dx12.render();
                dx12.render_end();
            }
        }
    } catch (DxException &e) {
        MessageBoxExW(nullptr, e.ToString().c_str(), nullptr, MB_OK, 0);
    }

    return 0;
}
void load_textures(fuse::directx::directx_12 &dx12) {
    dx12.load_texture("kyaru", L"resource\\kyaru.png");
    dx12.load_texture("white", L"resource\\white.png");
    dx12.load_texture("ground", L"resource\\ground_color.jpg");
    dx12.load_texture("wire", L"resource\\WireFence.dds");
    dx12.load_texture("tree_arr", L"resource\\treeArray2.dds");
    dx12.load_texture("terrain_d", L"resource\\terrain_d.png");
    dx12.load_texture("terrain_h", L"resource\\terrain_h.png");
}
void load_materials(fuse::directx::directx_12 &dx12) {
    dx12.load_material({"default", "metal", "rough", "glass", "terrain"},
                   {{Vector4(.5f, .5f, .5f, 1.f),
                                Vector3(0.5f, 0.5f, 0.5f), .5f},
                        {Vector4(.5f, .5f, .5f, 1.f),
                                Vector3(0.9f, 0.9f, 0.9f), .1f},
                        {Vector4(.5f, .5f, .5f, 1.f),
                                Vector3(0.1f, 0.1f, 0.1f), .9f},
                        {Vector4(.5f, .5f, .5f, .5f),
                                Vector3(0.5f, 0.5f, 0.5f), .1f},
                        {Vector4(.5f, .5f, .5f, 1.f),
                                Vector3(0.001f, 0.001, 0.001f), .99f}});
}
void load_geometries(fuse::directx::directx_12 &dx12) {
    auto geo = create_geometries();
    std::vector<fuse::directx::geometry<fuse::directx::vertex_billboard>> geo1;
    geo1.resize(1);
    fuse::directx::vertex_billboard v{Vector3(0.0f, 0.0f, 0.f),
                                      Vector2(1.f, 1.f)};
    geo1[0].name = "billboard";
    geo1[0].vertices = {{Vector3(0.0f, 0.0f, 0.f), Vector2(1.f, 1.f)}, {Vector3(1.0f, 0.0f, 0.f), Vector2(1.5f, 1.5f)},{Vector3(2.0f, 0.0f, 0.f), Vector2(1.f, 1.f)}};
    geo1[0].indices.emplace_back(0);
    geo1[0].indices.emplace_back(1);
    geo1[0].indices.emplace_back(2);

    dx12.init_geometries<fuse::directx::vertex>(geo);
    dx12.init_geometries<fuse::directx::vertex_billboard>(geo1);
}
std::vector<std::shared_ptr<fuse::directx::renderee>> build_renderees() {
    std::vector<std::shared_ptr<fuse::directx::renderee>> renderees;
    {
        auto skull0 = std::make_shared<fuse::directx::renderee>();
        skull0->name = "skull0";
        skull0->type = fuse::directx::renderee_type::opaque;
        skull0->geometry = "skull";
        skull0->texture[0] = "white";
        skull0->material = "default";
        skull0->tr.scale = Vector3(.5f, .5f, .5f);
        skull0->tr.position = Vector3(-5.f, 1.f, 3.f);
        renderees.emplace_back(skull0);

        auto skull1 = std::make_shared<fuse::directx::renderee>();
        skull1->name = "skull1";
        skull1->type = fuse::directx::renderee_type::opaque;
        skull1->geometry = "skull";
        skull1->texture[0] = "white";
        skull1->material = "rough";
        skull1->tr.position = Vector3(-12.f, 1.f, 3.f);
        renderees.emplace_back(skull1);

        auto skull2 = std::make_shared<fuse::directx::renderee>();
        skull2->name = "skull2";
        skull2->type = fuse::directx::renderee_type::opaque;
        skull2->geometry = "skull";
        skull2->texture[0] = "white";
        skull2->material = "metal";
        skull2->tr.position = Vector3(-19.f, 1.f, 3.f);
        renderees.emplace_back(skull2);

        auto skull3 = std::make_shared<fuse::directx::renderee>();
        skull3->name = "skull3";
        skull3->type = fuse::directx::renderee_type::translucent;
        skull3->geometry = "skull";
        skull3->texture[0] = "white";
        skull3->material = "glass";
        skull3->tr.position = Vector3(-26.f, 1.f, 3.f);
        renderees.emplace_back(skull3);

        auto wire = std::make_shared<fuse::directx::renderee>();
        wire->name = "wire";
        wire->type = fuse::directx::renderee_type::translucent;
        wire->geometry = "cube";
        wire->texture[0] = "wire";
        wire->material = "rough";
        wire->tr.position = Vector3(0.f, 6.f, 3.f);
        renderees.emplace_back(wire);

        auto cube0 = std::make_shared<fuse::directx::renderee>();
        cube0->name = "cube0";
        cube0->type = fuse::directx::renderee_type::translucent;
        cube0->geometry = "cube";
        cube0->texture[0] = "white";
        cube0->material = "glass";
        cube0->tr.position = Vector3(3.f, 6.f, 3.f);
        renderees.emplace_back(cube0);

        auto tree_billboard = std::make_shared<fuse::directx::renderee>();
        tree_billboard->name = "tree_billboard";
        tree_billboard->type = fuse::directx::renderee_type::billboard;
        tree_billboard->geometry = "billboard";
        tree_billboard->texture[0] = "tree_arr";
        tree_billboard->material = "rough";
        tree_billboard->tr.position = Vector3(0.f, 6.f, 10.f);
        renderees.emplace_back(tree_billboard);

        auto terrain = std::make_shared<fuse::directx::renderee>();
        terrain->name = "terrain";
        terrain->type = fuse::directx::renderee_type::terrain;
        terrain->geometry = "terrain";
        terrain->texture[0] = "terrain_d";
        terrain->texture[1] = "terrain_h";
        terrain->material = "terrain";
        terrain->tr.position = Vector3(0.f, 0.f, 0.f);
        renderees.emplace_back(terrain);
    }
    return renderees;
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

        case WM_MOUSEMOVE: {
            input.set_mouse_pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        }

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

std::vector<fuse::directx::geometry<fuse::directx::vertex>>
create_geometries() {
    std::vector<fuse::directx::geometry<fuse::directx::vertex>> ret;

    auto cube0 = create_cube_uv();
    cube0.name = "cube";
    ret.emplace_back(cube0);

    auto plane = create_plain(100, 100);
    plane.name = "plane";
    ret.emplace_back(plane);

    auto mirror = create_plain(5, 5);
    mirror.name = "mirror";
    ret.emplace_back(mirror);

    auto skull = load_mesh("resource/skull.txt");
    skull.name = "skull";
    ret.emplace_back(skull);

    auto terrain = create_terrain(20, 2);
    terrain.name = "terrain";
    ret.emplace_back(terrain);

    return std::move(ret);
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
        li.lights[1].color = DirectX::SimpleMath::Vector3(.5f, .5f, .5f);
        li.lights[1].fo_start;
        li.lights[1].direction;
        li.lights[1].fo_end;
        li.lights[1].position;
        li.lights[1].spot_pow;
    }

    return li;
}
