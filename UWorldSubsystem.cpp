// Fill out your copyright notice in the Description page of Project Settings.


#include "UWorldSubsystem.h"



#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"


#include "UWorldCommonGameViewportClient.h"


TArray<UWorld*> UUWorldSubsystem::CreatedWorlds;

void UUWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Lambda = GEngine->OnWorldDestroyed().AddLambda([&](UWorld* World) {
		if (World == GetMainWorldContext().ContextWorld) {
			//	UE_LOG(LogTemp, Warning, TEXT("MainWorld destroyed = %s"), *World->GetName());
			//	//auto CurrentWorld = ActiveWorldContext.ContextWorld;
			//	//CurrentWorld->RemoveController(CurrentWorld->GetFirstPlayerController());
			//
			
			if (CreatedWorlds.Contains(ActiveWorldContext.ContextWorld)) {
				FUWorldContext Context;
				Context.ContextWorld = ActiveWorldContext.ContextWorld;
				DestroyCreatedWorld(Context);
			}
			
			
		}
		});
}

void UUWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	Lambda.Reset();
}

//////void UGameBFL::CreateNewWorld3(const UObject* WorldContextObject, const TSoftObjectPtr<UWorld> Level, UWorld*& World, FUWorldContext2& UWorldContext)
void UUWorldSubsystem::CreateNewWorld(const TSoftObjectPtr<UWorld> Level, TSubclassOf<class AGameModeBase> GameMode, FUWorldContext& UWorldContext)
{

	UWorld* NewWorld = nullptr;

	const FString LevelName2 = FPackageName::ObjectPathToPackageName(Level.ToString());
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("level name = %s"), *LevelName2));

	UPackage* WorldPackage = FindPackage(nullptr, *LevelName2);
	if (WorldPackage == nullptr)
	{
		WorldPackage = LoadPackage(nullptr, *LevelName2, ELoadFlags::LOAD_None);
		if (!WorldPackage)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Package not loaded")));
			return;
		}

	}

	///======

	if (!WorldPackage)
	{
		WorldPackage = CreatePackage(nullptr);
	}

	//if (InWorldType == EWorldType::PIE)
	//{
	//	WorldPackage->SetPackageFlags(PKG_PlayInEditor);
	//}

	// Mark the package as containing a world.  This has to happen here rather than at serialization time,
	// so that e.g. the referenced assets browser will work correctly.
	if (WorldPackage != GetTransientPackage())
	{
		WorldPackage->ThisContainsMap();
	}
	///======

	FName Pname = *(WorldPackage->GetFName().ToString() + FGuid::NewGuid().ToString());
	//FName Pname = MakeUniqueObjectName(WorldPackage, UPackage::StaticClass(), WorldPackage->GetFName());
	UPackage* NewWorldPackage = CreatePackage(*Pname.ToString());
	NewWorldPackage->SetPackageFlags(PKG_ContainsMap);
	//NewWorldPackage->PIEInstanceID = WorldPackage->GetPIEInstanceID();
	//NewWorldPackage->FileName = FName(*WorldPackage->GetName());
	NewWorldPackage->MarkAsFullyLoaded();

	UWorld* PackageWorld = UWorld::FindWorldInPackage(WorldPackage);











	//UWorld* NewWorld = nullptr;

	//const FString LevelName2 = FPackageName::ObjectPathToPackageName(Level.ToString());
	////GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("level name = %s"), *LevelName2));

	//UPackage* WorldPackage = FindPackage(nullptr, *LevelName2);
	//if (WorldPackage == nullptr)
	//{
	//	WorldPackage = LoadPackage(nullptr, *LevelName2, ELoadFlags::LOAD_None);
	//	if (!WorldPackage)
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Package not loaded")));
	//		return;
	//	}

	//}


	//UWorld* PackageWorld = UWorld::FindWorldInPackage(WorldPackage);



	//UPackage* NewWorldPackage = CreatePackage(nullptr);
	////NewWorldPackage->SetPackageFlags(PKG_PlayInEditor);
	////NewWorldPackage->PIEInstanceID = PIEInstanceID;
	////NewWorldPackage->FileName = PackageFName;
	////NewWorldPackage->SetGuid(EditorLevelPackage->GetGuid());
	////NewWorldPackage->MarkAsFullyLoaded();















	FObjectDuplicationParameters Parameters(PackageWorld, NewWorldPackage);
	//FName Pname = *(WorldPackage->GetFName().ToString() + FGuid::NewGuid().ToString());
	//const FString PName = FPackageName::ObjectPathToPackageName(Level.ToString());
	//Parameters.DestName = *PName;
	Parameters.DestClass = PackageWorld->GetClass();
	Parameters.DuplicateMode = EDuplicateMode::PIE;
	Parameters.PortFlags = EPropertyPortFlags::PPF_Duplicate;


	//NewWorld = CastChecked<UWorld>(StaticDuplicateObjectEx(Parameters));
	NewWorld = Cast<UWorld>(StaticDuplicateObjectEx(Parameters));

























	NewWorld->SetShouldTick(true);

	auto GameInstance = UGameplayStatics::GetGameInstance(GEngine->GetCurrentPlayWorld());
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	WorldContext.OwningGameInstance = GameInstance;
	//WorldContext.PIEInstance = -1;
	NewWorld->SetGameInstance(GameInstance);
	NewWorld->WorldType = WorldContext.WorldType;
	//NewWorld->WorldType = EWorldType::PIE;
	WorldContext.SetCurrentWorld(NewWorld);

	//NewWorld->AddController(GetMainWorld()->GetFirstPlayerController());


	auto context = GameInstance->GetWorldContext()->GameViewport;
	//ViewportClient->Init(*GameInstance->GetWorldContext(), GameInstance);
	WorldContext.GameViewport = context;


	// Add to root set so it doesn't get garbage collected.
	NewWorld->AddToRoot();

	// Clear the dirty flags set during SpawnActor and UpdateLevelComponents
	WorldPackage->SetDirtyFlag(false);
	for (UPackage* ExternalPackage : WorldPackage->GetExternalPackages())
	{
		ExternalPackage->SetDirtyFlag(false);
	}

	// Tell the engine we are adding a world (unless we are asked not to)
	if (GEngine)
	{
		GEngine->WorldAdded(NewWorld);
	}


	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("CreateWorld: %s"), *NewWorld->GetFName().ToString()));

	//// In the PIE case the world will already have been initialized as part of CreatePIEWorldByDuplication
	if (!WorldContext.World()->bIsWorldInitialized)
	{
		WorldContext.World()->InitWorld();
	}










	FURL URL;
	//FURL URL(&WorldContext.LastURL, *NewWorld->GetFName().ToString(), ETravelType::TRAVEL_Absolute);
	//URL.AddOption(TEXT("GAME=/Game/ThirdPersonCPP/Blueprints/BpGameMode2.BpGameMode2_C"));
	URL.AddOption(TEXT("GAME=/Game/Blueprints/GameModeBP_CreatedWorld.GameModeBP_CreatedWorld_C"));
	WorldContext.World()->SetGameMode(URL);


	//WorldContext.World()->CreateAISystem();





	WorldContext.World()->InitializeActorsForPlay(URL);



	//AActor* lacder = nullptr;

	for (auto actor : NewWorld->PersistentLevel->Actors) {
		UE_LOG(LogTemp, Warning, TEXT("LoadedWorldActorsClasses: %s"), *UKismetSystemLibrary::GetDisplayName(actor));
		//if (actor->GetClass()->GetName() == "BpGameMode2_C")
		//{
		//	//lacder = actor;
		//}
	}
	UFunction* Func = NewWorld->GetAuthGameMode()->GetClass()->FindFunctionByName(FName("Load"));
	if (Func == nullptr) {
		return;
	}
	FStructOnScope FuncParam(Func);
	NewWorld->GetAuthGameMode()->ProcessEvent(Func, FuncParam.GetStructMemory());




	//UNavigationSystemV1 *NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(NewWorld);
	//if (NavSys)
	//{
	//	ANavigationData *UseNavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::Create);
	//	NavSys->Build();
	//}


	WorldContext.World()->BeginPlay();
	WorldContext.World()->bWorldWasLoadedThisTick = true;




	//if (!CleanupHandle.IsValid()) {

		 //Tick call in PIE
	auto TickHandle = FWorldDelegates::OnWorldTickStart.AddLambda([NewWorld, this](UWorld* World, ELevelTick TickType, float DeltaSeconds) {
		if (World == GEngine->GetCurrentPlayWorld() && GEngine->GetCurrentPlayWorld()->WorldType == EWorldType::PIE) {
			auto TickHand = this->TickHandles.Find(NewWorld);
			if (TickHand->IsValid()) {
				if (NewWorld->ShouldTick())
				{
					NewWorld->Tick(TickType, DeltaSeconds);
					//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("TickInWorld: %s"), *NewWorld->GetFName().ToString()));
					//UE_LOG(LogTemp, Warning, TEXT("TickInWorld: %s"), *NewWorld->GetFName().ToString());
				}

			}
		}});
	if (TickHandle.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("TickHackSet")));
		UE_LOG(LogTemp, Warning, TEXT("TickHackSet"));
	}
	TickHandles.Add(NewWorld, TickHandle);


	//auto CHandle = FWorldDelegates::OnPostWorldCleanup.AddLambda([NewWorld](UWorld* World, bool bSessionEnded, bool bCleanupResources) {
	//	if (!CreatedWorlds.Contains(World)) {
	//		;
	//		auto CLHandle = CleanupHandles.Find(NewWorld);
	//		if (CLHandle) {
	//			FWorldDelegates::OnPostWorldCleanup.Remove(*CLHandle);
	//			CLHandle->Reset();
	//			CleanupHandles.Remove(NewWorld);
	//		}
	//		auto TickHand = TickHandles.Find(NewWorld);
	//		if (TickHand && TickHand->IsValid()) {
	//			FWorldDelegates::OnWorldTickStart.Remove(*TickHand);
	//			TickHand->Reset();
	//			TickHandles.Remove(NewWorld);
	//		}


	//		if (NewWorld) {
	//			auto Name = NewWorld->GetMapName();
	//			for (FActorIterator ActorIt(NewWorld); ActorIt; ++ActorIt)
	//			{
	//				//ActorIt->RouteEndPlay(EEndPlayReason::LevelTransition);
	//				ActorIt->Destroy();
	//			}

	//			// Do this after destroying pawns/playercontrollers, in case that spawns new things (e.g. dropped weapons)
	//			//WorldRef->CleanupWorld();

	//			//GEngine->WorldDestroyed(WorldRef);

	//			CreatedWorlds.Remove(NewWorld);

	//			GEngine->DestroyWorldContext(NewWorld);
	//			NewWorld->DestroyWorld(true);

	//			//GEngine->TrimMemory();
	//			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Created World destroyed: %s"), *WorldRef->GetFName().ToString()));
	//			UE_LOG(LogTemp, Warning, TEXT("Created World destroyed: %s"), *Name);

	//		}


	//		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("DestroyedWorld: %s"), *World->GetFName().ToString()));
	//		UE_LOG(LogTemp, Warning, TEXT("DestroyedWorld: %s"), *World->GetFName().ToString());
	//	}
	//	});
	//CleanupHandles.Add(NewWorld, CHandle);
	//}


	CreatedWorlds.Add(NewWorld);

	UWorldContext.SetWorldContext(&WorldContext);
	//World = NewWorld;
	//
	//
	//auto vp = World->GetGameViewport();
	//if (vp) {
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Exist viewport")));
	//	//World->GetGameViewport()->Init(WorldContext, GameInstance);
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player: %d"), World->GetAuthGameMode()->GetNumPlayers()));
	//
	//}
	//else
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("No viewport")));
	//}

	//GEngine->GameViewport->Viewport->;
	//FViewportClient* Client = GEngine->GameViewport->Viewport->GetClient();
	//Client->
	//FViewport::SetViewportClient;

}


void UUWorldSubsystem::DestroyCreatedWorld(FUWorldContext UWorldContext) {

	//auto WorldRef = Cast<UWorld>(World->GetWorld());
	auto WorldRef = UWorldContext.ContextWorld;

	//WorldRef->SetShouldTick(false);

	if (CreatedWorlds.Contains(WorldRef)) {
		/*auto CleanDelegat = CleanupHandles.Find(WorldRef);
		if (CleanDelegat->IsValid()) {
			FWorldDelegates::OnPostWorldCleanup.Remove(*CleanDelegat);
			CleanDelegat->Reset();
		}*/
		auto TickHand = TickHandles.Find(WorldRef);
		if (TickHand && TickHand->IsValid()) {
			FWorldDelegates::OnWorldTickStart.Remove(*TickHand);
			TickHand->Reset();
			TickHandles.Remove(WorldRef);
		}
		CreatedWorlds.Remove(WorldRef);
	}

	//if (WorldRef) {
	//	auto WorldName = WorldRef->GetMapName();
	//	WorldRef->DestroyWorld(true);

	//	WorldRef->MarkAsGarbage();
	//	UE_LOG(LogTemp, Warning, TEXT("Destroy UWorld from player event: %s"), *WorldName);
	//	}

	if (WorldRef)
	{
		auto WorldName = WorldRef->GetMapName();
		for (FActorIterator ActorIt(WorldRef); ActorIt; ++ActorIt)
		{

			//ActorIt->RouteEndPlay(EEndPlayReason::LevelTransition);
			ActorIt->Destroy();
		}

		// Do this after destroying pawns/playercontrollers, in case that spawns new things (e.g. dropped weapons)
		//WorldRef->CleanupWorld();

		//WorldRef->MarkAsGarbage();


		WorldRef->DestroyWorld(true);
		GEngine->DestroyWorldContext(WorldRef);
		GEngine->WorldDestroyed(WorldRef);


		//GEngine->TrimMemory();
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Created World destroyed: %s"), *WorldRef->GetFName().ToString()));
		UE_LOG(LogTemp, Warning, TEXT("Destroy UWorld from player event: %s"), *WorldName);
	}

}


void UUWorldSubsystem::SetWorldTick(FUWorldContext UWorldContext, bool Tick) {
	auto contest = UWorldContext.ContextWorld;
		if(IsValid(contest)) contest->SetShouldTick(Tick);
}



TArray<FUWorldContext> UUWorldSubsystem::GetAllCreatedWorlds()
{
	TArray<FUWorldContext> ContextArray;
	for(auto world : CreatedWorlds) {
		FUWorldContext ct;
		ct.ContextWorld = world;
		ContextArray.Add(ct);
	}
	return ContextArray;
}

APlayerController* UUWorldSubsystem::ChangeWorld(FUWorldContext UWorldContext, APawn*& PrevPawn) {

	auto MainWorld = GetMainWorldContext().ContextWorld;
	auto NewWorld = UWorldContext.ContextWorld;
	if (!IsValid(NewWorld)) return nullptr;
	auto WorldContext = GEngine->GetWorldContextFromWorld(NewWorld);

	auto GameInstance = UGameplayStatics::GetGameInstance(GEngine->GetCurrentPlayWorld());
	//auto context = GameInstance->GetWorldContext()->GameViewport;

	//switch render to new UWorld
	//NewWorld->GetGameViewport()->Init(*WorldContext, GameInstance);
	Cast<UUWorldCommonGameViewportClient>(NewWorld->GetGameViewport())->ChangeRenderWorld(NewWorld);

	//find PC in worlds
	APlayerController* PC = nullptr;

	//if (MainWorld != NewWorld && !IsValid(NewWorld->GetFirstPlayerController())) {
		NewWorld->AddController(GEngine->GetCurrentPlayWorld()->GetFirstPlayerController());
	//}

	//if (MainWorld == NewWorld) {
	//	auto ctr = NewWorld->GetFirstPlayerController();
	//	NewWorld->RemoveController(ctr);
	//}
	ActiveWorldContext = UWorldContext;
	//auto World = GetMainWorld();
	//if (IsValid(World)) {

	//	PC = World->GetFirstPlayerController();

	//	if (!PC) {
	//		for (auto CurWorld : CreatedWorlds)
	//		{
	//			PC = CurWorld->GetFirstPlayerController();
	//			if (PC) break;
	//		}
	//	}


	//	if (PC) PrevPawn = PC->GetPawn();
	//}

	return PC ? PC : nullptr;
}

FUWorldContext UUWorldSubsystem::GetMainWorldContext()
{
	FUWorldContext UWorldContext;
	UWorldContext.ContextWorld = GEngine->GetCurrentPlayWorld();
	return UWorldContext;
}


FUWorldContext UUWorldSubsystem::GetCurrentWorldContext()
{
	return ActiveWorldContext;
}
