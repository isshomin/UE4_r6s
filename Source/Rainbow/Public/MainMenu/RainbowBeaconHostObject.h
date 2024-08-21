#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconHostObject.h"
#include "Http.h"
#include "RainbowGameInstance.h"
#include "RainbowBeaconHostObject.generated.h"

USTRUCT(BlueprintType)
struct FRainbowLobbyInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	class UTexture2D* MapImage;

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> PlayerList;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHostLobbyUpdated, FRainbowLobbyInfo, FonHostLobbyUpdate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHostChatReceived, const FText&, FOnHostChatReceived);

UCLASS()
class RAINBOW_API ARainbowBeaconHostObject : public AOnlineBeaconHostObject
{
	GENERATED_BODY()

public:
	ARainbowBeaconHostObject();

protected:
	FHttpModule* Http;
	int ServerID;
	void OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);

protected:
	FServerData ServerData;
	FRainbowLobbyInfo LobbyInfo;

	UFUNCTION(BlueprintCallable)
	void SetServerData(FServerData NewServerData);
		
	UFUNCTION(BlueprintCallable)
	void UpdateServerData(FServerData NewServerData);

	UFUNCTION(BlueprintCallable)
	int GetCurrentPlayerCount();

	UPROPERTY(BlueprintAssignable)
	FHostLobbyUpdated FOnHostLobbyUpdated; 
		
	UPROPERTY(BlueprintAssignable)
	FHostChatReceived FOnHostChatReceived;

	FTimerHandle TInitialLobbyHandle;
	void InitialLobbyHandling();

	UFUNCTION(BlueprintCallable)
	void UpdateLobbyInfo(FRainbowLobbyInfo NewLobbyInfo);

	void UpdateClientLobbyInfo();

	UFUNCTION(BlueprintCallable)
	void SendChatMessage(const FText& ChatMessage);

protected:
	virtual void BeginPlay() override;

	virtual void OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection) override;
	virtual void NotifyClientDisconnected(AOnlineBeaconClient* LeavingClientActor) override;

	UFUNCTION(BlueprintCallable)
		void ShutdownServer();
	

	void DisconnectAllClients();

	virtual void DisconnectClient(AOnlineBeaconClient* ClientActor) override;

	UFUNCTION(BlueprintCallable)
	void StartServer(const FString& MapURL);


public:
	UFUNCTION(BlueprintCallable)
	void SendChatToLobby(const FText& ChatMessage);

};
