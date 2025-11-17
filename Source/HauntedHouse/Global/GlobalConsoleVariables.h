
inline TAutoConsoleVariable<int> CVarInteractionDebugVariable(
TEXT("HH.InteractionDebugVariable"),
0,
TEXT("Controls the display of interactable debug lines. \n")
TEXT("0 = None, 1 = ForOneFrame, 2 = ForDuration, 3 = Persistent")
);

inline TAutoConsoleVariable<int> CVarSaveSystemDebugVariable(
TEXT("HH.InteractionDebugVariable"),
0,
TEXT("Controls console logs for save system \n")
TEXT("0 = None, 1 = logs active")
);