#include "weapon.h"

Aweapon::Aweapon()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

}

void Aweapon::BeginPlay()
{
	Super::BeginPlay();

	if(!CurrentOwner)
		Mesh->SetVisibility(false);
	
}

