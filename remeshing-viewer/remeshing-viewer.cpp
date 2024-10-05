#include <pmp/visualization/mesh_viewer.h>

class RemeshingViewer : public pmp::MeshViewer
{
public:
    RemeshingViewer(const char* title, int width, int height);

protected:
    void process_imgui() override;
};

RemeshingViewer::RemeshingViewer(const char* title, int width, int height) : pmp::MeshViewer(title, width, height)
{}

void RemeshingViewer::process_imgui()
{}

int main(int argc, char** argv)
{
    RemeshingViewer window("Remeshing Viewer", 1200, 900);
    return window.run();
}
