#include "Rainbow/Public/MainMenu/RainbowBeaconHostObject.h"
#include "Rainbow/Public/MainMenu/RainbowBeaconClient.h"
#include "Rainbow/Public/MainMenu/FPSMainMenuModeBase.h"

#include "OnlineBeaconHost.h"
#include "TimerManager.h"

ARainbowBeaconHostObject::ARainbowBeaconHostObject()
{
	ClientBeaconActorClass = ARainbowBeaconClient::StaticClass();
	BeaconTypeName = ClientBeaconActorClass->GetName();

	Http = &FHttpModule::Get();
	ServerID = -1;
}

void ARainbowBeaconHostObject::BeginPlay()
{
	LobbyInfo.PlayerList.Add(FString("Host"));
	GetWorld()->GetTimerManager().SetTimer(TInitialLobbyHandle, this, &ARainbowBeaconHostObject::InitialLobbyHandling, 0.2f, false);
}

void ARainbowBeaconHostObject::InitialLobbyHandling()
{
	UpdateLobbyInfo(LobbyInfo);

}

void ARainbowBeaconHostObject::SetServerData(FServerData NewServerData)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	ServerData = NewServerData;
	ServerData.CurrentPlayers = GetCurrentPlayerCount();

	JsonObject->SetNumberField("ServerID", 0);
	JsonObject->SetStringField("IPAddress", "123456789");
	JsonObject->SetStringField("ServerName", ServerData.ServerName);
	JsonObject->SetStringField("MapName", ServerData.MapName);
	JsonObject->SetNumberField("CurrentPlayers", ServerData.CurrentPlayers);
	JsonObject->SetNumberField("MaxPlayers", ServerData.MaxPlayers);

	FString JsonString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &ARainbowBeaconHostObject::OnProcessRequestComplete);

	Request->SetURL("https://localhost:44394/api/Host");
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->SetContentAsString(JsonString);

	Request->ProcessRequest();
}

void ARainbowBeaconHostObject::UpdateServerData(FServerData NewServerData)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	ServerData = NewServerData;
	ServerData.CurrentPlayers = GetCurrentPlayerCount();

	JsonObject->SetNumberField("ServerID", 0);
	JsonObject->SetStringField("IPAddress", "123456789");
	JsonObject->SetStringField("ServerName", ServerData.ServerName);
	JsonObject->SetStringField("MapName", ServerData.MapName);
	JsonObject->SetNumberField("CurrentPlayers", ServerData.CurrentPlayers);
	JsonObject->SetNumberField("MaxPlayers", ServerData.MaxPlayers);

	FString JsonString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &ARainbowBeaconHostObject::OnProcessRequestComplete);

	Request->SetURL("https://localhost:44394/api/Host/1");
	Request->SetVerb("PUT");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->SetContentAsString(JsonString);

	Request->ProcessRequest();
}

int ARainbowBeaconHostObject::GetCurrentPlayerCount()
{
	return LobbyInfo.PlayerList.Num();
}

void ARainbowBeaconHostObject::OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
	if (Success)
	{
		ServerID = FCString::Atoi(*Response->GetContentAsString());
		UE_LOG(LogTemp, Warning, TEXT("HttpRequest Success: %s"), *Response->GetContentAsString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HttpRequest FAILED"));
	}
}

void ARainbowBeaconHostObject::UpdateLobbyInfo(FRainbowLobbyInfo NewLobbyInfo)
{
	LobbyInfo.MapImage = NewLobbyInfo.MapImage;
	UpdateClientLobbyInfo();
	FOnHostLobbyUpdated.Broadcast(LobbyInfo);
}

void ARainbowBeaconHostObject::UpdateClientLobbyInfo()
{
	for (AOnlineBeaconClient* ClientBeacon : ClientActors)
	{
		if (ARainbowBeaconClient* Client = Cast<ARainbowBeaconClient>(ClientBeacon))
		{
			Client->Client_OnLobbyUpdated(LobbyInfo);
		}
	}
}
	
void ARainbowBeaconHostObject::SendChatMessage(const FText& ChatMessage)
{

}

void ARainbowBeaconHostObject::OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection)
{
	Super::OnClientConnected(NewClientActor, ClientConnection);

	if (NewClientActor)
	{	
		if (GetCurrentPlayerCount() >= ServerData.MaxPlayers)
		{
			if (ARainbowBeaconClient* Client = Cast<ARainbowBeaconClient>(NewClientActor))
			{
				Client->SetPlayerIndex(99);
				DisconnectClient(NewClientActor);
				return;
			}
		}
		FString PlayerName = FString("Player ");
		uint8 Index = LobbyInfo.PlayerList.Num();
		PlayerName.Append(FString::FromInt(Index));
		LobbyInfo.PlayerList.Add(PlayerName);

		if(ARainbowBeaconClient* Client = Cast<ARainbowBeaconClient>(NewClientActor))
		{ 
			Client->SetPlayerIndex(Index);
			Client->SetPlayerName(PlayerName);
		}

		FOnHostLobbyUpdated.Broadcast(LobbyInfo);

		UE_LOG(LogTemp, Warning, TEXT("CONNECTED CLIENT VALID"));
		UpdateClientLobbyInfo();

		UpdateServerData(ServerData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CONNECTED CLIENT INVALID"))
	}
}

void ARainbowBeaconHostObject::NotifyClientDisconnected(AOnlineBeaconClient* LeavingClientActor)
{
	Super::NotifyClientDisconnected(LeavingClientActor);

	UE_LOG(LogTemp, Warning, TEXT("Client Has DISCONNECTED"));
	if (ARainbowBeaconClient* Client = Cast<ARainbowBeaconClient>(LeavingClientActor))
	{
		uint8 Index = Client->GetPlayerIndex();
		if(Index == 99)
		{
			return;
		}
		LobbyInfo.PlayerList.RemoveAt(Index);
	}

	FOnHostLobbyUpdated.Broadcast(LobbyInfo);
	UpdateClientLobbyInfo();
	UpdateServerData(ServerData);
}

void ARainbowBeaconHostObject::ShutdownServer()
{
	DisconnectAllClients();
	if (AOnlineBeaconHost* Host = Cast<AOnlineBeaconHost>(GetOwner()))
	{
		UE_LOG(LogTemp, Warning, TEXT("DESTROYING HOSTBEACON"));
		Host->UnregisterHost(BeaconTypeName);
		Host->DestroyBeacon();
	}
	
	if(ServerID != -1)
	{ 
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

		Request->SetURL("https://localhost:44394/api/Host/" + FString::FromInt(ServerID));
		Request->SetVerb("DELETE");
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

		Request->ProcessRequest();

	}
}
	
void ARainbowBeaconHostObject::DisconnectAllClients()
{
	UE_LOG(LogTemp, Warning, TEXT("DISCONNECTING ALL Clients"));
	for (AOnlineBeaconClient* Client : ClientActors)
	{
		if(Client)
			DisconnectClient(Client);
	}
}

void ARainbowBeaconHostObject::DisconnectClient(AOnlineBeaconClient* ClientActor)
{
	AOnlineBeaconHost* BeaconHost = Cast<AOnlineBeaconHost>(GetOwner());
	if (BeaconHost)
	{
		if (ARainbowBeaconClient* Client = Cast<ARainbowBeaconClient>(ClientActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("DISCONNECTING CLIENT %s"), *ClientActor->GetName());
			Client->Client_OnDisconnected();
		}
		BeaconHost->DisconnectClient(ClientActor);
	}
}


void ARainbowBeaconHostObject::StartServer(const FString& MapURL)
{
	for (AOnlineBeaconClient* ClientBeacon : ClientActors)
	{
		if (ARainbowBeaconClient* Client = Cast<ARainbowBeaconClient>(ClientBeacon))
		{
			Client->Client_FullConnect();
		}
	}

	ShutdownServer();
	GetWorld()->ServerTravel(MapURL + "?listen");
}

void ARainbowBeaconHostObject::SendChatToLobby(const FText& ChatMessage)
{
	FOnHostChatReceived.Broadcast(ChatMessage);

	for (AOnlineBeaconClient* ClientBeacon : ClientActors)
	{
		if (ARainbowBeaconClient* Client = Cast<ARainbowBeaconClient>(ClientBeacon))
		{
			Client->Client_OnChatMessageReceived(ChatMessage);
		}
	}
}

