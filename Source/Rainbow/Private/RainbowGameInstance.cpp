#include "RainbowGameInstance.h"
#include "Engine/World.h"

URainbowGameInstance::URainbowGameInstance()
{
	Http = &FHttpModule::Get();
}

TArray<FServerData>& URainbowGameInstance::GetServerList()
{
	return ServerList;
}

void URainbowGameInstance::GenerateGetServerList()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &URainbowGameInstance::OnServerListProcessRequestComplete);

	Request->SetURL("https://localhost:44394/api/Host");
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->ProcessRequest();

}

void URainbowGameInstance::OnServerListProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
	if (Success)
	{
		FString ResponseStr = Response->GetContentAsString();
		ResponseStr.InsertAt(0, FString("{\"Response\":"));
		ResponseStr.AppendChar('}');

		UE_LOG(LogTemp, Warning, TEXT("Response: %s"), *ResponseStr);

		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseStr);

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			TArray<TSharedPtr<FJsonValue>> JsonValues = JsonObject->GetArrayField(TEXT("Response"));

			ServerList.Empty();

			for (TSharedPtr<FJsonValue> Value : JsonValues)
			{
				FServerData ServerData = FServerData();
				TSharedPtr<FJsonObject> JsonObj = Value->AsObject();

				ServerData.IPAddress = JsonObj->GetStringField("IPAddress");
				ServerData.ServerName = JsonObj->GetStringField("ServerName");
				ServerData.MapName = JsonObj->GetStringField("MapName");
				ServerData.CurrentPlayers = JsonObj->GetIntegerField("CurrentPlayers");
				ServerData.MaxPlayers = JsonObj->GetIntegerField("MaxPlayers");

				ServerList.Add(ServerData);
	
			}
			FOnServersReceived.Broadcast();
			/*for (FServerData ServerData : ServerList)
			{
				UE_LOG(LogTemp, Warning, TEXT("IP: %s"), *ServerData.IPAddress);
				UE_LOG(LogTemp, Warning, TEXT("ServerName: %s"), *ServerData.ServerName);
				UE_LOG(LogTemp, Warning, TEXT("MapName: %s"), *ServerData.MapName);
				UE_LOG(LogTemp, Warning, TEXT("CurrentPlayers: %d"), ServerData.CurrentPlayers);
				UE_LOG(LogTemp, Warning, TEXT("MaxPlayers: %d"), ServerData.MaxPlayers);

				UE_LOG(LogTemp, Warning, TEXT("END OF FIRST SERVER ENTRY"));
			}*/

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GETSERVERLIST FAILED"));
	}
}

void URainbowGameInstance::EnterMap(FString MapURL)
{
	GetWorld()->ServerTravel(MapURL);

}
