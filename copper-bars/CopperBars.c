
#include <windows.h>
#include <math.h>

DWORD pixels[640 * 480] = {0};
BITMAPINFO bmi = {0};

int barY = 240;


void render(int barY)
{
    for (int y = 0; y < 480; y++)
    {
        for (int x = 0; x < 640; x++)
        {
            int index = y * 640 + x;

            if (y >= barY && y < barY + 40)
            {
                int green = (y - barY) * 255 / 39;
                pixels[index] = (255 << 16) | (green << 8);
            }

            else if (y >= barY + 20 && y < barY + 100)
            {
                int green = (y - barY - 60) * 255 / 39;
                pixels[index] = (green << 8);
            }

            else if (y >= barY + 40 && y < barY + 160)
            {
                int blue = (y - barY - 120) * 255 / 39;
                pixels[index] = (170 << 16) | blue;
            }

            else
            {
                pixels[index] = 0;
            }
        }
    }
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }

    if (uMsg == WM_PAINT)
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        StretchDIBits(
            hdc,
            0, 0, 640, 480,
            0, 0, 640, 480,
            pixels,
            &bmi,
            DIB_RGB_COLORS,
            SRCCOPY
        );

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 255, 209));

        TextOut(hdc, 230, barY, "HALTCON", 7);

        EndPaint(hwnd, &ps);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "CopperBarsWindow";

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = 640;
    bmi.bmiHeader.biHeight = -480;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "HALTCON DemoScene - Copper Bars",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        640, 480,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
        return 1;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {0};

    bool running = TRUE;
    ULONGLONG start = GetTickCount64();

    while (running)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = 0;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!running)
            break;

        double time = (GetTickCount64() - start) / 1000.0;

        barY = 240 + (int)(200 * sin(time));

        render(barY);

        InvalidateRect(hwnd, NULL, FALSE);

        Sleep(16);
    }

    return 0;
}
