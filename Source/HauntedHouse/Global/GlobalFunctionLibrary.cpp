
#include "GlobalFunctionLibrary.h"
#include "GlobalConsoleVariables.h"

int GlobalFunctionLibrary::GetInteractionDebugValue()
{
	return CVarInteractionDebugVariable.GetValueOnAnyThread(false);
}

int GlobalFunctionLibrary::GetSaveSystemDebugValue()
{
	return CVarSaveSystemDebugVariable.GetValueOnAnyThread(false);
}
