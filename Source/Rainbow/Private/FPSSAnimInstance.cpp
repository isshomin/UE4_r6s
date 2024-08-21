#include "FPSSAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "FPSCharacter.h"

UFPSSAnimInstance::UFPSSAnimInstance()
{

}

void UFPSSAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	/*Character = Cast<AFPSCharacter>(TryGetPawnOwner());
	if (Character)
	{
		Mesh = Character->GetMesh();
		Character->CurrentWeaponChangedDelegate.AddDynamic(this, &UFPSSAnimInstance::CurrentWeaponChanged);
		CurrentWeaponChanged(Character->CurrentWeapon, nullptr);
	}*/
}

void UFPSSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	
	
	if (!Character)
	{
		Character = Cast<AFPSCharacter>(TryGetPawnOwner());
		if (Character)
		{
			Mesh = Character->GetMesh();
			Speed = Character->GetVelocity().Size();
			Character->CurrentWeaponChangedDelegate.AddDynamic(this, &UFPSSAnimInstance::CurrentWeaponChanged);
			CurrentWeaponChanged(Character->CurrentWeapon, nullptr);
		}
		else
			return;
	}
	
	SetVars(DeltaTime);
	CalclateWeaponSway(DeltaTime);

	LastRotation = CameraTransform.Rotator();

}

void UFPSSAnimInstance::CurrentWeaponChanged(class Aweapon* NewWeapon, const class Aweapon* OldWweapon)
{
	CurrentWeapon = NewWeapon;
	if (CurrentWeapon)
	{
		IKProperties = CurrentWeapon->IKProperties;
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UFPSSAnimInstance::SetIKTransform);
	}
}

void UFPSSAnimInstance::SetVars(const float DeltaTime)
{
	CameraTransform = FTransform(Character->GetBaseAimRotation(),
		Character->Camera->GetComponentLocation());

	const FTransform& RootOffset = Mesh->GetSocketTransform(FName("root"), RTS_Component).Inverse()
		* Mesh->GetSocketTransform(FName("ik_hand_root"));

	RelativeCameraTransform = CameraTransform.GetRelativeTransform(RootOffset);

	ADSWeight = Character->ADSWeight;

	constexpr float AngleClamp = 6.f;
	const FRotator& AddRotation = CameraTransform.Rotator() - LastRotation;
	FRotator AddRotationClamped = FRotator(FMath::ClampAngle(AddRotation.Pitch, -AngleClamp, AngleClamp) * 1.5f,
		FMath::ClampAngle(AddRotation.Yaw, -AngleClamp, AngleClamp), 0.f);
	AddRotationClamped.Roll = AddRotationClamped.Yaw * 0.7f;

	AccumulativeRotation += AddRotationClamped;
	AccumulativeRotation = UKismetMathLibrary::RInterpTo(AccumulativeRotation, FRotator::ZeroRotator, DeltaTime, 30.f);
	AccumulativeRotationInterp = UKismetMathLibrary::RInterpTo(AccumulativeRotationInterp, AccumulativeRotation, DeltaTime, 5.f);

}

void UFPSSAnimInstance::CalclateWeaponSway(const float DeltaTime)
{
	FVector LocationOffset = FVector::ZeroVector;
	FRotator RotationOffset = FRotator::ZeroRotator;

	const FRotator& AccumulativeRotationInterpInverse = AccumulativeRotationInterp.GetInverse();
	RotationOffset += AccumulativeRotationInterpInverse;

	LocationOffset += FVector(0.f, AccumulativeRotationInterpInverse.Yaw, AccumulativeRotationInterpInverse.Pitch) / 6.f;

	LocationOffset *= IKProperties.WegihtScale;
	RotationOffset.Pitch *= IKProperties.WegihtScale;
	RotationOffset.Yaw *= IKProperties.WegihtScale;
	RotationOffset.Roll *= IKProperties.WegihtScale;

	OffsetTransform = FTransform(RotationOffset, LocationOffset);

}

void UFPSSAnimInstance::SetIKTransform()
{
	RHandToSightsTransform = CurrentWeapon->GetSightsWorldTransform().GetRelativeTransform(Mesh->GetSocketTransform(FName("hand_r")));
}
