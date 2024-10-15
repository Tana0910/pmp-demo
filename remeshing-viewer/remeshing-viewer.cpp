#include <pmp/visualization/mesh_viewer.h>
#include <pmp/algorithms/features.h>
#include <pmp/algorithms/remeshing.h>
#include <pmp/algorithms/utilities.h>
#include <pmp/algorithms/normals.h>
#include <pmp/surface_mesh.h>
#include <pmp/io/io.h>

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include <imgui.h>

class RemeshingViewer : public pmp::MeshViewer
{
public:
    RemeshingViewer(const char* title, int width, int height);

protected:
    void process_imgui() override;
};

RemeshingViewer::RemeshingViewer(const char* title, int width, int height) : pmp::MeshViewer(title, width, height)
{
    set_draw_mode("Hidden Line");
    crease_angle_ = 0.0;
}


void RemeshingViewer::process_imgui()
{
    auto WideStr2MultiByte = [](const std::wstring wstr) -> std::string
        {
            size_t size = ::WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
            std::vector<char> buf;
            buf.resize(size);
            ::WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str(), -1, &buf.front(), static_cast<int>(size), nullptr, nullptr);
            std::string ret(&buf.front(), buf.size() - 1);
            return ret;
        };

    // ImGUIのUIパーツの描画とハンドリング

    MeshViewer::process_imgui();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Features", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();

        static int feature_angle = 70;
        ImGui::PushItemWidth(80);
        ImGui::SliderInt("##feature_angle", &feature_angle, 1, 180);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Detect Features"))
        {
            pmp::clear_features(mesh_);
            pmp::detect_features(mesh_, feature_angle);
            update_mesh();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Uniform Remeshing",
        ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();

        ImGui::PushItemWidth(80);

        static double edge_length{ 0.01 };
        ImGui::InputDouble("Edge Length", &edge_length, 0, 0, "%g");
        ImGui::SameLine();
        if (ImGui::Button("Mean"))
        {
            edge_length = mean_edge_length(mesh_);
        }

        static int n_iterations{ 10 };
        ImGui::SliderInt("Iterations##uniform", &n_iterations, 1, 20);

        static bool use_projection{ true };
        ImGui::Checkbox("Use Projection##uniform", &use_projection);

        static bool scale_lengths{ true };
        ImGui::Checkbox("Scale Lengths##uniform", &scale_lengths);

        ImGui::Spacing();

        if (ImGui::Button("Remesh##uniform"))
        {
            try
            {
                auto scaling = scale_lengths ? bounds(mesh_).size() : 1.0;
                uniform_remeshing(mesh_, edge_length * scaling, n_iterations,
                    use_projection);
            }
            catch (const pmp::InvalidInputException& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            update_mesh();
        }

        ImGui::PopItemWidth();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Adaptive Remeshing",
        ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();

        ImGui::PushItemWidth(80);

        static double min_length{ 0.001 };
        ImGui::InputDouble("Min. Edge Length", &min_length, 0, 0, "%g");

        static double max_length{ 0.05 };
        ImGui::InputDouble("Max. Edge Length", &max_length, 0, 0, "%g");

        static double max_error{ 0.0005 };
        ImGui::InputDouble("Max. Error", &max_error, 0, 0, "%g");

        static int n_iterations{ 10 };
        ImGui::SliderInt("Iterations##adaptive", &n_iterations, 1, 20);

        static bool use_projection{ true };
        ImGui::Checkbox("Use Projection##adaptive", &use_projection);

        static bool scale_lengths{ true };
        ImGui::Checkbox("Scale Lengths##adaptive", &scale_lengths);

        ImGui::Spacing();

        if (ImGui::Button("Remesh##adaptive"))
        {
            auto scaling = scale_lengths ? bounds(mesh_).size() : 1.0;
            try
            {
                adaptive_remeshing(mesh_, min_length * scaling,
                    max_length * scaling, max_error * scaling,
                    n_iterations, use_projection);
            }
            catch (const pmp::InvalidInputException& e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
            update_mesh();
        }

        ImGui::PopItemWidth();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Load",
        ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Load STL"))
        {
            std::wstring filepath(L"", MAX_PATH);
            HWND hWnd = reinterpret_cast<HWND>(glfwGetWindowUserPointer(GetGLFWPointer()));
            OPENFILENAME ofn = {
                sizeof(OPENFILENAME),
                hWnd,
                nullptr,
                L"STLファイル(*.stl)\0*.stl\0すべてのファイル(*.*)\0*.*\0\0",	// filter
                nullptr,
                0,
                0,
                &filepath.front(),
                MAX_PATH,
                nullptr,
                0,
                nullptr,
                nullptr,
                OFN_FILEMUSTEXIST,
                0,
                0,
                nullptr,
                0,
                nullptr,
                nullptr
            };
            if (::GetOpenFileName(&ofn) != 0)
            {
                filepath.resize(wcslen(filepath.data()));
                std::string meshfilepath = WideStr2MultiByte(filepath);
                load_mesh(meshfilepath.c_str());
                pmp::face_normals(mesh_);
            }
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Save",
        ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Save STL"))
        {
            std::wstring outputpath(L"", MAX_PATH);
            HWND hWnd = reinterpret_cast<HWND>(glfwGetWindowUserPointer(GetGLFWPointer()));
            OPENFILENAME ofn = {
                sizeof(OPENFILENAME),
                hWnd,
                nullptr,
                L"STLファイル(*.stl)\0*.stl\0すべてのファイル(*.*)\0*.*\0\0",	// filter
                nullptr,
                0,
                0,
                &outputpath.front(),
                MAX_PATH,
                nullptr,
                0,
                nullptr,
                nullptr,
                OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT,
                0,
                0,
                L"stl",
                0,
                nullptr,
                nullptr
            };
            if (::GetSaveFileName(&ofn) != 0)
            {
                outputpath.resize(wcslen(outputpath.data()));
                std::string meshfile = WideStr2MultiByte(outputpath);
                std::filesystem::path meshfilepath(meshfile);
                try
                {
                    pmp::write(this->mesh_, meshfilepath);
                }
                catch (const pmp::InvalidInputException& e)
                {
                    std::cerr << e.what() << std::endl;
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    RemeshingViewer window("Remeshing Viewer", 1200, 900);
    return window.run();
}
