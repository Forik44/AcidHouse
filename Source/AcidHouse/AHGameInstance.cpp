#include "AHGameInstance.h"

bool UAHGameInstance::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool bResult = Super::ProcessConsoleExec(Cmd, Ar, Executor);

	if (!bResult)
	{
		TArray<UGameInstanceSubsystem*> Subsystems = GetSubsystemArray<UGameInstanceSubsystem>();
		for (UGameInstanceSubsystem* Subsystem : Subsystems)
		{
			bResult |= Subsystem->ProcessConsoleExec(Cmd, Ar, Executor); 
		}
	}

	return bResult;
}
