#include<stdio.h>
#include "Game.h"
#include <cmath>
#define _USE_MATH_DEFINES
#define SCREENWIDTH 1280
#define SCREENHEIGHT 720
#define LOGICALWIDTH 200
#define LOGICALHEIGHT 200 
#define MAXSPEED 1.0f


float remangle = 0.785f;
float playerx = 0;
float playery = 0;

//static float angle2=0;
//float angle = 0;
//PlayerSpaceShip Ship;
//int rotate[2][2] =[cos(angle),sin(angle)sin(angle),cose(angle)]

float Min(float a,float b, float c)
{
	float TempMin = (a<b) ? a:b;
	return (TempMin<c) ? TempMin:c;
	
}


float Max(float a,float b, float c)
{
	float TempMin = (a>b) ? a:b;
	return (TempMin>c) ? TempMin:c;
	
}



Vector2f CalcCentroid(Vector2f point1,Vector2f point2,Vector2f point3)
{
	
	Vector2f Result={};
	
	Result.x = (point1.x + point2.x + point3.x) / 3.0f;
	Result.y = (point1.y + point2.y + point3.y) / 3.0f;
	
	return Result;
	
}


Vector2f CalcSizeRelativeToCentroid(Vector2f point,Vector2f Centroid)
{
	Vector2f Result={};
	
	Result.x = point.x - Centroid.x;
	Result.y = point.y - Centroid.y;
	
	return Result;
	
}


float degreesToRadians(float degrees) {
	
    return degrees * (3.14159f / 180.0f);
}

float radians_to_degrees(float radians) {
    return radians * float(180.0f /3.14159);
}



void Createrotationmatrix(float angle ,float *array)
{
	
	
	array[0] = cos(angle);
	array[1] = -sin(angle);
	array[2] = sin(angle);
	array[3] = cos(angle);
	
	
	
}

void matrixmul(float *matrix ,int width,int height,float *matrix2,int width2 ,int height2, float *result)
{
	
	if(height !=width2)
	{
		
		
		return;
		
	}
	
	
	for(int i=0; i<height; ++i)
	{
		for(int j=0; j<height2; ++j)
		{
			//result[i * width2 + j] =  0;
			
			for(int k=0; k<width; ++k)
			{
				
				result[(j  * width2) + i] += ( matrix[(k * width) + i] * matrix2[(j * width2 ) + k] );
				
				
			}
		}
		
	}
	
	
}









void PlotPixel( VideoBuffer *buffer, int x,int y,u32 Color)
{
	
	bool isPixel = true;
	
#if 0
	
	if (x >= buffer->width)
    {
        x = x % buffer->width;
    }
    else if (x < 0)
    {
        x = (x % buffer->width + buffer->width) % buffer->width;
    }
	
	if (y >= buffer->height)
    {
        y = y % buffer->height;
    }
    else if (y < 0)
    {
        y = (y % buffer->height + buffer->height) % buffer->height;
    }
	
#endif
	
	
	if (x >= buffer->width)
    {
		isPixel=false;
    }
    else if (x < 0)
    {
		isPixel=false;
		
    }
	
	if (y >= buffer->height)
    {
		
		isPixel=false;
    }
    else if (y < 0)
    {
		isPixel=false;
    }
	
	
	
	
	if (isPixel)
	{
		u32 *pixel = (u32*)buffer->BitmapMemory + x + y * buffer->width;
		*pixel=Color;
	}
	
}
Vector2f GetLogicalWidthAndHeight(VideoBuffer *buffer)
{
	
	
	Vector2f LogicalCordinateSpace = {};
	
	float logicalHeight = 200.0f ;
	float logicalWidth = 200.0f; // aspectRatio;
	
    // Calculate the scaling factors as pixels per logical unit
	
    float scaleX = buffer->width / logicalWidth;
	// Pixels per logical unit along the x-axis
	
    float scaleY = buffer->height / logicalHeight;  // Pixels per logical unit along the y-axis
	
    // Use the smaller of scaleX and scaleY to ensure uniform scaling (optional, prevents distortion)
	
    float scale = (scaleX < scaleY) ? scaleX : scaleY;
	
	LogicalCordinateSpace.x = buffer->width/scale;
	LogicalCordinateSpace.y = buffer->height/scale;
	
	return LogicalCordinateSpace;
}


void MapToPixel(VideoBuffer *buffer, float LogicalX,float LogicalY,int *PixelX,int  *PixelY)
{
	
	
	//float aspectRatio = (float) (buffer->width /(buffer->height)) ;
	
    float logicalHeight = 200.0f ;
	float logicalWidth = 200.0f; // aspectRatio;
	
    // Calculate the scaling factors as pixels per logical unit
	
    float scaleX = buffer->width / logicalWidth;
	// Pixels per logical unit along the x-axis
	
    float scaleY = buffer->height / logicalHeight;  // Pixels per logical unit along the y-axis
	
    // Use the smaller of scaleX and scaleY to ensure uniform scaling (optional, prevents distortion)
    float scale = (scaleX < scaleY) ? scaleX : scaleY;
	
    // Calculate the offsets to center the logical space in the screen
    float offsetX = (buffer->width - logicalWidth * scale) / 2.0f;
    float offsetY = (buffer->height - logicalHeight * scale) / 2.0f;
	
    // Map logical coordinates to pixel coordinates using the scaling factors
    *PixelX = (int)(offsetX + (LogicalX + 100.0f) * scale);  // X-axis mapping
	
    // Remove inversion of the y-axis: Map Y-axis normally, so positive values are upwards
    *PixelY = (int)(offsetY + (LogicalY + 100.0f) * scale);  // Y-axis mapping
}


Vector2f MapDistanceToPixel(VideoBuffer *buffer, float distance)
{
	
    int x0, y0;
	
    // Map (0.0f, 0.0f) to pixel coordinates
    MapToPixel(buffer, 0.0f, 0.0f, &x0, &y0);
	
    // Map (distance, 0.0f) to get x-direction scaling
    int x1, y1;
    MapToPixel(buffer, distance, 0.0f, &x1, &y1);
	
    // Map (0.0f, distance) to get y-direction scaling
    int x2, y2;
    MapToPixel(buffer, 0.0f, distance, &x2, &y2);
	
    // Calculate pixel distances in x and y directions
    float pixelDistanceX = float(x1 - x0);
    float pixelDistanceY = float(y2 - y0);
	
    // Return as a vector
    return Vector2f{ pixelDistanceX, pixelDistanceY};
	
}

void Drawcircle(VideoBuffer *buffer, Circle Asteroid)
{
	
	
	int Pixelx , Pixely;
	Vector2f CirclePoint = {};
	Vector2f LogicalCordinates = GetLogicalWidthAndHeight(buffer);
	
	for(float i = 0.0f; i < (2.0f *3.14159f); i+=0.001f)
	{
		
		
		CirclePoint.x = (Asteroid.CenterPoint.x + (Asteroid.radius * cosf(i)));
		CirclePoint.y = (Asteroid.CenterPoint.y + (Asteroid.radius * sinf(i)));
		
		
#if 0
		if (CirclePoint.x < - ( (LogicalCordinates.x ) / 2 ))
		{
			
			CirclePoint.x += (LogicalCordinates.x);
			
		}
		else if(CirclePoint.x >= (( LogicalCordinates.x )/2 ))
		{
			
			CirclePoint.x -=  ( LogicalCordinates.x ); 
			
		}
		
		if( CirclePoint.y < -LogicalCordinates.y / 2)
		{
			
			CirclePoint.y += LogicalCordinates.y;
			
		}
		else if(CirclePoint.y > LogicalCordinates.y / 2)
		{
			
			CirclePoint.y -= LogicalCordinates.y; 
			
		}
		
#endif
		
		
		
		if(!((CirclePoint.y < -LogicalCordinates.y / 2) || (CirclePoint.y > LogicalCordinates.y/2)||(CirclePoint.x > LogicalCordinates.x / 2)||  (CirclePoint.x < -LogicalCordinates.x / 2)))  
		{
			MapToPixel(buffer,CirclePoint.x,CirclePoint.y,&Pixelx,&Pixely);
			PlotPixel(buffer, Pixelx , Pixely,0xFFFFFFFF);
		}
		
	}
	
	
	
	
	
	
	
	//Vector2f PixelRadius = MapDistanceToPixel(buffer,Asteroid.radius);
	
    // Compute pixel radii for x and y
	
	
	
	
	
	//float stepSize = 1.0f / Asteroid.radius;
	//Asteroid.radius = PixelRadius;
#if 0
	for(float i = 0.0f; i < (2.0f *3.14159f); i+=0.001f)
	{
		
		
		
		CirclePoint.x = (Pixelx + (PixelRadius.x * cosf(i)));
		CirclePoint.y = (Pixely + (PixelRadius.y * sinf(i)));
		
		
		
		
	}
#endif
	
	
}



void BresenhamLine(VideoBuffer *buffer ,float x1, float y1 ,float x2,float y2)
{
	
	
	int pixelx1=0;
	int pixely1=0;
	int pixelx2=0;
	int pixely2=0;
	
	
	MapToPixel(buffer,x1,y1,&pixelx1,&pixely1);
	MapToPixel(buffer,x2,y2,&pixelx2,&pixely2);
	
	int dx = abs(pixelx2 - pixelx1);
	int dy = abs(pixely2 - pixely1);
	
	
	int sx = (pixelx1 < pixelx2) ? 1 : -1;  // Step direction in x
    int sy = (pixely1 < pixely2) ? 1 : -1;  // Step in direction y 
	
	
	
	int error = dx - dy;
	
	while(1)
	{
		
		
		PlotPixel(buffer,pixelx1,pixely1,0x0000FF00F);
		
		
		
		
		if (pixelx1 == pixelx2 && pixely1 == pixely2)
		{
			
            break;
			
			
		}
		
		int err = 2 * error;
		
		if(err > -dy)
		{
			
			error -=dy;
			pixelx1 += sx; 
			
		}
		if(err < dx) 
		{
			
			error+=dx;
			pixely1+=sy;
			
		}
		
		
		
	}
	
	
}


int clamp(int value,int min,int max)
{
	if(value<min)
	{
		return min;
	}
	if(value>max)
	{
		return max;
	}
	
	return value; 
}




void RenderRectInPixels(u32 Color,VideoBuffer *buffer,int y0, int y1 , int x0 ,int x1)
{
	
	
	x0=clamp(x0,0,buffer->width);
	x1=clamp(x1,0,buffer->width);
	y0=clamp(y0,0,buffer->height);
	y1=clamp(y1,0,buffer->height);
	
	int SquareWidth = x1 - x0;
	int Squareheight = y1 -  y0;
	u32 *row = (u32*)(buffer->BitmapMemory);
	row +=  (y0*buffer->width) + x0;
	
	for(int y=0; y<Squareheight; y++)
	{
		
		u32 *pixel = row;
		for(int x = 0; x<SquareWidth; x++)
		{
			
			
			*pixel++ = Color;
			
			
			
		}
		
		row += buffer->width;
		
	}
	
	
}






void DrawBox(VideoBuffer *buffer ,float x,float y,float x2 ,float y2,u32 Color)
{
#if 0
	
	int PixelX , PixelY , PixelX2 , PixelY2;
	MapToPixel(buffer, x ,y, &PixelX , &PixelY);
	MapToPixel(buffer,x2,y2, &PixelX2,&PixelY2);
	
	for(int top = PixelY; top > PixelY2; top--)
	{
		
		for(int left = PixelX; left < PixelX2; left++)
		{
			
			PlotPixel(buffer,left ,top ,Color);
			left=PixelX2-1;
			
		}
		
		
	}
#endif
	
	BresenhamLine(buffer , x ,y , x,y2);
	BresenhamLine(buffer , x ,y , x2,y);
	BresenhamLine(buffer , x2 ,y , x2,y2);
	BresenhamLine(buffer , x2 ,y2 , x,y2);
	
}




void drawline(VideoBuffer *buffer, float x1 , float x2 ,float y1, float y2,u32 Color)
{
	// two points on a line; (x,y)        (x1,y1)
	// y-y1=m(x-x1)
	// (y-y1/x-x1)=m
	//map to gamecoordinates not pixel coordinates
	
	//map pixel to coordinates.
	//
	float tempx;
	float tempy;
	if(y1>y2)
	{
		tempy=y2;
		y2=y1;
		y1=tempy;
		
		//swap points 
		tempx=x2;
		x2=x1;
		x1=tempx;
		
		
		
	}
	
	
	int PixelX1;
	int PixelY1;
	
	int PixelX2;
	int PixelY2;
	
	MapToPixel(buffer,x1,y1,&PixelX1,&PixelY1);
	MapToPixel(buffer,x2,y2,&PixelX2,&PixelY2);
	
	
	int dx = PixelX2 - PixelX1;
	int dy = PixelY2 - PixelY1;
	
	float m =(float)((PixelY2-PixelY1)/(PixelX2-PixelX1));
	
	float c = PixelY1 - (m * PixelX1);
	
	
	
	//for(int pointy=0; pointy<(y2-y1);pointy++){
	
	if(m==0.0f)
	{
		
		int y = (int)PixelY1;
		int xoffset=(int)PixelX1;// Constant y-coordinate for horizontal line
		u32 *pixel = (u32*)(buffer->BitmapMemory) + y * buffer->width + xoffset;
		for (int x = (int)PixelX1; x < (int)PixelX2; x++)
		{
			
			*pixel++ = Color;
		}
		
		
		
		
		
	}
	
	
	
	else{
		
		
		
		if(dx==0)
		{
			
			int x = (int)PixelX1; // Constant x-coordinate for vertical line
			for (int y = (int)PixelY1; y < (int)PixelY2; y++)
			{
				u32 *pixel = (u32*)(buffer->BitmapMemory) + y * buffer->width + x;
				*pixel = Color;
			}
			
			
		}
		
		
		else
		{
			for(int pointx=(int)(PixelX1); pointx<(int)(PixelX2); pointx++)
			{
				
				
				int pointy = (int) (m*pointx + c);
				//u32 *pixel = (u32*)(buffer->BitmapMemory) + pointy*buffer->width + pointx;
				u32 *pixel = (u32*)(buffer->BitmapMemory);
				pixel +=  (pointy * buffer->width) + pointx;
				*pixel = Color;
				
			}
			
			for(int pointy=(int)(PixelY1); pointy<(int)(PixelY2); pointy++)
			{
				
				int pointx = (int)((pointy - c) / m);
				//u32 *pixel = (u32*)(buffer->BitmapMemory) + pointy*buffer->width + pointx;
				u32 *pixel = (u32*)(buffer->BitmapMemory);
				pixel +=  (pointy*buffer->width) + pointx;
				*pixel = Color;
				
				
			}
			
			
		}
		
		
	}
	//}
	
}







//[RRGGBBAA,RRGGBBAA,RRGGBBAA,RRGGBBAA,RRGGBAA]

void WarpPoints(VideoBuffer *buffer,   Vector2f *Point)// TODO(add centerpoint): 
{
	
	//float aspectratio = float((buffer->width)/(buffer->height));
	
	// once 
	
	
	
    float logicalHeight = 200.0f ;
	float logicalWidth = 200.0f;
	
    // Calculate the scaling factors as pixels per logical unit
	
    float scaleX = buffer->width / logicalWidth;
	// Pixels per logical unit along the x-axis
	
    float scaleY = buffer->height / logicalHeight;  // Pixels per logical unit along the y-axis
	
    // Use the smaller of scaleX and scaleY to ensure uniform scaling (optional, prevents distortion)
    float scale = (scaleX < scaleY) ? scaleX : scaleY;
	float NewLogicalWidth = buffer->width / scale;
	
	if (Point->x < - ( (NewLogicalWidth ) / 2 ))
	{
		
		Point->x += (NewLogicalWidth);
		
	}
	if(Point->x >= ((NewLogicalWidth )/2 ))
	{
		
		Point->x -=  ( NewLogicalWidth ); 
		
	}
	
	if(Point->y < -LOGICALWIDTH / 2)
	{
		
		Point->y += LOGICALHEIGHT;
		
	}
	if(Point->y > LOGICALWIDTH / 2)
	{
		
		Point->y -= LOGICALHEIGHT; 
		
	}
	
	
}

void DrawTriangle(VideoBuffer *buffer,PlayerSpaceShip *Ship)
{
	
	
	
	BresenhamLine(buffer,Ship->point1.x,Ship->point1.y,Ship->point2.x,Ship->point2.y);
	BresenhamLine(buffer,Ship->point2.x,Ship->point2.y,Ship->point3.x,Ship->point3.y);
	BresenhamLine(buffer,Ship->point1.x,Ship->point1.y,Ship->point3.x,Ship->point3.y);
	
	
}

void CreateShipCollider(PlayerSpaceShip Ship , Box *ShipCollider)
{
	
	
	ShipCollider->left = Min(Ship.point1.x,Ship.point2.x,Ship.point3.x);
	
	ShipCollider->right = Max(Ship.point1.x,Ship.point2.x,Ship.point3.x);
	
	ShipCollider->top = Max(Ship.point1.y,Ship.point2.y,Ship.point3.y);
	
	ShipCollider->bottom = Min(Ship.point1.y,Ship.point2.y,Ship.point3.y);
	
	
}


void CreateAsteroidCollider(VideoBuffer *buffer,Circle Asteroid, Box *CircleCollider)
{
	
	// left ,top 
	// left ,bottom
	// right , top
	// right ,bottom
	
	Vector2f LogicalSpaces = GetLogicalWidthAndHeight(buffer);
	
	CircleCollider->left = Asteroid.CenterPoint.x - Asteroid.radius;
	
	CircleCollider->right = Asteroid.CenterPoint.x + Asteroid. radius;
	
	CircleCollider->top = Asteroid.CenterPoint.y + Asteroid.radius;
	
	CircleCollider->bottom = Asteroid.CenterPoint.y - Asteroid.radius;
	
	
	Vector2f topLeft = {CircleCollider->left, CircleCollider->top};
    Vector2f bottomLeft = {CircleCollider->left, CircleCollider->bottom};
    Vector2f topRight = {CircleCollider->right, CircleCollider->top};
    Vector2f bottomRight = {CircleCollider->right, CircleCollider->bottom};
	
    // Warp each point
	if(topLeft.y > (LogicalSpaces.y/2) && (topRight.y > LogicalSpaces.y/2) && bottomLeft.y >  LogicalSpaces.y/2 && bottomRight.y > LogicalSpaces.y/2 )
	{
		WarpPoints(buffer, &topLeft);
		WarpPoints(buffer, &topRight);
		WarpPoints(buffer, &bottomLeft);
		WarpPoints(buffer, &bottomRight);
	}
    // Update the collider with the warped points
    CircleCollider->left = topLeft.x;
    CircleCollider->right = topRight.x;
    CircleCollider->top = topLeft.y;
    CircleCollider->bottom = bottomLeft.y;
	
	
}

int CheckAABBCollisions(VideoBuffer *buffer ,Box ShipCollider , Box CircleCollider)
{
	
	
	if(ShipCollider.left < CircleCollider.right && ShipCollider.right > CircleCollider.left && ShipCollider.top > CircleCollider.bottom && 
	   ShipCollider.bottom < CircleCollider.top)
	{
		
		
		return true;
		
	}
	else
	{
		
		
		DrawBox(buffer,CircleCollider.left,CircleCollider.top,CircleCollider.right,CircleCollider.bottom,0x00FF);
		
	}
	
	return false;
}

void WarpCircle(Vector2f LogicSpace , Circle *Asteroid)
{
	
	
	
	if(Asteroid->CenterPoint.y > LogicSpace.y / 2  )
	{
		
		Asteroid->CenterPoint.y -= LogicSpace.y; 
		
	}
	if(Asteroid->CenterPoint.x > LogicSpace.x / 2  )
	{
		
		Asteroid->CenterPoint.x -= LogicSpace.x; 
		
	}
	
	
}







void GameUpdateAndRender(Game_Input *Input ,VideoBuffer *buffer,Game_State *GameMemory)
{
	
	
	// TODO(rememeber to pass in deltatime):
	Vector2f LogicalCordinates = GetLogicalWidthAndHeight(buffer);
	
	
#if 0
	
	char buffer2[256];
	sprintf_s(buffer2, "point1 , point2,  (%f,%f)\n", GameMemory->Ship.point1.x, GameMemory->Ship.point1.y );
	OutputDebugStringA(buffer2);
	
#endif
	GameMemory->Ship.angle = 0;
	
	GameMemory->Ship.Centroid = CalcCentroid(GameMemory->Ship.point1,GameMemory->Ship.point2,GameMemory->Ship.point3);
	//Vector2f acceleration = CalcAcceleration(GameMemory->Ship.point1,GameMemory->Ship.Centroid);
	
	float dx = GameMemory->Ship.point1.x - GameMemory->Ship.Centroid.x;
    float dy = GameMemory->Ship.point1.y - GameMemory->Ship.Centroid.y;
	
	float anglefront = atan2(dy, dx);
	
	
	
	
	float magnitude = sqrt((GameMemory->Ship.Velocity.x*GameMemory->Ship.Velocity.x) + (GameMemory->Ship.Velocity.y *GameMemory->Ship.Velocity.y));
	
	
	if(magnitude > MAXSPEED)
	{
		float Scale = MAXSPEED/magnitude;
		GameMemory->Ship.Velocity.x *= Scale;
		GameMemory->Ship.Velocity.y *= Scale;
		
	}
	
	
	Box CircleCollider[5] = {};
	int NumberOfAsteroids = 5;
	
	for(int i =0; i<NumberOfAsteroids; i++){
		
		GameMemory->Asteroids[i].CenterPoint.y += 0.6f;
		GameMemory->Asteroids[i].CenterPoint.x += 0.6f;
		
		WarpCircle(LogicalCordinates,&GameMemory->Asteroids[i]);
	}
	
	
	//float PixelRadius = MapDistanceToPixel(buffer , radius);
	// TODO(should i randomize the asteroid positions think about this)
	
	
	
	for(int i = 0; i < NumberOfAsteroids; i++)
		
	{
		
		
		CreateAsteroidCollider(buffer,GameMemory->Asteroids[i]  , &CircleCollider[i]);
		Drawcircle(buffer,GameMemory->Asteroids[i]);
		
	}
	
	
	
	Box ShipCollider = {};
	CreateShipCollider(GameMemory->Ship , &ShipCollider);
	
	//CreateAsteroidCollider(Asteroid , &CircleCollider);
	
	//Circle Asteroid = {radius,centerx,centery};
	
	//Drawcircle(buffer,Asteroid);
	
	
	
	DrawBox(buffer,ShipCollider.left,ShipCollider.top,ShipCollider.right,ShipCollider.bottom,0x00FF);
	
	
	for(int i= 0; i<NumberOfAsteroids; i++)
	{
		
		
		CheckAABBCollisions(buffer,ShipCollider,CircleCollider[i]);
		
		
		
	}
	
	
	GameMemory->Ship.Velocity.x = GameMemory->Ship.Velocity.x + (GameMemory->Ship.Acceleration.x * Input->DeltaTime);
	
	GameMemory->Ship.Velocity.y = GameMemory->Ship.Velocity.y + (GameMemory->Ship.Acceleration.y * Input->DeltaTime);
	
	
	GameMemory->Ship.point1.x += GameMemory->Ship.Velocity.x;
	GameMemory->Ship.point1.y += GameMemory->Ship.Velocity.y;
	
	GameMemory->Ship.point2.x += GameMemory->Ship.Velocity.x;
	GameMemory->Ship.point2.y += GameMemory->Ship.Velocity.y;
	
	GameMemory->Ship.point3.x += GameMemory->Ship.Velocity.x;
	GameMemory->Ship.point3.y += GameMemory->Ship.Velocity.y;
	
	
	
	GameMemory->Ship.Acceleration.x=0.0f;
	GameMemory->Ship.Acceleration.y=0.0f;
	
	
	//GameMemory->Ship.Velocity.x = 0.0f;
	//GameMemory->Ship.Velocity.y = 0.0f;
	
	
	
	if(Input->GameController.Move[BUTTONLEFT].EndedDown)
	{
		
		
		GameMemory->Ship.angle -=  5.0f;
		
		
		
	}
	
	
	
	
	if(Input->GameController.Move[BUTTONUP].EndedDown)
	{
		
		GameMemory->Ship.Acceleration.x += 1.0f * cosf(anglefront);
		GameMemory->Ship.Acceleration.y += 1.0f * sinf(anglefront);
		
	}
	
	
	if(Input->GameController.Move[BUTTONRIGHT].EndedDown)
	{
		
		GameMemory->Ship.angle +=  5.0f;
		
	}
	
	if(Input->GameController.Move[BUTTONDOWN].EndedDown)
	{
		
		
		
	}
	
	//sprintf("");
	float angle2 = degreesToRadians(GameMemory->Ship.angle);
	
	//remangle += angle2;
	
	int RotateMatrixWidth = 2;
	int RotateMatrixHeight = 2;
	
	
	float Rotatematrix[4] = {
		
		cosf(angle2), -sinf(angle2),
		sinf(angle2), cosf(angle2)
	};
	
	
	//Createrotationmatrix(angle1 , Rotatematrix);
	
	Vector2f point1 = CalcSizeRelativeToCentroid(GameMemory->Ship.point1,GameMemory->Ship.Centroid);
	Vector2f point2 = CalcSizeRelativeToCentroid(GameMemory->Ship.point2,GameMemory->Ship.Centroid);
	Vector2f point3 = CalcSizeRelativeToCentroid(GameMemory->Ship.point3,GameMemory->Ship.Centroid);
	
	
	float translatedPoints[6] = {0};
	
	translatedPoints[0] = point1.x;
	translatedPoints[1] = point1.y; 
	translatedPoints[2] = point2.x;
	translatedPoints[3] = point2.y;
	translatedPoints[4] = point3.x;
	translatedPoints[5] = point3.y;
	
	float result[6]={0};
	matrixmul(Rotatematrix,RotateMatrixWidth,RotateMatrixHeight,translatedPoints,2,3,result);
	
	
	/*
	char buffer3[256];
	sprintf_s(buffer3, "matrix: (%f, %f,%f,%f,%f,%f)\n", result[0], result[1],result[2],result[3],result[4],result[5]);
	OutputDebugStringA(buffer3);
	*/
	
	//convert points back to relative to the origin
	
	
	GameMemory->Ship.point1.x = (result[0] + GameMemory->Ship.Centroid.x);
	GameMemory->Ship.point1.y = (result[1] + GameMemory->Ship.Centroid.y);
	GameMemory->Ship.point2.x = (result[2] + GameMemory->Ship.Centroid.x);
	GameMemory->Ship.point2.y = (result[3] + GameMemory->Ship.Centroid.y);
	GameMemory->Ship.point3.x = (result[4] + GameMemory->Ship.Centroid.x);
	GameMemory->Ship.point3.y = (result[5] + GameMemory->Ship.Centroid.y);
	
	
	
	
#if 0
	
	char buffer2[256];
	sprintf_s(buffer2, "velocityx , velocityy, DeltaTime (%f,%f,%f)\n", GameMemory->Ship.Velocity.x, GameMemory->Ship.Velocity.y, Input->DeltaTime);
	OutputDebugStringA(buffer2);
	
	
	
#endif
	
	
	if(GameMemory->Ship.point1.y >= LogicalCordinates.y/2 &&
	   GameMemory->Ship.point1.x >= LogicalCordinates.x/2 || 
	   GameMemory->Ship.point2.x >= LogicalCordinates.x/2 &&
	   GameMemory->Ship.point2.y >= LogicalCordinates.y/2 || 
	   GameMemory->Ship.point3.x >= LogicalCordinates.x/2 &&
	   GameMemory->Ship.point3.y >= LogicalCordinates.y/2   ) 
		
	{
		
		GameMemory->Ship.point1.x -= LogicalCordinates.x;
		GameMemory->Ship.point1.y -= LogicalCordinates.y;
		
		
		GameMemory->Ship.point2.x -= LogicalCordinates.x;
		GameMemory->Ship.point2.y -= LogicalCordinates.y;
		
		
		GameMemory->Ship.point3.y -= LogicalCordinates.y;
		GameMemory->Ship.point3.x -= LogicalCordinates.x;
		
		
	}
	
	
	if(GameMemory->Ship.point1.y <= -LogicalCordinates.y/2 &&
	   GameMemory->Ship.point1.x >= LogicalCordinates.x/2 || 
	   GameMemory->Ship.point2.x >= LogicalCordinates.x/2 &&
	   GameMemory->Ship.point2.y <= -LogicalCordinates.y/2 || 
	   GameMemory->Ship.point3.x >= LogicalCordinates.x/2 &&
	   GameMemory->Ship.point3.y <= -LogicalCordinates.y/2   ) 
		
	{
		GameMemory->Ship.point1.x -= LogicalCordinates.x;
		GameMemory->Ship.point1.y += LogicalCordinates.y;
		
		GameMemory->Ship.point2.x -= LogicalCordinates.x;
		GameMemory->Ship.point2.y += LogicalCordinates.y;
		
		GameMemory->Ship.point3.y += LogicalCordinates.y;
		GameMemory->Ship.point3.x -= LogicalCordinates.x;
	}
	
	
	
	if(GameMemory->Ship.point1.y <= -LogicalCordinates.y/2 &&
	   GameMemory->Ship.point1.x <= -LogicalCordinates.x/2 || 
	   GameMemory->Ship.point2.x <= -LogicalCordinates.x/2 &&
	   GameMemory->Ship.point2.y <= -LogicalCordinates.y/2 || 
	   GameMemory->Ship.point3.x <= -LogicalCordinates.x/2 &&
	   GameMemory->Ship.point3.y <= -LogicalCordinates.y/2   ) 
		
	{
		
		GameMemory->Ship.point1.x += LogicalCordinates.x;
		GameMemory->Ship.point1.y += LogicalCordinates.y;
		
		GameMemory->Ship.point2.x += LogicalCordinates.x;
		GameMemory->Ship.point2.y += LogicalCordinates.y;
		
		GameMemory->Ship.point3.y += LogicalCordinates.y;
		GameMemory->Ship.point3.x += LogicalCordinates.x;
		
		
	}
	
	
	if(GameMemory->Ship.point1.y >= LogicalCordinates.y/2 &&
	   GameMemory->Ship.point1.x <= -LogicalCordinates.x/2 || 
	   GameMemory->Ship.point2.x <= -LogicalCordinates.x/2 &&
	   GameMemory->Ship.point2.y >= LogicalCordinates.y/2 || 
	   GameMemory->Ship.point3.x <= -LogicalCordinates.x/2 &&
	   GameMemory->Ship.point3.y >= LogicalCordinates.y/2   ) 
		
	{
		
		GameMemory->Ship.point1.x += LogicalCordinates.x;
		GameMemory->Ship.point1.y -= LogicalCordinates.y;
		
		
		GameMemory->Ship.point2.x += LogicalCordinates.x;
		GameMemory->Ship.point2.y -= LogicalCordinates.y;
		
		
		GameMemory->Ship.point3.y -= LogicalCordinates.y;
		GameMemory->Ship.point3.x += LogicalCordinates.x;
		
		
	}
	
	
	{
		
		
		if(GameMemory->Ship.point1.y >= LogicalCordinates.y/2  && GameMemory->Ship.point2.y >= LogicalCordinates.y/2  && GameMemory->Ship.point3.y >= LogicalCordinates.y/2   )
			// top y 
		{
			
			WarpPoints(buffer,&GameMemory->Ship.point1);
			WarpPoints(buffer,&GameMemory->Ship.point2);
			WarpPoints(buffer,&GameMemory->Ship.point3);
			
		}
		
		if(GameMemory->Ship.point1.y <= -LogicalCordinates.y/2 && GameMemory->Ship.point2.y <= -LogicalCordinates.y/2 && GameMemory->Ship.point3.y <= -LogicalCordinates.y/2)
		{
			
			
			WarpPoints(buffer,&GameMemory->Ship.point1);
			WarpPoints(buffer,&GameMemory->Ship.point2);
			WarpPoints(buffer,&GameMemory->Ship.point3);
			
			
		}
		
		if(GameMemory->Ship.point1.x <= -LogicalCordinates.x/2 && GameMemory->Ship.point2.x <= -LogicalCordinates.x/2 && GameMemory->Ship.point3.x <= -LogicalCordinates.x/2)
		{
			
			
			WarpPoints(buffer,&GameMemory->Ship.point1);
			WarpPoints(buffer,&GameMemory->Ship.point2);
			WarpPoints(buffer,&GameMemory->Ship.point3);
			
			
		}
		
		if(GameMemory->Ship.point1.x >= LogicalCordinates.x/2 && GameMemory->Ship.point2.x >= LogicalCordinates.x/2 && GameMemory->Ship.point3.x >= LogicalCordinates.x/2)
		{
			
			
			WarpPoints(buffer,&GameMemory->Ship.point1);
			WarpPoints(buffer,&GameMemory->Ship.point2);
			WarpPoints(buffer,&GameMemory->Ship.point3);
			
			
		}
		
	}
	//check all four corners
	DrawTriangle(buffer,&GameMemory->Ship);
	
	
	
	
	
	//float rad = angle/(180*pi);
	
	
	/*
	Ship.point1.x = result[0];
	Ship.point1.y = result[1];
	Ship.point2.x = result[2];
	Ship.point2.y = result[3];
	Ship.point3.x = result[4];
	Ship.point3.y = result[5];
	*/
	
	//DrawRect(buffer,playerx,playery,10,10,0xFFF000F0);
	//RenderRectInPixels(0xFFFBBFAA, buffer ,0 , 200 , 20 , 70);
}
