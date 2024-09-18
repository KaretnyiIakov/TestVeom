// Fill out your copyright notice in the Description page of Project Settings.


#include "Storage.h"

// Sets default values
AStorage::AStorage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStorage::BeginPlay()
{
	Super::BeginPlay();
	
}

int AStorage::GetLimit()
{
	switch (_resourceLimit)
	{
		case ELimit::Zero: return 0;
		case ELimit::Fifty: return 50;
		case ELimit::OneHundred: return 100;
		case ELimit::TwoHundredFifty: return 250;
		case ELimit::FiveHundred: return 500;
	}
	return 0;
}

void AStorage::SetRandLimit()
{
}

void AStorage::SetRandResourceType()
{
}

void AStorage::SetRandResourceCount()
{
}

void AStorage::GiveResourceToLoader(int32 Count)
{
	_resourceCount = _resourceCount - Count;
	_reservedResourceCount = _reservedResourceCount + Count;
}

void AStorage::GetResourceToLoader(int32 Count)
{
	_resourceCount = _resourceCount + Count;
	_reservedResourceCount = _reservedResourceCount - Count;
}

void AStorage::OnBeTaskAim(bool Add, FString LoaderName, bool WantToTake, int32 TaskResourceCount)
{
	_taskCounter = (Add ? 1 : -1) + _taskCounter;

	if (Add)
		(WantToTake ? _loadersWhoWantToPut : _loadersWhoWantToTake).Add(LoaderName, TaskResourceCount);
	else
		(WantToTake ? _loadersWhoWantToPut : _loadersWhoWantToTake).Remove(LoaderName);
}

void AStorage::RegisterInManager()
{

}

// Called every frame
void AStorage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

