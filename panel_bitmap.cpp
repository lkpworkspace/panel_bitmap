#include "SFEPanel.hpp"

namespace sfe {

class PanelBitmap : public SFEPanel {
public:
    virtual bool Init() override {
        _width = 0;
        _height = 0;
        _pixels.clear();
        return true;
    }

    virtual void Update() override {
        ImGui::Begin(PanelName().c_str());
        auto win_pos = ImGui::GetWindowPos();
        auto cursor_pos = ImGui::GetCursorPos();
        auto region_avail_pos = win_pos;
        region_avail_pos.x += cursor_pos.x;
        region_avail_pos.y += cursor_pos.y;
        auto content_region_min = ImGui::GetWindowContentRegionMin();
        auto content_region_max = ImGui::GetWindowContentRegionMax();
        auto region_avail_sz = content_region_max;
        region_avail_sz.x -= content_region_min.x;
        region_avail_sz.y -= content_region_min.y;

        ImVec2 center_pos(region_avail_sz.x / 2.0 + region_avail_pos.x, region_avail_sz.y / 2.0 + region_avail_pos.y);
        if (_width > region_avail_sz.x || _height > region_avail_sz.y || _pixels.size() != _width * _height) {
            ImGui::End();    
            return;
        }
        auto* drawlist = ImGui::GetWindowDrawList();
        auto scale_x = region_avail_sz.x / (_width * 1.0f);
        auto scale_y = region_avail_sz.y / (_height * 1.0f);
        auto scale = scale_x < scale_y ? scale_x : scale_y;
        ImVec2 start_pos(center_pos.x - (_width / 2.0 * scale), center_pos.y - (_height / 2.0 * scale));
        for (int j = 0; j < _height; ++j) {
            for (int i = 0; i < _width; ++i) {
                auto min_x = start_pos.x + i * scale;
                auto min_y = start_pos.y + j * scale;
                auto max_x = min_x + scale;
                auto max_y = min_y + scale;
                drawlist->AddRectFilled(ImVec2(min_x, min_y), ImVec2(max_x, max_y), _pixels[j * _width + i], 0.0f, ImDrawCornerFlags_None);
            }
        }
        ImGui::End();
    }

    virtual void Exit() override {

    }

    virtual void OnMessage(const SFEMessage& msg) override {
        if (msg.dst != PanelName()) {
            return;
        }
        auto& json_msg = msg.json_msg;
        if (json_msg["command"].asString() == "draw") {
            _width = json_msg["width"].asInt();
            _height = json_msg["height"].asInt();
            auto& data = json_msg["pixels"];
            _pixels.clear();
            for (int i = 0; i < data.size(); ++i) {
                _pixels.emplace_back(data[i].asUInt());
            }
        }
    }

private:
    int _width = 0;
    int _height = 0;
    std::vector<uint32_t> _pixels;
};

} // namespace sfe

extern "C" std::shared_ptr<sfe::SFEPanel> create_panel(const std::string& panel_type) {
    // TODO here
    return std::make_shared<sfe::PanelBitmap>();
}
