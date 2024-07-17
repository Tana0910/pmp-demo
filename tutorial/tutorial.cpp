#define _USE_MATH_DEFINES
#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

int main()
{
    std::wstring path(L"", MAX_PATH);
    ::GetModuleFileName(nullptr, &path.front(), MAX_PATH);
    ::PathRemoveFileSpec(&path.front());
    path.resize(wcslen(path.c_str()));
    path.append(L"\\..\\..\\..\\pmp-library\\data\\off\\bunny.off");
    pmp::SurfaceMesh mesh;
    pmp::read(mesh, path);

    std::cout << "===== bunny.off geometry data =====" << std::endl;
    std::cout << "vertices_size: " << mesh.vertices_size() << std::endl;
    std::cout << "faces_size: " << mesh.faces_size() << std::endl;
    std::cout << "edges_size: " << mesh.edges_size() << std::endl;
    std::cout << "===================================" << std::endl;

    mesh.clear();
    auto v0 = mesh.add_vertex(pmp::Point(0.0, 0.0, 0.0));
    auto v1 = mesh.add_vertex(pmp::Point(1.0, 0.0, 0.0));
    auto v2 = mesh.add_vertex(pmp::Point(0.0, 1.0, 0.0));
    auto v3 = mesh.add_vertex(pmp::Point(0.0, 0.0, 1.0));

    auto f0 = mesh.add_triangle(v0, v1, v3);
    auto f1 = mesh.add_triangle(v1, v2, v3);
    auto f2 = mesh.add_triangle(v2, v0, v3);
    auto f3 = mesh.add_triangle(v0, v2, v1);

    std::cout << "==== new created data =====" << std::endl;
    std::cout << "vertices_size: " << mesh.vertices_size() << std::endl;
    std::cout << "faces_size: " << mesh.faces_size() << std::endl;
    std::cout << "edges_size: " << mesh.edges_size() << std::endl;

    for (const auto& pos : mesh.positions())
    {
        auto* ppos = pos.data();
        std::cout << "(x, y, z) = (" << *ppos << ", " << *(ppos + 1) << ", " << *(ppos + 2) << ")" << std::endl;
    }

    auto points = mesh.get_vertex_property<pmp::Point>("v:point");
    if (points.data() != nullptr)
    {
        for (auto& point : points.vector())
        {
            std::cout << "(x, y, z) = (" << point << ")" << std::endl;
        }

        pmp::Point bc(0, 0, 0);
        for (const auto& v : mesh.vertices())
        {
            bc += points[v];
        }

        bc /= mesh.n_vertices();
        std::cout << "n_vertices = " << mesh.n_vertices() << std::endl;
        std::cout << "barycenter = (" << bc << ")" << std::endl;
    }

    std::cout << "===========================" << std::endl;
    return 0;
}
