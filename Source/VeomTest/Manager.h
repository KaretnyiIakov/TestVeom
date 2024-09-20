// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FunctionLib.h"
#include "Storage.h"
#include "Loader.h"
#include "Math/UnrealMathUtility.h"
#include "Manager.generated.h"

// class Manager

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadersSpeedChange, ESpeed, Speed);

UCLASS()
class VEOMTEST_API AManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AManager();
	
	// Maps "Resource Type : Parameter" for the convenience of management
	
	// Resource Type : the total number of resources in the game, for all storages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Maps")
	TMap<int32, int32> ResourceAmountMap;
	// Resource Type : a structure containing all storages related to this type of resources
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Maps")
	TMap<int32, FStorages> ResourceStoragesMap;
	// Resource Type : the total value of the limit for this resource for all storages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Maps")
	TMap<int32, int32> ResourceLimitMap;

	// An array of tasks on the waiting list
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	TArray<FTask> AwaitingTasks;
	// List of available loaders
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
	TArray<ALoader*> FreeLoaders;
	// The list of warehouses that are excluded from the general system of calculation and management of resources
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
	TArray<AStorage*> OutOfSystemStorages;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Creating a specific take-and-carry task with two storages
	UFUNCTION(BlueprintCallable, Category= "Task")
	void CreateTask(AStorage* FirstStorage, AStorage* SecondStorage, int32 ResourceType, int32 ResourceCount);
	// Creating tasks for a specific type of resource
	UFUNCTION(BlueprintCallable, Category= "Task")
	void CreateTasksOfSelectedResource(int32 ResourceType, TMap<AStorage*, int32> StoragesHaveMoreThenNeed, TMap<AStorage*, int32> StoragesHaveLessThenNeed );
	
	UFUNCTION(BlueprintCallable, Category= "Loader")
	ALoader* FindNearestLoader(FVector Origin, const TArray<ALoader*>& LoadersToCheck, float& Distance);
	UFUNCTION(BlueprintCallable, Category= "Task")
	void GiveTaskForNearestLoader(FTask Task);

	// Register Loader - bind loaders events and initially add to free loaders list
	UFUNCTION(BlueprintCallable, Category= "Loader")
	void RegisterLoader(ALoader* Loader);
	UFUNCTION(BlueprintCallable, Category= "Loader")
	void OnLoaderBeBusy(ALoader* Loader);
	UFUNCTION(BlueprintCallable, Category= "Loader")
	void OnLoaderBeFree(ALoader* Loader);
	UFUNCTION(BlueprintCallable, Category= "Loader")
	void ChangeLoadersSpeed(ESpeed Speed);

	// Register Storage - empty now
	UFUNCTION(BlueprintCallable, Category= "Loader")
	void RegisterStorage(AStorage* Storage);

	// Update maps Resource Type : Parameter for all storages, used in initial game
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void UpdateMapsForAllStorages();

	// Update all Resource Type : Parameter maps with a specific storage and use InSystem storage parameter as toggle to add or remove storage info from maps
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void UpdateMapsForStorage(AStorage* CurrentStorage, bool bShouldBeInSystem);
	// Update Resource Type : Storages map with a specific storage
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void UpdateResourceStoragesMap(AStorage* CurrentStorage, bool bShouldBeInSystem);
	// Update Resource Type : Amount map with a specific storage
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void UpdateResourceAmountMap(AStorage* CurrentStorage, bool bShouldBeInSystem);
	// Update Resource Type : Limit map with a specific storage
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void UpdateResourceLimitMap(AStorage* CurrentStorage, bool bShouldBeInSystem);

	// Establish whether the storage is in the management system or not
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void SetStorageSystemStatus(AStorage* CurrentStorage, bool bShouldBeInSystem);
	// Add or remove resources from storage (now call only from UI by user)
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void ChangeResourcesAmountOnStorage(AStorage* CurrentStorage, bool bIsAddedResource, int32 ResourceAmount);
	// Change resource type which stores the storage
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void ChangeStorageResourceType(AStorage* CurrentStorage, int32 NewType);

	// Starts a cycle of checking the number of resources in storages and causes the creation of tasks resources transfer
	UFUNCTION(BlueprintCallable, Category= "Resource")
	void ManageSelectedResource(int32 ResourceType, AStorage* IgnoreStorage);

	// Find the percentage of filling for storages with a specific resource type as the ratio of the total resource limit to its total quantity
	UFUNCTION(BlueprintCallable, Category= "Resource")
	float FindAveragePercentForResource(int32 ResourceType);
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLoadersSpeedChange OnLoadersSpeedChange;
	
private:

	// Creating empty "resource type - parameter" cards for 50 types of resources (performed at the start of the game)
	void SetEmptyResourcesMap();
	void SetEmptyStoragesMap();
	void SetEmptyLimitsMap();
};
