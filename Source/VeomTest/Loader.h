// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FunctionLib.h"
#include "Loader.generated.h"

//class ALoader;

//
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
	class UStaticMeshComponent* MeshComponent;

public:	
	// Sets default values for this actor's properties
	ALoader();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	bool _isBusy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	bool _firstStorageIsVisited;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	bool _secondStorageIsVisited;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	FTask _task;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	int32 _speedMulti;


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
	void GetTask(FTask Task);

	UFUNCTION(BlueprintImplementableEvent, Category = "Task")
	void TaskInProgress();

	UFUNCTION(BlueprintCallable, Category = "Task")
	void EndTask();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void OnLoaderSpeedChange(ESpeed Speed);

	
	
};
