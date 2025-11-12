#include "W:\HauntedHouse\Intermediate\Build\Win64\x64\HauntedHouseEditor\Development\UnrealEd\SharedPCH.UnrealEd.Project.ValApi.ValExpApi.Cpp20.InclOrderUnreal5_5.h"
#include "GlobalFunctionLibrary.h"

#include "GlobalConsoleVariables.h"

int GlobalFunctionLibrary::GetInteractionDebugValue()
{
	return CVarInteractionDebugVariable.GetValueOnAnyThread(false);
}
