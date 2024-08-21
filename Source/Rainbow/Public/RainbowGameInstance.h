#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Http.h"
#include "RainbowGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FMapInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MapURL;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MapName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MapDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* MapImage;

};

USTRUCT(BlueprintType)
struct FServerData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ServerID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString IPAddress;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ServerName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MapName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentPlayers;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxPlayers;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FServersReceived);

UCLASS()
class RAINBOW_API URainbowGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	URainbowGameInstance();

protected:
	FHttpModule* Http;

	TArray<FServerData>	ServerList;

	UFUNCTION(BlueprintCallable)
	TArray<FServerData>& GetServerList();

	UPROPERTY(BlueprintAssignable)
	FServersReceived FOnServersReceived;

	UFUNCTION(BlueprintCallable)
	void GenerateGetServerList();

	void OnServerListProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);

protected:
	UFUNCTION(BlueprintCallable)
	void EnterMap(FString MapURL);

};
