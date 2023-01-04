#include <WinUser.h>
#include <windowsx.h>

#include "directx_12.h"
#include "debug.h"
#include "helper.h"
#include "Input.h"

using namespace DirectX::SimpleMath;

Input input;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

std::vector<fuse::directx::geometry<fuse::directx::vertex>> create_geometries();
std::vector<fuse::directx::object_constant> create_obj_const();
std::vector<fuse::directx::render_info>
create_render_info(
        const std::vector<fuse::directx::geometry<fuse::directx::vertex>> &,
        int offset);
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
        dx12.load_texture(L"resource\\kyaru.png");
        dx12.load_texture(L"resource\\white.png");
        dx12.load_texture(L"resource\\ground_color.jpg");
        dx12.load_texture(L"resource\\WireFence.dds");
        dx12.load_texture(L"resource\\treeArray2.dds");
        dx12.bind_texture(0, 0, 0);
        dx12.bind_texture(1, 3, 0);
        dx12.bind_texture(2, 2, 0);
        dx12.bind_texture(3, 1, 0);
        dx12.bind_texture(4, 1, 0);
        dx12.bind_texture(5, 4, 0);
        dx12.bind_texture(6, 4, 0);


        dx12.init_geometries<fuse::directx::vertex>(geo);
        std::vector<fuse::directx::geometry<fuse::directx::vertex_billboard>> geo1;
        geo1.resize(2);
        fuse::directx::vertex_billboard v {Vector3(0.0f, 0.0f, 0.f), Vector2(1.f, 1.f)};
        geo1[0].vertices.emplace_back(v);
        geo1[0].indices.emplace_back(0);

        fuse::directx::vertex_billboard v1 {Vector3(0.0f, 0.0f, 0.f), Vector2(1.f, 1.f)};
        geo1[1].vertices.emplace_back(v1);
        geo1[1].indices.emplace_back(0);

        dx12.init_geometries<fuse::directx::vertex_billboard>(geo1);

        auto infos = create_render_info(geo, 0);
        fuse::directx::render_info binfo{true, 5, (int) geo1[0].indices.size(),
                                         (int) geo1[0].index_offset,
                                         (int) geo1[0].vertex_offset, false,
                                         false, false, Vector4::Zero, false};
        fuse::directx::render_info binfo1{true, 6, (int) geo1[1].indices.size(),
                                         (int) geo1[1].index_offset,
                                         (int) geo1[1].vertex_offset, false,
                                         false, false, Vector4::Zero, false};

        infos.emplace_back(binfo);
        infos.emplace_back(binfo1);

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
                dx12.render(infos);
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
    ret.emplace_back(cube0);

    auto cube1 = create_cube_uv();
    ret.emplace_back(cube1);

    auto plane = create_plain(100, 100);
    ret.emplace_back(plane);

    auto mirror = create_plain(5, 5);
    ret.emplace_back(mirror);

    auto skull = load_mesh("resource/skull.txt");
    ret.emplace_back(skull);

    return std::move(ret);
}

std::vector<fuse::directx::object_constant> create_obj_const() {
    std::vector<fuse::directx::object_constant> consts(7);

    DirectX::SimpleMath::Vector3 tmp = {1.f, 1.5f, 3.f};
    auto t0 = DirectX::SimpleMath::Matrix::CreateTranslation(tmp);
    consts[0].world_matrix = t0;
    consts[0].material.diffuse_albedo = Vector4(0.5f, 0.5f, 0.5f, 5.f);
    consts[0].material.fresnel_r0 = Vector3(0.05f, 0.05f, 0.05f);
    consts[0].material.roughness = 0.5f;

    DirectX::SimpleMath::Vector3 tmp2 = {-1.f, 0.5f, 3.f};
    auto t1 = DirectX::SimpleMath::Matrix::CreateTranslation(tmp2);
    consts[1].world_matrix = t1;
    consts[1].material.diffuse_albedo = Vector4(0.1f, 0.1f, 0.1f, 1.f);;
    consts[1].material.fresnel_r0 = Vector3(0.95f, 0.93f, 0.88f);
    consts[1].material.roughness = 0.1f;

    transform t2;
    t2.position.x = -50;
    t2.position.z = -50;
    t2.rotation.x = DirectX::XM_PI / 2;
    consts[2].world_matrix = t2.rotation_matrix() * t2.translation_matrix();
    consts[2].material.diffuse_albedo = Vector4(0.8f, 0.8f, 0.8f, 1.f);;
    consts[2].material.fresnel_r0 = Vector3(0.01f, 0.01f, 0.01f);
    consts[2].material.roughness = 0.9f;

    DirectX::SimpleMath::Vector3 mirror = {-1.5f, 0.f, 7.f};
    consts[3].world_matrix = DirectX::SimpleMath::Matrix::CreateTranslation(
            mirror);
    consts[3].material.diffuse_albedo = Vector4(0.5f, 0.5f, 0.5f, 0.3f);;
    consts[3].material.fresnel_r0 = Vector3(0.95f, 0.95f, 0.95f);
    consts[3].material.roughness = 0.1f;


    DirectX::SimpleMath::Vector3 skull = {-5.f, 0.f, 3.f};
    consts[4].world_matrix = Matrix::CreateTranslation(skull);
    consts[4].material.diffuse_albedo = Vector4(1.f, 1.f, 1.0f, 1.0f);;
    consts[4].material.fresnel_r0 = Vector3(0.05f, 0.05f, 0.05f);
    consts[4].material.roughness = 0.3f;

    consts[5].position = Vector3(0.f, 0.f, 10.f);
    consts[5].world_matrix = Matrix::CreateTranslation(consts[5].position);
    consts[5].material.diffuse_albedo = Vector4(1.f, 1.f, 1.0f, 1.0f);;
    consts[5].material.fresnel_r0 = Vector3(0.05f, 0.05f, 0.05f);
    consts[5].material.roughness = 1.f;

    consts[6].position = Vector3(3.f, 0.f, 10.f);
    consts[6].world_matrix = Matrix::CreateTranslation(consts[6].position);
    consts[6].material.diffuse_albedo = Vector4(1.f, 1.f, 1.0f, 1.0f);;
    consts[6].material.fresnel_r0 = Vector3(0.05f, 0.05f, 0.05f);
    consts[6].material.roughness = 1.f;

    return std::move(consts);
}

std::vector<fuse::directx::render_info>
create_render_info(
        const std::vector<fuse::directx::geometry<fuse::directx::vertex>> &geo,
        int offset) {
    std::vector<fuse::directx::render_info> infos(geo.size());

    for (auto i = 0; i < geo.size(); ++i) {
        infos[i].object_index = offset + i;
        infos[i].index_count = geo[i].indices.size();
        infos[i].index_offset = geo[i].index_offset;
        infos[i].vertex_offset = geo[i].vertex_offset;
        infos[i].do_reflect = true;
        infos[i].is_transparent = false;
        infos[i].is_mirror = false;
        infos[i].do_shadow = false;
    }

    infos[0].do_shadow = true;
    infos[1].is_transparent = true;
    infos[1].do_shadow = true;
    infos[3].is_mirror = true;
    infos[3].is_transparent = true;
    infos[3].mirror_plane = Plane(Vector3(-1.5f, 0.f, 5.f), Vector3::Backward);
    infos[3].do_reflect = false;
    infos[4].do_reflect = false;
    infos[4].do_shadow = true;

    infos.erase(infos.begin() + 3);
    infos.erase(infos.begin() + 2);

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
        li.lights[1].color = DirectX::SimpleMath::Vector3(.2f, .2f, .2f);
        li.lights[1].fo_start;
        li.lights[1].direction;
        li.lights[1].fo_end;
        li.lights[1].position;
        li.lights[1].spot_pow;
    }

    return li;
}
