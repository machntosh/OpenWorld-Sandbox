#pragma once
#include "DrawDebugHelpers.h"


#define DRAW_SPHERE(ItemLocation) if (GetWorld()) DrawDebugSphere(GetWorld(), ItemLocation,25.f,12,FColor::Cyan,true);
#define DRAW_SPHERE_COLOR(ItemLocation) if (GetWorld()) DrawDebugSphere(GetWorld(), ItemLocation,6.f,12,FColor::Orange,false, 5.f);
#define DRAW_SPHERE_SINGLE_FRAME(ItemLocation) if (GetWorld()) DrawDebugSphere(GetWorld(), ItemLocation,25.f,12,FColor::Cyan,false,-1.f);
#define DRAW_LINE(ItemLocation,Forward) if (GetWorld()) DrawDebugLine(GetWorld(), ItemLocation, ItemLocation + Forward * 100.f, FColor::Red, true, -1.f, 0, 1.f);
#define DRAW_LINE_SINGLE_FRAME(ItemLocation,Forward) if (GetWorld()) DrawDebugLine(GetWorld(), ItemLocation, ItemLocation+Forward*100.f, FColor::Red, false, -1.f, 0, 1.f);
#define DRAW_POINT(ItemLocation) if (GetWorld()) DrawDebugPoint(GetWorld(), ItemLocation, 15.f, FColor::Red, true, -1.f, 0);
#define DRAW_POINT_SINGLE_FRAME(ItemLocation) if (GetWorld()) DrawDebugPoint(GetWorld(), ItemLocation, 15.f, FColor::Red, false, -1.f, 0);
#define DRAW_VECTOR(StartLocation,EndLocation) if (GetWorld()) \
{ \
DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.f, 0, 1.f); \
DrawDebugPoint(GetWorld(), EndLocation, 15.f, FColor::Red, true); \
}
#define DRAW_VECTOR_SINGLE_FRAME(StartLocation,EndLocation) if (GetWorld()) \
{ \
DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.f, 0, 1.f); \
DrawDebugPoint(GetWorld(), EndLocation, 15.f, FColor::Red, false,-1.f); \
}

namespace Debug
{
	static void Print(const FString& Msg, const FColor& Color =FColor::MakeRandomColor(),int32 InKey = -1)
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(InKey,6.f,Color,Msg);
	}
		UE_LOG(LogTemp,Warning,TEXT("%s"),*Msg);
}
}



