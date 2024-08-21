#include "Rainbow/Public/MainMenu/FPSMainMenuModeBase.h"
#include "Rainbow/Public/MainMenu/RainbowBeaconHostObject.h"
#include "OnlineBeaconHost.h"

AFPSMainMenuModeBase::AFPSMainMenuModeBase()
{
	HostObject = nullptr;
	Host = nullptr;
}

bool AFPSMainMenuModeBase::CreateHostBeacon()
{
	Host = GetWorld()->SpawnActor<AOnlineBeaconHost>(AOnlineBeaconHost::StaticClass());
	if (Host)
	{
			UE_LOG(LogTemp, Warning, TEXT("SPAWNED AOnlineBeaconHost"))
		if (Host->InitHost())
		{
			Host->PauseBeaconRequests(false);
			UE_LOG(LogTemp, Warning, TEXT("SPAWNED INIT HOST"))
			HostObject = GetWorld()->SpawnActor<ARainbowBeaconHostObject>(ARainbowBeaconHostObject::StaticClass());
			if (HostObject)
			{
				UE_LOG(LogTemp, Warning, TEXT("SPAWNED HOST OBJECT"))
				Host->RegisterHost(HostObject);
				return true;
			}
		}
	}
	return false;
}

class ARainbowBeaconHostObject* AFPSMainMenuModeBase::GetBeaconHost()
{
	return HostObject;
}
