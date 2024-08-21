#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "weapon.h"
#include "FPSSAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class RAINBOW_API UFPSSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFPSSAnimInstance();

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UFUNCTION()
	virtual void CurrentWeaponChanged(class Aweapon* NewWeapon, const class Aweapon* OldWweapon);
	
	virtual void SetVars(const float DeltaTime);
	virtual void CalclateWeaponSway(const float DeltaTime);

	virtual void SetIKTransform();

public:
	UPROPERTY(BlueprintReadWrite, Category="Anim")
	class AFPSCharacter* Character;

	UPROPERTY(BlueprintReadWrite, Category="Anim")
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, Category="Anim")
	class Aweapon* CurrentWeapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	FIKProperties IKProperties;

	UPROPERTY(BlueprintReadOnly, Category="Anim")
	FRotator LastRotation;

	//IK 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	FTransform CameraTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	FTransform RelativeCameraTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	FTransform RHandToSightsTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	FTransform OffsetTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	float ADSWeight = 0.f;

	UPROPERTY(BlueprintReadWrite, Category="Anim")
	FRotator AccumulativeRotation;

	UPROPERTY(BlueprintReadWrite, Category="Anim")
	FRotator AccumulativeRotationInterp;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move", meta=(AllowPrivateAccess = true))
	float Speed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move", meta=(AllowPrivateAccess = true))
	float Vertical;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move", meta=(AllowPrivateAccess = true))
	float Horizontal;
};