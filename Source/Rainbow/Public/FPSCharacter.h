#pragma once

#include "CoreMinimal.h"
#include "weapon.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Animation/AnimInstance.h"
#include "FPSCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCurrentWeaponChangedDelegate, class Aweapon*, CurrentWeapon, const class Aweapon*, OldWeapon);

UCLASS()
class RAINBOW_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFPSCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

public:	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	class USkeletalMeshComponent* ClientMesh;



protected:
	UPROPERTY(EditDefaultsOnly, Category="Configurations")
	TArray<TSubclassOf<class Aweapon>> DefaultWeapons;

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category="State")
	TArray<class Aweapon*> Weapons;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentWeapon, Category="State")
	class Aweapon* CurrentWeapon;

	UPROPERTY(BlueprintAssignable, Category="Delegates")
	FCurrentWeaponChangedDelegate CurrentWeaponChangedDelegate;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="State")
	int32 CurrentIndex = 0;

	UFUNCTION(BlueprintCallable, Category="Character")
	virtual void EquipWeapon(const int32 Index);

protected:
	UFUNCTION()
	virtual void OnRep_CurrentWeapon(const class Aweapon* OldWeapon);
	
	UFUNCTION(Server, Reliable)
	void Server_SetCurrentWeapon(class Aweapon* Weapon);
	virtual void Server_SetCurrentWeapon_Implementation(class Aweapon* NewWeapon);


public:
	UFUNCTION(BlueprintCallable, Category="Anim")
	virtual void StartAiming();

	UFUNCTION(BlueprintCallable, Category="Anim")
	virtual void ReverseAiming();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Anim")
	float ADSWeight = 0.f;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Configurations|Anim")
	class UCurveFloat* AimingCurve;

	FTimeline AimingTimeline;

	UFUNCTION(Server, Reliable)
	void Server_Aim(const bool bForward = true);
	virtual FORCEINLINE void Server_Aim_Implementation(const bool bForward)
	{
		Multi_Aim(bForward);
		Multi_Aim_Implementation(bForward);

	}

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Aim(const bool bForward);
	virtual void Multi_Aim_Implementation(const bool bForward);

	UFUNCTION()
	virtual void TimelineProgress(const float Value);


protected:
	virtual void NextWeapon();
	virtual void LastWeapon();


	void MoveForward(const float Value);
	void MoveRight(const float Value);

	void Pitch(const float Value);
	void Yaw(const float Value);

};
