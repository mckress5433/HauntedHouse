
#include "GlobalFunctionLibrary.h"
#include "GlobalConsoleVariables.h"

int GlobalFunctionLibrary::GetInteractionDebugValue()
{
	return CVarInteractionDebugVariable.GetValueOnAnyThread(false);
}
