#ifndef DRAWTOOLS_H
#define DRAWTOOLS_H

static float tm = 127/255.f;
static ImVec4 arr[] = {{144/255.f,238/255.f,144/255.f,tm},{135/255.f,206/255.f,255/255.f,tm},{255/255.f,0/255.f,0/255.f,tm},{0/255.f,255/255.f,0/255.f,tm},{0 /255.f,255/255.f, 127/255.f,tm}
                      ,{255/255.f,182/255.f,193/255.f,tm},{218/255.f,112/255.f,214/255.f,tm},{248/255.f,248/255.f,255/255.f,tm},{0/255.f,255/255.f,255/255.f,tm},{255/255.f,165/255.f,0/255.f,tm}
                      ,{153/255.f,204/255.f,255/255.f,tm},{204/255.f,255/255.f,153/255.f,tm},{255/255.f,255/255.f,153/255.f,tm},{255/255.f,153/255.f,153/255.f,tm},{153/255.f,153/255.f,204/255.f,tm}
                      ,{204/255.f,204/255.f,204/255.f,tm},{102/255.f,204/255.f,153/255.f,tm},{255/255.f,102/255.f,0/255.f,tm},{102/255.f,204/255.f,204/255.f,tm},{153/255.f,204/255.f,255/255.f,tm}
};
static int length = sizeof(arr)/16;

static struct ColorFly{
    ImVec4 Red={255/255.f,0/255.f,0/255.f,255/255.f};
    ImVec4 Red_={255/255.f,0/255.f,0/255.f,50/255.f};
    ImVec4 Green={0/255.f,255/255.f,0/255.f,255/255.f};
    ImVec4 Green_={0/255.f,255/255.f,0/255.f,50/255.f};
    ImVec4 White={1.0,1.0,1.0,1.0};
    ImVec4 White_={255/255.f,255/255.f,255/255.f,180.f/255.f};
    ImVec4 Black={0/255.f,0/255.f,0/255.f,255.f/255.f};
    ImVec4 Yellow={255/255.f, 255/255.f, 0,255/255.f,};   
}ColorY;

class DrawIo{
public:
    inline void 绘制加粗文本(float size, float x, float y, ImColor color, ImColor color1, const char* str)
    {
        ImGui::GetBackgroundDrawList()->AddText(NULL, size, {(float)(x-0.1), (float)(y-0.1)}, color1, str);
        ImGui::GetBackgroundDrawList()->AddText(NULL, size, {(float)(x+0.1), (float)(y+0.1)}, color1, str);
        ImGui::GetBackgroundDrawList()->AddText(NULL, size, {x, y}, color, str);
    }
    
    inline void 绘制字体描边(float size,int x, int y, ImVec4 color, const char* str)
    {
        ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
        ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x - 0.1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
        ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
        ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
        ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(color), str);
    }
    
    inline void CenterText(float X,float Y,const char* content,float fontSize, ImVec4 color){
        auto textSize = ImGui::CalcTextSize(content, 0, fontSize);
        textSize.x *= fontSize / 25.0f;
        textSize.y *= fontSize / 25.0f;        
        ImGui::GetBackgroundDrawList()->AddText(NULL, fontSize,ImVec2(X - textSize.x / 2, Y), ImGui::ColorConvertFloat4ToU32(color), content);              
    }
    
    inline void DrawRectFilledCorner(int x, int y, int w, int h, ImColor color, int rounding)
    {
        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), (color), rounding, ImDrawCornerFlags_All);
    }
    
    inline void DrawCircleFilled(int x, int y, int radius, ImVec4 color, int segments)
    {
        ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(color), segments);
    }
    
    inline void DrawLine(int x1, int y1, int x2, int y2, ImVec4 color, int size)
    {
        ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(color), size);
    }
    
    inline void DrawRect(int x, int y, int w, int h, ImVec4 color,int size)
    {  //rounding 方框边缘曲率  //rounding_corners_flags 方框边缘弯曲类型  1.ImDrawCornerFlags_All 2.ImDrawCornerFlags_Top 3. ImDrawCornerFlags_Bot 4. ImDrawCornerFlags_Left 5. ImDrawCornerFlags_Right
        ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x+w, y+h), ImGui::ColorConvertFloat4ToU32(color), 0, 0, size);
    }
    
    inline void DrawRect(float x, float y, float x2, float y2, int color,int size)
    {  //rounding 方框边缘曲率  //rounding_corners_flags 方框边缘弯曲类型  1.ImDrawCornerFlags_All 2.ImDrawCornerFlags_Top 3. ImDrawCornerFlags_Bot 4. ImDrawCornerFlags_Left 5. ImDrawCornerFlags_Right
        ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x2, y2), color, 0, 0, size);
    }
    
    inline void DrawRectFilled(int x, int y, int w, int h, ImVec4 color)
    {
        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x+w, y+h), ImGui::ColorConvertFloat4ToU32(color), 0, 0);
    }
    
    inline void DrawCircle(float X, float Y, float radius, float a, float r, float g, float b, float thickness) {
        ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(X, Y), radius, ImColor(r, g, b, a), 0, thickness);
    }
    
    inline void DrawStrokeText(int x, int y, ImVec4 color, const char* str)
    {
        ImGui::GetBackgroundDrawList()->AddText(NULL, 25, ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
        ImGui::GetBackgroundDrawList()->AddText(NULL, 25, ImVec2(x - 0.1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
        ImGui::GetBackgroundDrawList()->AddText(NULL, 25, ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
        ImGui::GetBackgroundDrawList()->AddText(NULL, 25, ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
        ImGui::GetBackgroundDrawList()->AddText(NULL, 25, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(color), str);
    }
    
    inline void DrawTriangleFilled(int x1, int y1, int x2, int y2, int x3, int y3, ImVec4 Color, int T)
    {
        ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::ColorConvertFloat4ToU32(Color));
    }
    
    inline void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, ImVec4 Color, int T)
    {
        ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::ColorConvertFloat4ToU32(Color));
    }
}DrawTf;

#endif
