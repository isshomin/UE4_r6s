#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "weapon.generated.h"

USTRUCT(BlueprintType)
struct FIKProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimSequence* AnimPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform CustomOffsetTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Aimoffset = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WegihtScale = 1.f;

};

UCLASS(Abstract)
class RAINBOW_API Aweapon : public AActor
{
	GENERATED_BODY()

public:
	Aweapon();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		class USkeletalMeshComponent* Mesh;


	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
		class AFPSCharacter* CurrentOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FIKProperties IKProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FTransform PlacementTransform;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IK")
	FTransform GetSightsWorldTransform() const;
	virtual FORCEINLINE FTransform GetSightsWorldTransform_Implementation() const { return Mesh->GetSocketTransform(FName("Sights")); }

};