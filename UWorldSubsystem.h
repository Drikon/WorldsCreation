// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UWorldSubsystem.generated.h"




USTRUCT(BlueprintType)
struct FUWorldContext
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Variables")
		UWorld* ContextWorld = nullptr;


public:
	void SetWorldContext(FWorldContext* inContext) { ContextWorld = inContext->World(); }


};



/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UUWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	//UUWorldSubsystem();

	TMap<UWorld*, FDelegateHandle> CleanupHandles;
	TMap<UWorld*, FDelegateHandle> TickHandles;
	static TArray<UWorld*> CreatedWorlds;


	FDelegateHandle Lambda;


	FUWorldContext ActiveWorldContext;

public:

	// USubsystem implementation Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// USubsystem implementation End



public:

	UFUNCTION(BlueprintCallable, Category = "UWorldSubsystem")
		void CreateNewWorld(const TSoftObjectPtr<class UWorld> Level, const TSubclassOf<class AGameModeBase> GameMode, FUWorldContext& UWorldContext);
	UFUNCTION(BlueprintCallable, Category = "UWorldSubsystem")
		void DestroyCreatedWorld(FUWorldContext UWorldContext);
	UFUNCTION(BlueprintCallable, Category = "UWorldSubsystem")
		static void SetWorldTick(FUWorldContext UWorldContext, bool Tick);

	UFUNCTION(BlueprintCallable, Category = "UWorldSubsystem")
		TArray<FUWorldContext> GetAllCreatedWorlds();


	UFUNCTION(BlueprintCallable, Category = "UWorldSubsystem")
		APlayerController* ChangeWorld(FUWorldContext UWorldContext, APawn*& PrevPawn);


	UFUNCTION(BlueprintPure, Category = "UWorldSubsystem")
		FUWorldContext GetMainWorldContext();
	UFUNCTION(BlueprintPure, Category = "UWorldSubsystem")
		FUWorldContext GetCurrentWorldContext();

};
