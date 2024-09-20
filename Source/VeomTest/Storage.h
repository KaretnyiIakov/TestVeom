// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Storage.generated.h"

// Enum with limit variants
UENUM(BlueprintType)
enum class ELimit : uint8
{

	Zero UMETA(DisplayName = "0"),
	Fifty UMETA(DisplayName = "50"),
	OneHundred UMETA(DisplayName = "100"),
	TwoHundredFifty UMETA(DisplayName = "250"),
	FiveHundred UMETA(DisplayName = "500"),

	Enum_MAX UMETA(Hidden)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoaderInteraction,bool, Add, FString, LoaderName);

UCLASS()
class VEOMTEST_API AStorage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStorage();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* MeshComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Counter for the number of storages, used in naming
	static int32 _storageCounter;
	
public:	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 ResourceType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 ResourceCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 ReservedResourceCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource", meta = (ExposeOnSpawn = true))
	ELimit ResourceLimit;

	// Task counter and loader aim info to visualize in UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tasks")
	int32 TaskCounter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tasks")
	TMap<FString, int32> WantToPutLoaders;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tasks")
	TMap<FString, int32> WantToTakeLoaders;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager", meta = (ExposeOnSpawn = true))
	bool bInSystem;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLoaderInteraction OnLoaderInteraction;

	// Return limit enum property as int value
	UFUNCTION(BlueprintPure, Category = "Resource")
	int GetLimit() const;

	// Set random parameters function, called on start game
	
	UFUNCTION(BlueprintCallable, Category = "Random")
	void SetRandLimit();
	UFUNCTION(BlueprintCallable, Category = "Random")
	void SetRandResourceType();
	UFUNCTION(BlueprintCallable, Category = "Random")
	void SetRandResourceCount();

	UFUNCTION(BlueprintCallable, Category = "LoaderInteraction")
	void GiveResourceToLoader(int32 Count);
	UFUNCTION(BlueprintCallable, Category = "LoaderInteraction")
	void GetResourceFromLoader(int32 Count);

	// Marks the storage as a loader task target
	UFUNCTION(BlueprintCallable, Category = "Task")
	void OnBeTaskAim(bool Add, FString LoaderName, bool WantToTake, int32 TaskResourceCount);
	// Count Task related to this storage
	UFUNCTION(BlueprintCallable, Category = "Task")
	void TaskCounterUpdate(bool Up);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	// Function used for naming storages (only for cosmetic)
	static void IncrementCounter();
	// Function used for naming storages (only for cosmetic)
	void RenameStorage();
};
