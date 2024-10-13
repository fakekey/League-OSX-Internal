#define FMT_HEADER_ONLY
#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include "FuncExport.hpp"
#include "GameObject.hpp"
#include "Irelia.hpp"
#include "Offsets.hpp"
#include "TwistedFate.hpp"
#include "Utils.h"
#include "Utils.hpp"
#include "Yasuo.hpp"
#include "Yone.hpp"
#include "imgui.h"
#include "imgui_impl_metal.h"
#include "imgui_impl_osx.h"
#include "rd_route.h"
#include <AppKit/AppKit.hpp>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include <fmt/format.h>
#include <mach/mach_time.h>

/// Some basic colors
namespace Colors {
    static const ImVec4 BLACK = ImVec4(0.06f, 0.06f, 0.06f, 1.f);
    static const ImVec4 WHITE = ImVec4(1.f, 1.f, 1.f, 1.f);
    static const ImVec4 RED = ImVec4(1.f, 0.f, 0.f, 1.f);
    static const ImVec4 DARK_RED = ImVec4(0.6f, 0.f, 0.f, 1.f);
    static const ImVec4 GREEN = ImVec4(0.f, 1.f, 0.f, 1.f);
    static const ImVec4 DARK_GREEN = ImVec4(0.f, 0.6f, 0.f, 1.f);
    static const ImVec4 YELLOW = ImVec4(1.f, 1.f, 0.f, 1.f);
    static const ImVec4 DARK_YELLOW = ImVec4(0.6f, 0.6f, 0.f, 1.f);
    static const ImVec4 CYAN = ImVec4(0.f, 1.f, 1.f, 1.f);
    static const ImVec4 PURPLE = ImVec4(1.f, 0.f, 1.f, 1.f);
    static const ImVec4 GRAY = ImVec4(0.5f, 0.5f, 0.5f, 1.f);
    static const ImVec4 ORANGE = ImVec4(1.f, 0.54f, 0.f, 1.f);
    static const ImVec4 BLUE = ImVec4(0.f, 0.f, 1.f, 1.f);
    static const ImVec4 BROWN = ImVec4(0.54f, 0.27f, 0.06f, 1.f);
    static const ImVec4 SPELL_NOT_READY = ImVec4(0.29f, 0.31f, 0.34f, 1.f);
} // namespace Colors

namespace SizeUtils {
    static const ImVec2 EXPANDED = ImVec2(-FLT_MIN, 0.0f);
}

class SpellDraw {
  public:
    std::string name;
    Vector3 startPos;
    Vector3 endPos;
    std::string type;
    uint8_t skillShotType;
    float radius;
    float width;
    float range;
    float castStartTime;
    float castEndTime;

    SpellDraw(const char *name, const Vector3 &startPos, const Vector3 &endPos,
              std::string type, uint8_t skillShotType, float radius, float width, float range,
              float castStartTime, float castEndTime)
        : name(std::string(name)), type(type), skillShotType(skillShotType), radius(radius), width(width), range(range),
          castStartTime(castStartTime), castEndTime(castEndTime) {
        this->startPos = Vector3{startPos};
        this->endPos = Vector3{endPos};
    }
};

static __int64 *ScreenInstance = nullptr;
static Controller *Control = nullptr;
static uint64 UUID = 0;
static std::map<uint64, std::shared_ptr<SpellDraw>> ListSpell = {};

static bool IsOverlayInit = false;
static NS::Window *pLolWindow = nullptr;
static MTL::Device *pDevice = nullptr;
static MTK::View *pMtkView = nullptr;
static MTL::CommandQueue *pCommandQueue = nullptr;

static Memory::Module *Dll = nullptr;
static Memory::Module *Lol = nullptr;
static GameObject *LocalPlayer = nullptr;
static Chat *ChatClient = nullptr;
static float *GameTime = nullptr;
static ObjectList *HeroList = nullptr;
static ObjectList *MinionList = nullptr;
static ObjectList *TurretList = nullptr;

static bool (*WorldToScreenFn)(_DWORD *, Vector3 *, Vector3 *) = nullptr;
static void WorldToScreen(Vector3 *in, Vector3 *out) {
    WorldToScreenFn((_DWORD *)(*ScreenInstance + 0x270), in, out);
}

static __int64 (*GetHealthBarScreenPositionFn)(__int64, __int64, __int64, float, float) = nullptr;
static void GetHealthBarScreenPosition(GameObject *Obj, Vector3 *out) {
    GetHealthBarScreenPositionFn(*(__int64 *)((__int64)Obj + (0x73A * 0x8)), (__int64)out, *ScreenInstance + 0x270, Obj->MaxHealth, Obj->MaxHealth);
}

static bool IsScreenPointOnScreen(const ImVec2 &point, float offsetX = 0, float offsetY = 0) {
    return point.x > -offsetX && point.x < (pMtkView->bounds().size.width + offsetX) && point.y > -offsetY &&
           point.y < (pMtkView->bounds().size.height + offsetY);
}

static void InitImGui() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.DeltaTime = 1.0f / 60.0f;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();

    // Setup Renderer backend
    ImGui_ImplMetal_Init(pDevice);
    ImGui_ImplOSX_Init(pMtkView);

    ImGui::GetStyle().FrameRounding = 4.0f;
    ImGui::GetStyle().GrabRounding = 4.0f;
    style.WindowPadding = ImVec2(10, 10);        // Padding within a window
    style.WindowMinSize = ImVec2(100, 100);      // Minimum window size
    style.WindowRounding = 0.0f;                 // Radius of window corners rounding. Set to 0.0f to have rectangular windows
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f); // Alignment for title bar text
    style.ChildRounding = 0.0f;                  // Radius of child window corners rounding. Set to 0.0f to have rectangular child windows
    style.FramePadding = ImVec2(5, 5);           // Padding within a framed rectangle (used by most widgets)
    style.FrameRounding = 0.0f;                  // Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
    style.ItemSpacing = ImVec2(5, 5);            // Horizontal and vertical spacing between widgets/lines
    style.ItemInnerSpacing = ImVec2(4, 4);       // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
    style.TouchExtraPadding = ImVec2(0, 0);      // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    style.IndentSpacing = 21.0f;                 // Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
    style.ColumnsMinSpacing = 6.0f;              // Minimum horizontal spacing between two columns
    style.ScrollbarSize = 12.0f;                 // Width of the vertical scrollbar, Height of the horizontal scrollbar
    style.ScrollbarRounding = 2.0f;              // Radius of grab corners rounding for scrollbar
    style.GrabMinSize = 10.0f;                   // Minimum width/height of a grab box for slider/scrollbar
    style.GrabRounding = 0.0f;                   // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.DisplayWindowPadding = ImVec2(22, 22);
    style.DisplaySafeAreaPadding = ImVec2(4, 4);
    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
    style.CurveTessellationTol = 1.25f;

    ImVec4 *colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
    colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 0.40f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.60f, 0.90f, 0.50f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.10f, 0.10f, 0.10f, 1.f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.70f, 0.20f, 0.00f, 0.83f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.50f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

static void DrawCircleAt(ImDrawList *canvas, const Vector3 &worldPos, float radius, bool filled, int numPoints, ImColor color, float thickness) {
    if (numPoints >= 200)
        return;

    static ImVec2 points[200];
    float step = 6.2831f / numPoints;
    float theta = 0.f;

    for (int i = 0; i < numPoints; i++, theta += step) {
        Vector3 worldSpace = {worldPos.x + radius * cos(theta), worldPos.y, worldPos.z - radius * sin(theta)};
        Vector3 screenSpace{};
        WorldToScreen(&worldSpace, &screenSpace);
        points[i].x = screenSpace.x;
        points[i].y = screenSpace.y;
    }

    if (filled)
        canvas->AddConvexPolyFilled(points, numPoints, color);
    else
        canvas->AddPolyline(points, numPoints, color, true, thickness);
}

static void DrawCircleWorld(const Vector3 &center, float radius, int numPoints, float thickness, const ImVec4 &color) {
    DrawCircleAt(ImGui::GetBackgroundDrawList(), center, radius, false, numPoints, ImColor(color), thickness);
}

static void DrawTextAt(ImDrawList *canvas, const Vector3 &pos, const ImVec2 &offset, const char *text, ImColor color) {
    Vector3 worldPos{pos};
    Vector3 screenPos{};
    WorldToScreen(&worldPos, &screenPos);
    canvas->AddText(ImVec2(screenPos.x + offset.x, screenPos.y + offset.y), color, text);
}

static void DrawTextWorld(const Vector3 &pos, const ImVec2 &offset, const char *text, const ImVec4 &color) {
    DrawTextAt(ImGui::GetBackgroundDrawList(), pos, offset, text, ImColor(color));
}

static void DrawRectWorld(ImDrawList *canvas, const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, const Vector3 &p4, float thickness, const ImVec4 &color) {
    static ImVec2 points[4];
    Vector3 screenSpace{};

    Vector3 worldSpace = {p1};
    WorldToScreen(&worldSpace, &screenSpace);
    points[0].x = screenSpace.x;
    points[0].y = screenSpace.y;

    worldSpace = {p2};
    WorldToScreen(&worldSpace, &screenSpace);
    points[1].x = screenSpace.x;
    points[1].y = screenSpace.y;

    worldSpace = {p3};
    WorldToScreen(&worldSpace, &screenSpace);
    points[2].x = screenSpace.x;
    points[2].y = screenSpace.y;

    worldSpace = {p4};
    WorldToScreen(&worldSpace, &screenSpace);
    points[3].x = screenSpace.x;
    points[3].y = screenSpace.y;

    canvas->AddPolyline(points, 4, ImColor(color), true, thickness);
}

static void DrawRectFilled(const Vector4 &box, const ImVec4 &color, float rounding = 0) {
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(box.x, box.y), ImVec2(box.z, box.w), ImColor(color), rounding);
}

static void DrawRectFromPos(const Vector3 &startPos, const Vector3 &endPos, float radius, float thickness, const ImVec4 &color) {
    Vector3 dir = (endPos - startPos).normalize();
    if (std::isnan(dir.x))
        return;

    Vector3 leftDir = dir.rotate_y(90.f).scale(radius);
    Vector3 rightDir = dir.rotate_y(-90.f).scale(radius);
    Vector3 p1 = Vector3(startPos.x + leftDir.x, startPos.y + leftDir.y, startPos.z + leftDir.z);
    Vector3 p2 = Vector3(endPos.x + leftDir.x, endPos.y + leftDir.y, endPos.z + leftDir.z);
    Vector3 p3 = Vector3(endPos.x + rightDir.x, endPos.y + rightDir.y, endPos.z + rightDir.z);
    Vector3 p4 = Vector3(startPos.x + rightDir.x, startPos.y + rightDir.y, startPos.z + rightDir.z);

    DrawRectWorld(ImGui::GetBackgroundDrawList(), p1, p2, p3, p4, thickness, color);
}

static void DrawAARange() {
    for (int i = 0; i < HeroList->Size; i++) {
        GameObject *Champ = HeroList->Item[i];
        if (Champ->IsAlive() && (Champ->IsEnemyTo(LocalPlayer) || Champ->NetworkId == LocalPlayer->NetworkId)) {
            Vector3 worldPos{Champ->Position};
            Vector3 screenPos{};
            WorldToScreen(&worldPos, &screenPos);
            if (IsScreenPointOnScreen(ImVec2(screenPos.x, screenPos.y))) {
                DrawCircleWorld(Champ->Position, Champ->AttackRange + Champ->Info()->gameplayRadius, 100, 1, Colors::WHITE);
                auto txt = Champ->GetName();
                auto txtSize = ImGui::CalcTextSize(txt);
                DrawTextWorld(Champ->Position, ImVec2(-txtSize.x / 2, 4), txt, Colors::WHITE);
            }
        }
    }
}

static void DrawTurretRange() {
    for (int i = 0; i < TurretList->Size; i++) {
        GameObject *Turret = TurretList->Item[i];
        if (Turret->Visible && Turret->IsAlive() && Turret->IsEnemyTo(LocalPlayer)) {
            Vector3 worldPos{Turret->Position};
            Vector3 screenPos{};
            WorldToScreen(&worldPos, &screenPos);
            if (IsScreenPointOnScreen(ImVec2(screenPos.x, screenPos.y))) {
                DrawCircleWorld(Turret->Position, 750 + Turret->Info()->gameplayRadius, 100, 2, Colors::RED);
            }
        }
    }
}

static void DrawSkillTrack() {
    ImGui::SetNextWindowPos(ImVec2{0, ImGui::GetMainViewport()->GetCenter().y}, ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2{800, 0}, ImGuiCond_Once);
    ImGui::Begin("Skill Track", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
    for (int i = 0; i < HeroList->Size; i++) {
        GameObject *Champ = HeroList->Item[i];
        if (!Champ->IsAllyTo(LocalPlayer) || strcmp(Champ->GetName(), "PracticeTool_TargetDummy") == 0)
            continue;

        ImGui::Image(Champ->Icon(), ImVec2(24, 24));
        for (int i = -1; i < 6; i++) {
            if (i == -1)
                i = 0x3F;

            ImGui::SameLine();
            auto spell = Champ->SpellBook.GetSpellSlotByID(i);
            auto spellState = Champ->SpellBook.GetSpellStateByID(i);
            ImGui::Image(spell->Info->Data->Icon(), ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1),
                         spellState == Offset::SpellState::Ready || spellState == Offset::SpellState::PlayerReady ||
                                 spellState == Offset::SpellState::ActiveReady
                             ? Colors::WHITE
                             : Colors::SPELL_NOT_READY);

            auto coolDown = spell->ReadyAt - *GameTime;
            if (coolDown > 0.f) {
                auto startPos = ImGui::GetItemRectMin();
                std::string txt;
                if (coolDown > 1.f) {
                    txt = fmt::format("{:.0f}", coolDown);
                } else if (coolDown < 1.f) {
                    txt = fmt::format("{:.1f}", coolDown);
                }
                ImGui::SetWindowFontScale(0.9f);
                auto txtSize = ImGui::CalcTextSize(txt.c_str());
                ImGui::GetWindowDrawList()->AddText(ImVec2(startPos.x + (24 - txtSize.x) / 2, startPos.y + (24 - txtSize.y) / 2),
                                                    ImColor(Colors::WHITE), txt.c_str());
                ImGui::SetWindowFontScale(1.0f);
            } else if (spellState == Offset::SpellState::NoMana || spellState == Offset::SpellState::PlayerNoMana) {
                auto startPos = ImGui::GetItemRectMin();
                ImGui::GetWindowDrawList()->AddImage(GameData::GetUnitIconByName("outofchakra"), startPos, ImVec2(startPos.x + 24, startPos.y + 24));
            }

            if (i == 0x3F)
                i = -1;
        }
    }
    ImGui::End();
}

static std::mutex SpellLocker;
static void DrawSpellCast() {
    std::set<uint64> listKeyToErase = {};
    listKeyToErase.clear();
    SpellLocker.lock();
    for (auto const &[key, val] : ListSpell) {
        if (*GameTime > val->castEndTime + Control->GetPing()) {
            listKeyToErase.insert(key);
        }
    }
    for (auto const &key : listKeyToErase) {
        ListSpell.erase(key);
    }
    for (auto const &[key, val] : ListSpell) {
        if (val->skillShotType == Offset::SkillShotType::LINE) {
            DrawRectFromPos(val->startPos, val->endPos, val->width, 2, Colors::GREEN);
        } else if (val->skillShotType == Offset::SkillShotType::CIRCLE) {
            DrawCircleWorld(val->endPos, val->radius, 100, 2, Colors::GREEN);
        }
        if (val->skillShotType == Offset::SkillShotType::UNKNOWN ||
            (val->skillShotType == Offset::SkillShotType::LINE && val->width == 0.f) ||
            (val->skillShotType == Offset::SkillShotType::CIRCLE && val->radius == 0.f)) {
            Control->PrintChat(fmt::format("{:s}: start ({:f},{:f},{:f}), end ({:f},{:f},{:f})", val->name,
                                           val->startPos.x, val->startPos.y, val->startPos.z,
                                           val->endPos.x, val->endPos.y, val->endPos.z)
                                   .c_str());
        }
    }
    SpellLocker.unlock();
}

static void DrawHealthPrediction() {
    {
        for (int i = 0; i < HeroList->Size; i++) {
            GameObject *Champ = HeroList->Item[i];
            if (Champ->IsAllyTo(LocalPlayer))
                continue;
            if (Champ->HasUnitTag("Unit_Champion_Clone"))
                continue;
            if (!Champ->Visible || !Champ->IsAlive())
                continue;

            Vector3 screenPos{};
            WorldToScreen(&Champ->Position, &screenPos);
            if (!IsScreenPointOnScreen(ImVec2(screenPos.x, screenPos.y)))
                continue;

            GetHealthBarScreenPosition(Champ, &screenPos);
            auto HPBarStartPos = screenPos.x - 46;
            auto HPBarWidth = 106.f;
            auto PercentHP = Champ->Health / Champ->MaxHealth * 100.f;
            auto HPBarEndPos = PercentHP * HPBarWidth / 100.f;
            DrawRectFilled(Vector4{HPBarStartPos, screenPos.y - 25, HPBarStartPos + HPBarEndPos, screenPos.y - 12},
                           Control->IsLasthitable(LocalPlayer, Champ) ? Colors::WHITE : Colors::RED);
        }
    }

    {
        for (int i = 0; i < MinionList->Size; i++) {
            GameObject *Minion = MinionList->Item[i];
            if (Minion->IsAllyTo(LocalPlayer))
                continue;
            if (Minion->HasUnitTag("Unit_Champion_Clone") || !Minion->HasUnitTag("Unit_Minion_Lane"))
                continue;
            if (!Minion->Visible || !Minion->IsAlive())
                continue;
            if (Controller::blacklistedObjectNames.find(Character::ToLower(Minion->GetName())) != Controller::blacklistedObjectNames.end())
                continue;

            Vector3 screenPos{};
            WorldToScreen(&Minion->Position, &screenPos);
            if (!IsScreenPointOnScreen(ImVec2(screenPos.x, screenPos.y)))
                continue;

            GetHealthBarScreenPosition(Minion, &screenPos);
            auto HPBarStartPos = screenPos.x - 30;
            auto HPBarWidth = 60.f;
            if (Minion->HasUnitTag("Unit_Minion_Lane_Super")) {
                HPBarStartPos -= 16;
                HPBarWidth += 31;
            }
            auto PercentHP = Minion->Health / Minion->MaxHealth * 100.f;
            auto HPBarEndPos = PercentHP * HPBarWidth / 100.f;
            DrawRectFilled(Vector4{HPBarStartPos, screenPos.y - 6, HPBarStartPos + HPBarEndPos, screenPos.y - 2},
                           Control->IsLasthitable(LocalPlayer, Minion) ? Colors::WHITE : Colors::RED);
        }
    }
}

static bool IsDrawAARange = true;
static bool IsDrawTurretRange = true;
static bool IsDrawSkillTrack = true;
static bool IsDrawSpellCast = true;
static bool IsDrawHealthPrediction = true;

static void DrawUI(bool IsShowMenu) {
    NS::AutoreleasePool *pPool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer *pCmd = pCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor *pRpd = pMtkView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder *pEnc = pCmd->renderCommandEncoder(pRpd);

    // Start the Dear ImGui frame
    ImGui_ImplMetal_NewFrame(pRpd);
    ImGui_ImplOSX_NewFrame(pMtkView);
    ImGui::NewFrame();

    // Draw stuff here
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize.x = pMtkView->bounds().size.width;
    io.DisplaySize.y = pMtkView->bounds().size.height;

    if (IsShowMenu) {
        ImGui::SetNextWindowPos(ImVec2{16, 16}, ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2{800, 0}, ImGuiCond_Once);
        ImGui::SetNextWindowSizeConstraints(ImVec2{0, 0}, ImVec2{800, ImGui::GetMainViewport()->Size.y / 2 - 48});
        ImGui::Begin("Main Menu", nullptr,
                     ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoMove);
        ImGui::LabelText("[Shift]", "[Show/Hide] Menu");
        ImGui::Button("Save all scripts");
        ImGui::SameLine();
        ImGui::Button("Reload all scripts");

        if (ImGui::CollapsingHeader("Draw Auto Attack Range")) {
            ImGui::LabelText("fakekey", "[Author]");
            ImGui::Checkbox("Enabled##AA", &IsDrawAARange);
            ImGui::Separator();
            ImGui::Button("About##AA", SizeUtils::EXPANDED);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary)) {
                ImGui::SetTooltip("Display your and enemies's auto attack range");
            }
            ImGui::Separator();
        }
        if (ImGui::CollapsingHeader("Draw Turret Range")) {
            ImGui::LabelText("fakekey", "[Author]");
            ImGui::Checkbox("Enabled##Turret", &IsDrawTurretRange);
            ImGui::Separator();
            ImGui::Button("About##Turret", SizeUtils::EXPANDED);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary)) {
                ImGui::SetTooltip("Display turret's attack range");
            }
            ImGui::Separator();
        }
        if (ImGui::CollapsingHeader("Draw Skill Track")) {
            ImGui::LabelText("fakekey", "[Author]");
            ImGui::Checkbox("Enabled##SkillTrack", &IsDrawSkillTrack);
            ImGui::Separator();
            ImGui::Button("About##SkillTrack", SizeUtils::EXPANDED);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary)) {
                ImGui::SetTooltip("Display skill tracker");
            }
            ImGui::Separator();
        }
        if (ImGui::CollapsingHeader("Draw Spell Cast")) {
            ImGui::LabelText("fakekey", "[Author]");
            ImGui::Checkbox("Enabled##SpellCast", &IsDrawSpellCast);
            ImGui::Separator();
            ImGui::Button("About##SpellCast", SizeUtils::EXPANDED);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_Stationary)) {
                ImGui::SetTooltip("Display spell cast");
            }
            ImGui::Separator();
        }

        ImGui::End();
    }

    DrawHealthPrediction();
    if (IsDrawSpellCast)
        DrawSpellCast();
    if (IsDrawAARange)
        DrawAARange();
    if (IsDrawTurretRange)
        DrawTurretRange();
    if (IsDrawSkillTrack)
        DrawSkillTrack();

    // Rendering
    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    ImGui_ImplMetal_RenderDrawData(draw_data, pCmd, pEnc);

    pEnc->endEncoding();
    pCmd->presentDrawable(pMtkView->currentDrawable());
    pCmd->commit();

    pPool->release();
}

static Script *Combo = nullptr;
static uint64 LastLoop = 0;
static float LastScript = 0.f;
static float LastAttack = 0.f;
static float LastMove = 0.f;

static void MainStuff() {
    // Draw
    DrawUI(Utils::IsPressed(kVK_Shift));

    if (mach_absolute_time() > LastLoop + 16000000) {
        // Orbwalk
        if (Utils::IsPressed(kVK_Space) && !ChatClient->IsChatOpen) {
            GameObject *Target = Control->GetBestChampInRange(LocalPlayer, HeroList);
            if (Target != nullptr && *GameTime > LastAttack + LocalPlayer->GetAttackDelay()) {
                Control->Attack(LocalPlayer, Target);
                LastAttack = *GameTime;
            } else if (*GameTime > LastMove + 0.03 && *GameTime > LastAttack + LocalPlayer->GetAttackCastDelay() + Control->GetPing() * 2) {
                Vector3 *CurPos = Control->GetCursorPos();
                Control->Move(LocalPlayer, CurPos);
                LastMove = *GameTime;
            }
        } else if (Utils::IsPressed(kVK_ANSI_C) && !ChatClient->IsChatOpen) {
            GameObject *Target = Control->GetBestMinionInRange(LocalPlayer, MinionList);
            if (Target != nullptr && *GameTime > LastAttack + LocalPlayer->GetAttackDelay()) {
                Control->Attack(LocalPlayer, Target);
                LastAttack = *GameTime;
            } else if (*GameTime > LastMove + 0.03 && *GameTime > LastAttack + LocalPlayer->GetAttackCastDelay() + Control->GetPing() * 2) {
                Vector3 *CurPos = Control->GetCursorPos();
                Control->Move(LocalPlayer, CurPos);
                LastMove = *GameTime;
            }
        } else if (Utils::IsPressed(kVK_ANSI_X) && !ChatClient->IsChatOpen) {
            GameObject *Target = Control->GetMinionLastHitInRange(LocalPlayer, MinionList);
            if (Target != nullptr && *GameTime > LastAttack + LocalPlayer->GetAttackDelay()) {
                Control->Attack(LocalPlayer, Target);
                LastAttack = *GameTime;
            } else if (*GameTime > LastMove + 0.03 && *GameTime > LastAttack + LocalPlayer->GetAttackCastDelay() + Control->GetPing() * 2) {
                Vector3 *CurPos = Control->GetCursorPos();
                Control->Move(LocalPlayer, CurPos);
                LastMove = *GameTime;
            }
        }
        if (Combo != nullptr) {
            if (*GameTime > LastScript + 0.03 && *GameTime > LastAttack + LocalPlayer->GetAttackCastDelay() + Control->GetPing() * 2) {
                Combo->Run();
                LastScript = *GameTime;
            }
        }

        LastLoop = mach_absolute_time();
    }
}

static char (*LolDraw)(__int64, __int64, _DWORD *, _DWORD *, unsigned __int8) = nullptr;
static char LolDrawHook(__int64 a1, __int64 a2, _DWORD *a3, _DWORD *a4, unsigned __int8 a5) {
    if (!IsOverlayInit) {
        pLolWindow = *Lol->GetType<NS::Window **>(Offset::World::LolWindow);
        pCommandQueue = *(MTL::CommandQueue **)(*(_QWORD *)(a1 + 0x78) + 0x160);
        pDevice = pCommandQueue->device();
        pMtkView = MTK::View::alloc()->init(pLolWindow->contentView()->frame(), pDevice);
        pMtkView->setPreferredFramesPerSecond(0);
        pMtkView->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 0.0, 0.0));
        pMtkView->layer()->setOpaque(false);
        pLolWindow->contentView()->addSubview(pMtkView);

        // Init ImGui
        InitImGui();
        IsOverlayInit = true;

        // Load DB
        Dll = new Memory::Module("libHucker.dylib");
        Dl_info DllInfo;
        dladdr(reinterpret_cast<void *>(Dll->Address), &DllInfo);
        std::string FullPath(DllInfo.dli_fname);
        std::string DataPath = FullPath.substr(0, FullPath.find_last_of('/')) + "/Data";
        GameData::Initialize(DataPath, pDevice);
        Control->PrintChat("Welcome! Hold Shift to Open Menu");
    }

    if (IsOverlayInit)
        MainStuff();

    return LolDraw(a1, a2, a3, a4, a5);
}

static __int64 (*OnProcessSpell)(__int64, __int64) = nullptr;
static __int64 OnProcessSpellHook(__int64 a1, __int64 a2) {
    GameObject *Champ = (GameObject *)(a1 - 0x2768);
    if (Champ->HasUnitTag("Unit_Champion")) {
        SpellCastInfo *spellInfo = (SpellCastInfo *)a2;
        if (spellInfo->Info != nullptr && spellInfo->Info->Data != nullptr) {
            auto spellInfoDB = spellInfo->Info->Data->Info();
            if (spellInfoDB != nullptr && spellInfo->StartPos != spellInfo->EndPos) {
                UUID += 1ULL;
                SpellLocker.lock();
                ListSpell.insert(std::pair<uint64, std::shared_ptr<SpellDraw>>(
                    UUID,
                    std::make_shared<SpellDraw>(spellInfo->Info->Id, spellInfo->StartPos, spellInfo->EndPos,
                                                spellInfoDB->type, spellInfoDB->skillShotType, spellInfoDB->castRadius, spellInfoDB->width, spellInfoDB->castRange,
                                                spellInfo->StartTime, spellInfo->StartTime + spellInfoDB->delay)));
                SpellLocker.unlock();
            }
        }
    }

    return OnProcessSpell(a1, a2);
}

static void Initialize() {
    // Init
    Lol = new Memory::Module("LeagueofLegends");
    ScreenInstance = Lol->GetType<__int64 *>(Offset::World::ScreenInstance);
    OSpellBook::Lol = Lol;
    GameObject::Lol = Lol;
    LocalPlayer = Lol->GetStruct<GameObject *>(Offset::LocalPlayer::PlayerInstance);
    ChatClient = Lol->GetStruct<Chat *>(Offset::Chat::ChatClient);
    GameTime = Lol->GetType<float *>(Offset::World::GameTime);
    HeroList = Lol->GetStruct<ObjectList *>(Offset::World::HeroList);
    MinionList = Lol->GetStruct<ObjectList *>(Offset::World::MinionList);
    TurretList = Lol->GetStruct<ObjectList *>(Offset::World::TurretList);
    Control = new Controller(Lol);

    // Init Script
    Script::Lol = Lol;
    Script::Control = Control;
    Script::LocalPlayer = LocalPlayer;
    Script::HeroList = HeroList;
    Script::MinionList = MinionList;
    Script::ChatClient = ChatClient;

    if (strcmp("Irelia", LocalPlayer->GetName()) == 0) {
        Combo = new Irelia();
    } else if (strcmp("Yasuo", LocalPlayer->GetName()) == 0) {
        Combo = new Yasuo();
    } else if (strcmp("Yone", LocalPlayer->GetName()) == 0) {
        Combo = new Yone();
    } else if (strcmp("TwistedFate", LocalPlayer->GetName()) == 0) {
        Combo = new TwistedFate();
    }
    if (Combo != nullptr) {
        Combo->Load();
    }

    // Hooks
    LolDrawFunc LolDrawFn = Lol->GetType<LolDrawFunc>(Offset::World::LolDraw);
    OnProcessSpellFunc OnProcessSpellFn = Lol->GetType<OnProcessSpellFunc>(Offset::Control::OnProcessSpell);
    WorldToScreenFn = Lol->GetType<WorldToScreenFunc>(Offset::World::WorldToScreen);
    GetHealthBarScreenPositionFn = Lol->GetType<GetHealthBarScreenPositionFunc>(Offset::World::GetHealthBarScreenPosition);
    rd_route((void *)LolDrawFn, (void *)LolDrawHook, (void **)&LolDraw);
    rd_route((void *)OnProcessSpellFn, (void *)OnProcessSpellHook, (void **)&OnProcessSpell);
}

static void Deinitialize() {
    GameData::Deallocate();
    ShowDialog("Info", "Released");
}

__attribute__((constructor)) void Load() {
    Initialize();
}

__attribute__((destructor)) void Unload() {
    Deinitialize();
}