// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Storage.h"
#include "FunctionLib.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ESpeed : uint8
{
    Up UMETA(DisplayName = "Up"),
    Down UMETA(DisplayName = "Down"),
    Normal UMETA(DisplayName = "Normal")
};


USTRUCT(BlueprintType)
struct FTask
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
    int32 _resourceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
    int32 _resourceCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
    AStorage* _firstStorage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
    AStorage* _secondStorage;

    FTask()
        : _firstStorage(nullptr)
        , _secondStorage(nullptr)
    {}

};
USTRUCT(BlueprintType)
struct FStorages
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storages")
    TArray<AStorage*> _storages;
};

UCLASS()
class VEOMTEST_API UFunctionLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    static void ClearTask(FTask& Task);
};
