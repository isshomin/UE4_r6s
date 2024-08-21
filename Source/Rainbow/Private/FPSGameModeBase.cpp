#include "FPSGameModeBase.h"
#include "FPSCharacter.h"

AFPSGameModeBase::AFPSGameModeBase()
{
	static ConstructorHelpers::FClassFinder<AFPSCharacter> BP_Char1(TEXT("Blueprint'/Game/Blueprints/BP_FPSCharacter.BP_FPSCharacter_C'"));
	if (BP_Char1.Succeeded())
	{
		DefaultPawnClass = BP_Char1.Class;
	}
}
