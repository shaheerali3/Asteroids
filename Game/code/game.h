/* date = August 10th 2024 1:58 pm */

#ifndef GAME_H
#define GAME_H


typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;


typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef s32 b32;


struct Vector2i
{
	
	int x;
	int y;
	
	
};



struct Vector2f
{
	
	float x;
	float y;
	
	
};

struct Box
{
	
	float left;
	float right;
	float top;
	float bottom;
	
};



struct Circle
{
	
	float radius;
	Vector2f CenterPoint;
	
};


struct PlayerSpaceShip
{
	
	Vector2f point1;
	Vector2f point2;
	Vector2f point3;
	
	Vector2f Centroid;
	Vector2f Velocity;
	Vector2f Acceleration;
	float angle;
	
	
	//b32 isinitilized;
};




struct Game_State
{
	
	PlayerSpaceShip Ship;
	Circle Asteroids[5];
	
	
};




struct VideoBuffer
{
	int width;
	int height;
	void *BitmapMemory;
	
};


struct ButtonState
{
	
	int HalfTransitionCount;
	b32 EndedDown;
	
	
};
enum button 
{
	
	BUTTONUP,
	BUTTONDOWN,
	BUTTONLEFT,
	BUTTONRIGHT,
	
	BUTTONCOUNT,
	BUTTONUNKNOWN
		
};



struct DeviceInput
{
	
	ButtonState Move[BUTTONCOUNT];
	
};


struct Game_Input 
{
	
	DeviceInput GameController;
	float DeltaTime;
	
};






#endif //GAME_H
