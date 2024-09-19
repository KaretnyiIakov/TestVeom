// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Storage.generated.h"

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
	static int32 _storageCounter;
public:	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 _resourceType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 _resourceCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 _reservedResourceCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource", meta = (ExposeOnSpawn = true))
	ELimit _resourceLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tasks")
	int32 _taskCounter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tasks")
	TMap<FString, int32> _loadersWhoWantToPut;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tasks")
	TMap<FString, int32> _loadersWhoWantToTake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager", meta = (ExposeOnSpawn = true))
	bool _inSystem;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLoaderInteraction OnLoaderInteraction;

	UFUNCTION(BlueprintPure, Category = "Resource")
	int GetLimit() const;
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

	UFUNCTION(BlueprintCallable, Category = "Task")
	void OnBeTaskAim(bool Add, FString LoaderName, bool WantToTake, int32 TaskResourceCount);
	UFUNCTION(BlueprintCallable, Category = "Task")
	void TaskCounterUpdate(bool Up);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	static void IncrementCounter();
	void RenameStorage();
};
