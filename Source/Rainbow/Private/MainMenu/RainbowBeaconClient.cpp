#include "Rainbow/Public/MainMenu/RainbowBeaconClient.h"

ARainbowBeaconClient::ARainbowBeaconClient()
{
	PlayerIndex = 0;
}


void ARainbowBeaconClient::OnFailure()
{
	UE_LOG(LogTemp, Warning, TEXT("CLIENT FAILED TO CONNECT TO HOST BEACON"));
	FOnConnected.Broadcast(false);
}

void ARainbowBeaconClient::OnConnected()
{
	UE_LOG(LogTemp, Warning, TEXT("CLIENT CONNECTED TO HOST BEACON"));
	FOnConnected.Broadcast(true);
}

void ARainbowBeaconClient::SendChatMessage(const FText& ChatMessage)
{
	Server_SendChatMessage(ChatMessage);
}

bool ARainbowBeaconClient::Server_SendChatMessage_Validate(const FText& ChatMessage)
{
	return true;
}

void ARainbowBeaconClient::Server_SendChatMessage_Implementation(const FText& ChatMessage)
{
	FString Message = PlayerName + ": " + ChatMessage.ToString();
	UE_LOG(LogTemp, Warning, TEXT("Chat: %s"), *Message);
	
	if (ARainbowBeaconHostObject* Host = Cast<ARainbowBeaconHostObject>(BeaconOwner))
	{
		Host->SendChatToLobby(FText::FromString(Message));
	}
}

void ARainbowBeaconClient::Client_OnChatMessageReceived_Implementation(const FText& ChatMessage)
{
	FOnChatReceived.Broadcast(ChatMessage);
}

void ARainbowBeaconClient::Client_OnDisconnected_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("DISCONNECTED"))
	FOnDisconnected.Broadcast();
}

void ARainbowBeaconClient::Client_OnLobbyUpdated_Implementation(FRainbowLobbyInfo LobbyInfo)
{
	FOnLobbyUpdated.Broadcast(LobbyInfo);
}

void ARainbowBeaconClient::Client_FullConnect_Implementation()
{
	FOnFullConnect.Broadcast();
}

void ARainbowBeaconClient::FullConnectToServer(const FString& JoinAddress)
{
	if(APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
	}
		LeaveLobby();
}

void ARainbowBeaconClient::SetPlayerIndex(uint8 Index)
{
	PlayerIndex = Index;
}

uint8 ARainbowBeaconClient::GetPlayerIndex()
{
	return PlayerIndex;
}

void ARainbowBeaconClient::SetPlayerName(const FString& NewPlayerName)
{
	PlayerName = NewPlayerName;
}

FString ARainbowBeaconClient::GetPlayerName()
{
	return PlayerName;
}

bool ARainbowBeaconClient::ConnectToServer(const FString& Address)
{
	FURL Destination = FURL(nullptr, *Address, ETravelType::TRAVEL_Absolute);
	Destination.Port = 7787;
	return InitClient(Destination);
}

void ARainbowBeaconClient::LeaveLobby()
{
	DestroyBeacon();
}

