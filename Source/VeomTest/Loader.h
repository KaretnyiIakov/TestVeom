// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FunctionLib.h"
#include "Loader.generated.h"

// class ALoader;

// Delegates for transmitting information about the employment of the loader
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeFree, class ALoader*, LoaderInstance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeBusy, class ALoader*, LoaderInstance);

UCLASS()
class VEOMTEST_API ALoader : public APawn
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	class UFloatingPawnMovement* MovementComponent;
	
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

    // Counter for the number of loaders, used in naming
	static int32 _loaderCounter;
	
public:	
	// Sets default values for this actor's properties
	ALoader();

	// It is necessary for the correct operation of the tasks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	bool bIsBusy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	bool bFirstStorageIsVisited;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	bool bSecondStorageIsVisited;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	FTask LoaderTask;
	
	// Loader speed multiplier for the ability to adjust the speed from the user
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	int32 SpeedMultiplier;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBeFree OnBeFree;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBeBusy OnBeBusy;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Task")
	void StartTask(FTask Task);

	UFUNCTION(BlueprintImplementableEvent, Category = "Task")
	void TaskInProgress();

	UFUNCTION(BlueprintCallable, Category = "Task")
	void EndTask();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void OnLoaderSpeedChange(ESpeed Speed);

private:
	// Functions used for naming loaders (only for cosmetic)
	static void IncrementCounter();
	void RenameLoader();
};
