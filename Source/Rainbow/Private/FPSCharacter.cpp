#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"



AFPSCharacter::AFPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetTickGroup(ETickingGroup::TG_PostUpdateWork);
	GetMesh()->bVisibleInReflectionCaptures = true;
	GetMesh()->bCastHiddenShadow = true;

	ClientMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ClientMesh"));
	ClientMesh->SetCastShadow(false);
	ClientMesh->bCastHiddenShadow = false;
	ClientMesh->bVisibleInReflectionCaptures = false;
	ClientMesh->SetTickGroup(ETickingGroup::TG_PostUpdateWork);
	ClientMesh->SetupAttachment(GetMesh());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = true;
	Camera->SetupAttachment(GetMesh(), FName("head"));
	

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FPS_Char1(TEXT("SkeletalMesh'/Game/Characters/ThirdCharacter/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (FPS_Char1.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(FPS_Char1.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f),
			FRotator(0.f, -90.f, 0.f));
	}
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Client_Char1(TEXT("SkeletalMesh'/Game/Characters/ThirdCharacter/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (Client_Char1.Succeeded())
	{
		ClientMesh->SetSkeletalMesh(Client_Char1.Object);
	
	}
}

void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AimingCurve)
	{
		FOnTimelineFloat TimelineFloat;
		TimelineFloat.BindDynamic(this, &AFPSCharacter::TimelineProgress);

		AimingTimeline.AddInterpFloat(AimingCurve, TimelineFloat);
	}

	if (IsLocallyControlled())
	{
		ClientMesh->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
		GetMesh()->SetVisibility(false);
	}
	else
	{
		ClientMesh->DestroyComponent();
	}

	if (HasAuthority())
	{
		for (const TSubclassOf<Aweapon>& WeaponClass : DefaultWeapons)
		{
			if(!WeaponClass)
				continue;
			FActorSpawnParameters Params;
			Params.Owner = this;
			Aweapon* SpawnedWeapon = GetWorld()->SpawnActor<Aweapon>(WeaponClass, Params);
			const int32 Index = Weapons.Add(SpawnedWeapon);
			if (Index == CurrentIndex)
			{
				CurrentWeapon = SpawnedWeapon;
				OnRep_CurrentWeapon(nullptr);
			}
		}
	}
	
}

void AFPSCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(1, 1.5f, FColor::White, FString::Printf(TEXT("Speed : %f"), GetVelocity().Size()));
	AimingTimeline.TickTimeline(DeltaTime);


}


void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AFPSCharacter, Weapons, COND_None);
	DOREPLIFETIME_CONDITION(AFPSCharacter, CurrentWeapon, COND_None);
	DOREPLIFETIME_CONDITION(AFPSCharacter, ADSWeight, COND_None);
}

void AFPSCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	DOREPLIFETIME_ACTIVE_OVERRIDE(AFPSCharacter, ADSWeight, ADSWeight >= 1.f || ADSWeight <= 0.f);

}


void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction(FName("Aim"), EInputEvent::IE_Pressed, this, &AFPSCharacter::StartAiming);
	PlayerInputComponent->BindAction(FName("Aim"), EInputEvent::IE_Released, this, &AFPSCharacter::ReverseAiming);
	PlayerInputComponent->BindAction(FName("NextWeapon"), EInputEvent::IE_Pressed, this, &AFPSCharacter::NextWeapon);
	PlayerInputComponent->BindAction(FName("LastWeapon"), EInputEvent::IE_Pressed, this, &AFPSCharacter::LastWeapon);
	
	
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Yaw"), this, &AFPSCharacter::Yaw);
	PlayerInputComponent->BindAxis(FName("Pitch"), this, &AFPSCharacter::Pitch);

}

void AFPSCharacter::OnRep_CurrentWeapon(const class Aweapon* OldWeapon)
{
	if (CurrentWeapon)
	{
		if (!CurrentWeapon->CurrentOwner)
		{
			const FTransform& PlacementTransform = CurrentWeapon->PlacementTransform * GetMesh()->GetSocketTransform(FName("weaponsocket_r"));
			CurrentWeapon->SetActorTransform(PlacementTransform, false, nullptr, ETeleportType::TeleportPhysics);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("weaponsocket_r"));
			
			CurrentWeapon->CurrentOwner = this;
		
		}

			CurrentWeapon->Mesh->SetVisibility(true);
	}

	if (OldWeapon)
	{
		OldWeapon->Mesh->SetVisibility(false);
	}

	CurrentWeaponChangedDelegate.Broadcast(CurrentWeapon, OldWeapon);

}

void AFPSCharacter::EquipWeapon(const int32 Index)
{
	if (!Weapons.IsValidIndex(Index) || CurrentWeapon == Weapons[Index])
		return;
	if (IsLocallyControlled() || HasAuthority())
	{
		CurrentIndex = Index;

		const Aweapon* OldWeapon = CurrentWeapon;
		CurrentWeapon = Weapons[Index];
		OnRep_CurrentWeapon(OldWeapon);
	}
	
	if(!HasAuthority())
	{
		Server_SetCurrentWeapon(Weapons[Index]);
	}

}

void AFPSCharacter::Server_SetCurrentWeapon_Implementation(Aweapon* NewWeapon)
{
	const Aweapon* OldWeapon = CurrentWeapon;
	CurrentWeapon = NewWeapon;
	OnRep_CurrentWeapon(OldWeapon);
}

void AFPSCharacter::StartAiming()
{
	if (IsLocallyControlled() || HasAuthority())
	{
		Multi_Aim_Implementation(true);
		
	}

	if (!HasAuthority())
	{
		Server_Aim(true);
	}
}

void AFPSCharacter::ReverseAiming()
{
	if (IsLocallyControlled() || HasAuthority())
	{
		Multi_Aim_Implementation(false);

	}

	if (!HasAuthority())
	{
		Server_Aim(false);
	}
}

void AFPSCharacter::Multi_Aim_Implementation(const bool bForward)
{
	if (bForward)
	{
		AimingTimeline.Play();
	}
	else
	{
		AimingTimeline.Reverse();
	}
}

void AFPSCharacter::TimelineProgress(const float Value)
{
	ADSWeight = Value;
}



void AFPSCharacter::NextWeapon()
{
	const int32 Index = Weapons.IsValidIndex(CurrentIndex + 1) ? CurrentIndex + 1 : 0;
	EquipWeapon(Index);
}

void AFPSCharacter::LastWeapon()
{
	const int32 Index = Weapons.IsValidIndex(CurrentIndex - 1) ? CurrentIndex - 1 : Weapons.Num() - 1;
	EquipWeapon(Index);
}

void AFPSCharacter::MoveForward(const float Value)
{
	const FVector& Direction = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::MoveRight(const float Value)
{
	const FVector& Direction = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);

}

void AFPSCharacter::Pitch(const float Value)
{
	AddControllerPitchInput(Value);
}

void AFPSCharacter::Yaw(const float Value)
{
	AddControllerYawInput(Value);
}

