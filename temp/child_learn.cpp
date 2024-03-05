#include <windows.h>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
#include <map>
#pragma comment(lib, "opengl32.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WND_WIDTH{300};
int WND_HEIGTH{600};
int text_y;

std::map<int, std::string> items;
std::unordered_set<int> break_points;

void add_item(int index, std::string item_text)
{
    items.insert({index, item_text});
}

// x, y of Top left
void draw_arrow(HDC hdc, int x, int y)
{
    MoveToEx(hdc, x, y + (text_y / 2), nullptr);
    LineTo(hdc, x + text_y, y + (text_y / 2));
    LineTo(hdc, x + text_y - ((text_y) / 2), y);
    MoveToEx(hdc, x + text_y, y + (text_y / 2), nullptr);
    LineTo(hdc, x + text_y - ((text_y) / 2), y + text_y);
}

int exec_index;
void set_execution_index(int idx)
{
    exec_index = idx;
}

int main()
{
    HINSTANCE instance = GetModuleHandle(nullptr);
    static TCHAR szAppName[] = TEXT("SysMets1");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = instance;
    wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    wndclass.lpszMenuName = nullptr;
    wndclass.lpszClassName = szAppName;
    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("Cannot Register Class"), szAppName, MB_ICONERROR);
        return 1;
    }

    add_item(0, "LD SP, 0xfffe");
    add_item(3, "XOR A");
    add_item(0x0051, "LD L, 0x0f");

    hwnd = CreateWindow(szAppName, TEXT("Test"), WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_VSCROLL,
                        CW_USEDEFAULT, CW_USEDEFAULT, WND_WIDTH, WND_HEIGTH, NULL, NULL, instance,
                        NULL);

    set_execution_index(0);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int mouse_x{};
    static int mouse_y{};

    static int scroll_pos{0};

    static int dot_y{-1};

    static int client_y;

    static HBRUSH red_brush;
    static HPEN arrow_pen;

    switch (message)
    {
    case WM_CREATE: {
        auto hdc = GetDC(hwnd);
        TEXTMETRIC tm;
        GetTextMetrics(hdc, &tm);
        text_y = tm.tmHeight + tm.tmExternalLeading;
        red_brush = CreateSolidBrush(RGB(255, 0, 0));
        arrow_pen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));

        RECT r;
        GetClientRect(hwnd, &r);
        client_y = r.bottom - r.top;

        SetScrollRange(hwnd, SB_VERT, 0, items.size() - (client_y / text_y), false);
        SetScrollPos(hwnd, SB_VERT, scroll_pos, TRUE);

        ReleaseDC(hwnd, hdc);
        return S_OK;
    }
    case WM_VSCROLL: {
        switch (LOWORD(wParam))
        {
        case SB_LINEDOWN:
            scroll_pos += 1;
            break;
        case SB_LINEUP:
            scroll_pos -= 1;
            break;
        case SB_PAGEUP:
            scroll_pos -= client_y / text_y;
            break;
        case SB_PAGEDOWN:
            scroll_pos += client_y / text_y;
            break;
        case SB_THUMBPOSITION:
            scroll_pos = HIWORD(wParam);
            break;
        default:
            break;
        }

        scroll_pos =
            std::max<int>(0, std::min<int>(scroll_pos, items.size() - (client_y / text_y)));

        if (scroll_pos != GetScrollPos(hwnd, SB_VERT))
        {
            SetScrollPos(hwnd, SB_VERT, scroll_pos, TRUE);
            InvalidateRect(hwnd, nullptr, true);
        }

        return S_OK;
    }
    case WM_LBUTTONDOWN: {

        if (exec_index < items.size() - 1)
            ++exec_index;
        int dot_y = mouse_y / text_y + scroll_pos;
        if (dot_y >= items.size())
            return 0;

        auto iter = break_points.find(dot_y);
        if (iter == break_points.end())
            break_points.insert(dot_y);
        else
            break_points.erase(iter);

        InvalidateRect(hwnd, nullptr, true);
        return S_OK;
    }

    case WM_MOUSEMOVE:
        mouse_x = LOWORD(lParam);
        mouse_y = HIWORD(lParam);
        return S_OK;
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        auto hdc = ::BeginPaint(hwnd, &ps);
        SetBkMode(hdc, TRANSPARENT);

        int offset{};
        for (auto const &[idx, text] : items)
        {
            auto const s = std::to_string(idx) + ": ";
            ::TextOut(hdc, text_y + 4, offset * text_y - (scroll_pos * text_y), s.c_str(),
                      s.size());
            ::TextOut(hdc, text_y + 40, offset * text_y - (scroll_pos * text_y), text.c_str(),
                      text.size());
            ++offset;
        }
        // break points
        auto old_brush = SelectObject(hdc, red_brush);
        for (int bp : break_points)
            Ellipse(hdc, 0, bp * text_y - (scroll_pos * text_y), text_y,
                    bp * text_y + text_y - (scroll_pos * text_y));
        SelectObject(hdc, old_brush);

        // execution arrow
        auto old_pen = SelectObject(hdc, arrow_pen);
        draw_arrow(hdc, 0, exec_index * text_y - (scroll_pos * text_y));
        SelectObject(hdc, old_pen);

        ::EndPaint(hwnd, &ps);
        return S_OK;
    }
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return S_OK;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}