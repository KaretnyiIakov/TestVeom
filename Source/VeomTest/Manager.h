// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FunctionLib.h"
#include "Storage.h"
#include "Loader.h"
#include "Math/UnrealMathUtility.h"
#include <cmath> 
#include "Manager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadersSpeedChange, ESpeed, Speed);
UCLASS()
class VEOMTEST_API AManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AManager();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Maps")
	TMap<int32, int32> _resource_Count;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Maps")
	TMap<int32, FStorages> _resource_Storage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource Maps")
	TMap<int32, int32> _resource_Limit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	TArray<FTask> _tasks;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
	TArray<ALoader*> _freeLoaders;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
	TArray<AStorage*> _outSystemStorages;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category= "Task")
	void CreateTask(AStorage* FirstStorage, AStorage* SecondStorage, int32 ResourceType, int32 ResourceCount);
	UFUNCTION(BlueprintCallable, Category= "Task")
	void CreateTasksOfSelectedResource(int32 ResourceType, TMap<AStorage*, int32> StoragesHaveMoreThenNeed, TMap<AStorage*, int32> StoragesHaveLessThenNeed );
	UFUNCTION(BlueprintCallable, Category= "Task")
	void GiveTaskForNearestLoader(FTask Task);
	
	UFUNCTION(BlueprintCallable, Category= "Loader")
	ALoader* FindNearestLoader(FVector Origin, const TArray<ALoader*>& LoadersToCheck, float& Distance);
	UFUNCTION(BlueprintCallable, Category= "Loader")
	void RegisterLoader(ALoader* Loader);
	UFUNCTION(BlueprintCallable, Category= "Loader")
	void OnLoaderBeBusy(ALoader* Loader);
	UFUNCTION(BlueprintCallable, Category= "Loader")
	void OnLoaderBeFree(ALoader* Loader);
	UFUNCTION(BlueprintCallable, Category= "Loader")
	void ChangeLoadersSpeed(ESpeed Speed);

	UFUNCTION(BlueprintCallable, Category= "Storage")
	void UpdateMapsForAllStorages();
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void UpdateMapsForStorage(AStorage* CurrentStorage, bool Add);
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void UpdateStorageMap(AStorage* CurrentStorage, bool Add);
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void UpdateResourceCountMap(AStorage* CurrentStorage, bool Add);
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void UpdateResourceLimitMap(AStorage* CurrentStorage, bool Add);
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void SetStorageSystemStatus(AStorage* CurrentStorage, bool Add);
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void AddOrRemoveResourcesFromStorage(AStorage* CurrentStorage, bool Add, int32 ResourceCount);
	UFUNCTION(BlueprintCallable, Category= "Storage")
	void ChangeStorageResourceType(AStorage* CurrentStorage, int32 NewType);
	
	UFUNCTION(BlueprintCallable, Category= "Resource")
	void ManageSelectedResource(int32 ResourceType, AStorage* IgnorStorage);
	UFUNCTION(BlueprintCallable, Category= "Resource")
	float FindAveragePercentForResource(int32 ResourceType);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLoadersSpeedChange OnLoadersSpeedChange;
	
private:
	void SetEmptyResourcesMap();
	void SetEmptyStoragesMap();
	void SetEmptyLimitsMap();
};
