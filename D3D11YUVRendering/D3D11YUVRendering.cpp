// D3D11YUVRendering.cpp : Defines the entry point for the application.
//


#include "D3D11YUVRendering.h"
#include "OutputManager.h"

#define MAX_LOADSTRING 100

char buf[1024];

struct YUVFrame
{
	UINT width;
	UINT height;
	UINT pitch;
	BYTE* Y;
	BYTE* U;
	BYTE* V;
};

enum YUV
{
	YUV444,
	YUV420
};

enum YUV yuv = YUV444; // Change it accordingly

//
// Globals
//
OUTPUTMANAGER OutMgr;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWnd;										// The window handle created

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);



void WriteYUV444ToTexture(YUVFrame *yuvFrame);
void WriteYUV420ToTexture(YUVFrame *yuvFrame);
YUVFrame* ReadYUV420FromFile();
YUVFrame* ReadYUV444FromFile();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_D3D11YUVRENDERING, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_D3D11YUVRENDERING));

	RECT DeskBounds;
	bool FirstTime = true;
	bool Occluded = true;

	MSG msg = { 0 };

	while (WM_QUIT != msg.message)
	{
		DUPL_RETURN Ret = DUPL_RETURN_SUCCESS;
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == OCCLUSION_STATUS_MSG)
			{
				// Present may not be occluded now so try again
				Occluded = false;
			}
			else
			{
				// Process window messages
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else if (FirstTime)
		{

			// First time through the loop so nothing to clean up
			FirstTime = false;

			// Re-initialize
			Ret = OutMgr.InitOutput(hWnd, &DeskBounds);

			// We start off in occluded state and we should immediate get a occlusion status window message
			Occluded = true;
		}
		else
		{
			// Nothing else to do, so try to present to write out to window if not occluded
			if (!Occluded)
			{
				YUVFrame *yuvFrame = (yuv == YUV444) ? ReadYUV444FromFile() : ReadYUV420FromFile();
				(yuv == YUV444) ? WriteYUV444ToTexture(yuvFrame) : WriteYUV420ToTexture(yuvFrame);
				free(yuvFrame->Y);
				free(yuvFrame->U);
				free(yuvFrame->V);
				free(yuvFrame);

				Ret = OutMgr.UpdateApplicationWindow(&Occluded);
			}
		}
	}

	if (msg.message == WM_QUIT)
	{
		OutMgr.CleanRefs();
		// For a WM_QUIT message we should return the wParam value
		return static_cast<INT>(msg.wParam);
	}

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_D3D11YUVRENDERING));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= nullptr;//MAKEINTRESOURCEW(IDC_D3D11YUVRENDERING);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	case WM_SIZE:
	{
		// Tell output manager that window size has changed
		OutMgr.WindowResize();
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}



YUVFrame* ReadYUV444FromFile()
{

	FILE *file = nullptr;
	sprintf_s(buf, "content\\yuv444.bmp");
	fopen_s(&file, buf, "rb");
	int size = sizeof(YUVFrame);
	YUVFrame *yuvFrame = (YUVFrame*)malloc(sizeof(YUVFrame));
	int readBytes = fread(yuvFrame, sizeof(YUVFrame), 1, file);

	size = yuvFrame->pitch * yuvFrame->height;
	yuvFrame->Y = (BYTE *)malloc(size);
	readBytes = fread(yuvFrame->Y, 1, size, file);

	size = (yuvFrame->pitch * yuvFrame->height);
	yuvFrame->U = (BYTE *)malloc(size);
	readBytes = fread(yuvFrame->U, 1, size, file);

	size = (yuvFrame->pitch * yuvFrame->height);
	yuvFrame->V = (BYTE *)malloc(size);
	readBytes = fread(yuvFrame->V, 1, size, file);

	fclose(file);

	return yuvFrame;
}

YUVFrame* ReadYUV420FromFile()
{

	FILE *file = nullptr;
	sprintf_s(buf, "content\\yuv420.bmp");
	fopen_s(&file, buf, "rb");

	int size = sizeof(YUVFrame);
	YUVFrame *yuvFrame = (YUVFrame*)malloc(size);
	int readBytes = fread(yuvFrame, size, 1, file);

	size = yuvFrame->pitch * yuvFrame->height;
	yuvFrame->Y = (BYTE *)malloc(size);
	readBytes = fread(yuvFrame->Y, size, 1, file);

	size = yuvFrame->pitch/2 * yuvFrame->height / 2;
	yuvFrame->U = (BYTE *)malloc(size);
	readBytes = fread(yuvFrame->U, size, 1, file);

	size = yuvFrame->pitch/2 * yuvFrame->height / 2;
	yuvFrame->V = (BYTE *)malloc(size);
	readBytes = fread(yuvFrame->V, size, 1, file);

	fclose(file);

	return yuvFrame;
}

void WriteYUV444ToTexture(YUVFrame *yuvFrame)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	UINT subresource = D3D11CalcSubresource(0, 0, 0);
	HRESULT hr = OutMgr.m_DeviceContext->Map(OutMgr.m_SharedSurf, subresource, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	BYTE* dptr = reinterpret_cast<BYTE*>(resource.pData);

	int dist = 0;
	uint8_t u = 0, v = 0;
	for (int h = 0; h < yuvFrame->height; h++)
	{
		for (int w = 0; w < yuvFrame->width; w++)
		{
			dist = resource.RowPitch * (yuvFrame->height - 1 - h);

			dptr[dist + w * 4 + 2] = yuvFrame->Y[yuvFrame->pitch * h + w];
			dptr[dist + w * 4 + 1] = yuvFrame->U[yuvFrame->pitch * h + w];
			dptr[dist + w * 4] = yuvFrame->V[yuvFrame->pitch * h + w];
		}
	}

	OutMgr.m_DeviceContext->Unmap(OutMgr.m_SharedSurf, subresource);

	hr = OutMgr.m_DeviceContext->Map(OutMgr.m_SharedSurfY, subresource, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	dptr = reinterpret_cast<BYTE*>(resource.pData);
	OutMgr.m_DeviceContext->Unmap(OutMgr.m_SharedSurfY, subresource);

}

void WriteYUV420ToTexture(YUVFrame *yuvFrame)
{
	// Copy image into CPU access texture
	OutMgr.m_DeviceContext->CopyResource(OutMgr.m_AccessibleSurf, OutMgr.m_SharedSurf);

	// Copy from CPU access texture to bitmap buffer
	D3D11_MAPPED_SUBRESOURCE resource;
	UINT subresource = D3D11CalcSubresource(0, 0, 0);
	OutMgr.m_DeviceContext->Map(OutMgr.m_AccessibleSurf, subresource, D3D11_MAP_WRITE, 0, &resource);

	BYTE* dptr = reinterpret_cast<BYTE*>(resource.pData);

	int dist = 0, dd = yuvFrame->pitch / 2;
	uint8_t u = 0, v = 0;
	for (int h = 0; h < yuvFrame->height; h += 2)
	{
		for (int w = 0; w < yuvFrame->width; w += 2)
		{
			u = yuvFrame->U[yuvFrame->pitch/2 * h / 2 + w / 2];
			v = yuvFrame->V[yuvFrame->pitch/2 * h / 2 + w / 2];

			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					dist = resource.RowPitch * (yuvFrame->height - 1 - (h + i));
					dptr[dist + (w + j) * 4 + 2] = yuvFrame->Y[yuvFrame->pitch * (h + i) + (w + j)];
					dptr[dist + (w + j) * 4 + 1] = u;
					dptr[dist + (w + j) * 4] = v;
				}
			}
		}
	}

	OutMgr.m_DeviceContext->Unmap(OutMgr.m_AccessibleSurf, subresource);
	OutMgr.m_DeviceContext->CopyResource(OutMgr.m_SharedSurf, OutMgr.m_AccessibleSurf);
}