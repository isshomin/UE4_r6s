#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSMainMenuModeBase.generated.h"

UCLASS()
class RAINBOW_API AFPSMainMenuModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPSMainMenuModeBase();
	
protected:
	UFUNCTION(BlueprintCallable)
		bool CreateHostBeacon();
	
	class ARainbowBeaconHostObject* HostObject;

	class AOnlineBeaconHost* Host;

	UFUNCTION(BlueprintCallable)
	class ARainbowBeaconHostObject* GetBeaconHost();

};
