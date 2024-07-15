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

    std::cout << "vertices_size: " << mesh.vertices_size() << std::endl;
    std::cout << "faces_size: " << mesh.faces_size() << std::endl;
    std::cout << "edges_size: " << mesh.edges_size() << std::endl;
}
