#include <windows.h>
#include <stdint.h>
#include "Win32_Shah.h"
#include "game.cpp"


#define global_variable static 
#define internal static


global_variable Win32_VideoBuffer Globalbuffer;
global_variable int running = 1;




internal Game_State * GameStartup()
{
	
	
	Game_State *GameState = new Game_State;
	
	if(GameState)
	{
		
		GameState->Ship =
			
		{
			{0.0f, 2.5f},   // point1
			{-2.5f, -2.5f},// point2
			{2.5f, -2.5f},// point3
			{0.0f,0.0f},
			{0.0f, 0.0f},
			{0.0f,0.0f},
			{0.0f} //angle 
		};
		
		
		GameState->Asteroids[0]  = {6.0f,80.0f,80.0f};
		GameState->Asteroids[1]  = {6.0f,40.0f,80.0f};
		GameState->Asteroids[2]  = {6.0f,90.0f,50.0f};
		GameState->Asteroids[3]  = {6.0f,60.0f,10.0f};
		GameState->Asteroids[4]  = {6.0f,20.0f,20.0f};
		
		
	}
	
	return GameState;
	
	
}


internal void  Delete_Game_state(Game_State *GameState)
{
	
	
	delete GameState;
	
}


void Win32ResizeDibSection(Win32_VideoBuffer *buffer , int width, int height)
{
	
	
	
	if(buffer->BitmapMemory)
	{
		
		VirtualFree(buffer->BitmapMemory ,buffer->width * buffer->height * 4 ,  MEM_RELEASE);
		
		
	}
	
	buffer->width = width;
	buffer->height = height;
	
	
	
	
	buffer->BitmapInfo ={};
	buffer->BitmapInfo.bmiHeader.biSize = sizeof(buffer->BitmapInfo.bmiHeader);
	buffer->BitmapInfo.bmiHeader.biWidth = buffer->width;  
	buffer->BitmapInfo.bmiHeader.biHeight = buffer->height;
	buffer->BitmapInfo.bmiHeader.biPlanes = 1;
	buffer->BitmapInfo.bmiHeader.biBitCount = 32;
	buffer->BitmapInfo.bmiHeader.biCompression =  BI_RGB; 
	
	int BitMapSize = buffer->width * buffer->height * 4;
	
	buffer->BitmapMemory = VirtualAlloc(NULL ,BitMapSize,MEM_COMMIT | MEM_RESERVE,PAGE_READWRITE);
	
	
	
	
}



LRESULT CALLBACK MainWindowCallBack(
									HWND hwnd,        // handle to window
									UINT uMsg,        // message identifier
									WPARAM wParam,    // first message parameter
									LPARAM lParam)    // second message parameter
{ 
	
	switch (uMsg)
    {
        
		case WM_SIZE:
		{
			
			RECT rect;
			GetClientRect(hwnd , &rect);
			int width =  rect.right - rect.left; 
			int height = rect.bottom - rect.top;
			
			Win32ResizeDibSection(&Globalbuffer,width,height);
			
			
			
		} break;
		
		case WM_DESTROY:
		{
			
		} break;
		
		case WM_CLOSE:
		{
			running = 0;
			
		} break;
		
		case WM_ACTIVATEAPP:
		{
			
			
			
			
		} break;
		
		
		
		
		default:
		{
			
		}
		
    }
	return DefWindowProcA(hwnd,uMsg,wParam,lParam);
}



void RenderColor(uint32_t Color,Win32_VideoBuffer *buffer)
{
	
	
	int pitch = buffer->width;
	uint32_t *pixel = (uint32_t*)buffer->BitmapMemory;
	
	for(int y=0; y < buffer->height; y++)
	{
		
		uint32_t *row = pixel;
		
		
		for(int x=0; x<buffer->width; x++)
		{
			
			*row = Color;
			++row;
			
		}
		
		pixel += pitch;
		
	}
	
}







int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   PSTR lpCmdLine, int nCmdShow)
{
	
	
	
	
	WNDCLASS WindowClass={};
	
	
	
	
	WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowCallBack; 
	WindowClass.hInstance= hInstance;
	WindowClass.lpszClassName = "AsteroidsWindowClass";
	
	
	LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    s64 PerfCountFrequency = PerfCountFrequencyResult.QuadPart;
	
	UINT DesiredSchedulerMS = 1;
	b32 SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);
	
	
	
	
	if(RegisterClassA(&WindowClass))
		
		
		
	{
		
		
		HWND Window = CreateWindowExA(0,
                                      WindowClass.lpszClassName,
                                      "Asteroids",
                                      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      1280,
                                      720,
                                      0,
									  0,
                                      hInstance,
                                      0);
		
		
		
		
		if(Window)
		{
			
			
			
			int MonitorRefreshRate = 60;
			HDC RefreshDC = GetDC(Window);
			int Win32RefreshRate = GetDeviceCaps(RefreshDC , VREFRESH);
			if(Win32RefreshRate > 1)
			{
				
				MonitorRefreshRate = Win32RefreshRate;
				
			}
			ReleaseDC(Window, RefreshDC);
			
			int GameUpdateHz = MonitorRefreshRate;
			
			float TargetSecondsPerFrame = 1.0f/(float)GameUpdateHz;
			
			
			Game_State *GameState=GameStartup();
			
			
			Game_Input Input[2] ={};
			Game_Input *OldInput =&Input[0];
			Game_Input *NewInput =&Input[1];
			
			
			VideoBuffer GameBuffer = {};
			
			//Win32ResizeDibSection(&Globalbuffer,1280, 720);
			LARGE_INTEGER LastCounter; //counts per frame
			
			
			QueryPerformanceCounter(&LastCounter);
			while(running)
			{
				
				NewInput->DeltaTime = TargetSecondsPerFrame;
				
				GameBuffer.width = Globalbuffer.width;
				GameBuffer.height = Globalbuffer.height;
				GameBuffer.BitmapMemory = Globalbuffer.BitmapMemory;
				
				
				//input
				
				
				
				
				enum button btn = BUTTONUNKNOWN;
				
				DeviceInput *OldKeyboardInput =&OldInput->GameController;
				DeviceInput *NewKeyboardInput =&NewInput->GameController;
				
				//DeviceInput ZeroController={};
				
				*NewKeyboardInput = {};
				
				
				for (int ButtonIndex = 0; ButtonIndex < BUTTONCOUNT; ++ButtonIndex)
				{
					
					
					NewKeyboardInput->Move[ButtonIndex].EndedDown=
						OldKeyboardInput->Move[ButtonIndex].EndedDown;    
					
					
					
				}
				
				
				
				
				
#if 0
				switch (btn) {
					case BUTTONUP:
					{
						
						NewKeyboardInput->Move[BUTTONUP].EndedDown=
							OldKeyboardInput->Move[BUTTONUP].EndedDown;    
						
					}
					
					
					case BUTTONDOWN:
					{
						NewKeyboardInput->Move[BUTTONDOWN].EndedDown=
							OldKeyboardInput->Move[BUTTONDOWN].EndedDown; 
						//printf("Button is Down.\n");
					}
					
					
					case BUTTONLEFT:
					{
						NewKeyboardInput->Move[BUTTONLEFT].EndedDown=
							OldKeyboardInput->Move[BUTTONLEFT].EndedDown; 
						
						//printf("Button is Left.\n");
						
					}
					
					case BUTTONRIGHT:
					{
						NewKeyboardInput->Move[BUTTONRIGHT].EndedDown=
							OldKeyboardInput->Move[BUTTONRIGHT].EndedDown; 
						//printf("Button is Right.\n");
					}
					break;
					
					default:
					// No action is required for BUTTONUNKNOWN or unexpected values
					// You can choose to log, ignore, or handle this case as needed
					break;
				}
				
#endif
				
				
				
				
				MSG message;
				while(PeekMessageA(&message,Window,0,0,PM_REMOVE))
				{
					
					switch(message.message){
						
						
						case WM_KEYDOWN:
						case WM_KEYUP:
						case WM_SYSKEYDOWN:
						case WM_SYSKEYUP:
						{
							u32 VkCode =  (u32)message.wParam;
							b32 WasDown = ((message.lParam & (1<<30)) != 0);
							b32 IsDown = ((message.lParam & (1<<31)) ==0);
							
							if(WasDown != IsDown){
								if(VkCode == VK_LEFT)
								{
									btn = BUTTONLEFT;
									NewKeyboardInput->Move[BUTTONLEFT].EndedDown = IsDown;
									++NewKeyboardInput->Move[BUTTONLEFT].HalfTransitionCount;
									
									
								}
								if(VkCode == VK_UP)
								{
									
									
									btn = BUTTONUP;
									NewKeyboardInput->Move[BUTTONUP].EndedDown = IsDown;
									++NewKeyboardInput->Move[BUTTONUP].HalfTransitionCount;
									
									//OutputDebugStringA("up key was pressed \n");
									
								}
								if(VkCode == VK_DOWN) 
								{
									
									btn = BUTTONLEFT;
									NewKeyboardInput->Move[BUTTONDOWN].EndedDown = IsDown;
									++NewKeyboardInput->Move[BUTTONDOWN].HalfTransitionCount;
									
									
									
								}
								if(VkCode == VK_RIGHT)
								{
									
									
									btn = BUTTONRIGHT;
									NewKeyboardInput->Move[BUTTONRIGHT].EndedDown = IsDown;
									++NewKeyboardInput->Move[BUTTONRIGHT].HalfTransitionCount;
									
									
								}
								
							}
							
							
							
							
						}break;
						default:
						{
							TranslateMessage(&message); 
							DispatchMessage(&message); 
						}
						
						
					}
				}
				
				// game_update and render //
				
				
				HDC DC = GetDC(Window);
				RenderColor(0x00000000,&Globalbuffer);
				//RenderRect(0xFFFBBFAA,&GameBuffer,400,470,100,200);
				GameUpdateAndRender(NewInput,&GameBuffer,GameState);
				
				StretchDIBits(DC,0,0,Globalbuffer.width,Globalbuffer.height,0,0,Globalbuffer.width,Globalbuffer.height,Globalbuffer.BitmapMemory,&Globalbuffer.BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
				
				
				LARGE_INTEGER EndCounter;
				QueryPerformanceCounter(&EndCounter);
				s64 CountersPerFrame = EndCounter.QuadPart - LastCounter.QuadPart;
				s32 MSPerFrame = (s32)((1000*CountersPerFrame) / PerfCountFrequency);
				s32 FPS = (s32)(PerfCountFrequency / CountersPerFrame);
				
				float SecondsElapsedForWork = (float)CountersPerFrame / (float)PerfCountFrequency;
				while(SecondsElapsedForWork < TargetSecondsPerFrame)
				{
					
					if(SleepIsGranular)
					{
						
						DWORD MSToSleep=DWORD(1000 * (TargetSecondsPerFrame - SecondsElapsedForWork));
						if(MSToSleep > 0)
						{
							
							Sleep(MSToSleep);
							
							
						}
						
						
					}
					
					QueryPerformanceCounter(&EndCounter);
					CountersPerFrame = EndCounter.QuadPart - LastCounter.QuadPart; 
					SecondsElapsedForWork = (float)CountersPerFrame / (float)PerfCountFrequency;
					
					
				}
				
				
				
				char Buffer[256];
				wsprintfA(Buffer, "ms/frame: %dms / %dFPS\n", MSPerFrame, FPS);
				OutputDebugStringA(Buffer);
				
				
				LastCounter = EndCounter;
				
				Game_Input *Temp;
				Temp = NewInput;
				NewInput = OldInput;
				OldInput = Temp;
				
				
			}
			
			//Delete_Game_state(GameState);
			
		}
		
		
		
		
		else
		{
			
			
		}
		
		
	}
	
	
	
	else
	{
		
		
	}
	
	
    return 0;
	
}

