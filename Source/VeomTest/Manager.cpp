// Fill out your copyright notice in the Description page of Project Settings.

#include "Manager.h"

#include "VectorTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnitConversion.h"


// Sets default values
AManager::AManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AManager::BeginPlay()
{
	Super::BeginPlay();
	
	SetEmptyLimitsMap();
	SetEmptyResourcesMap();
	SetEmptyStoragesMap();
}

// Called every frame
void AManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AManager::CreateTask(AStorage* FirstStorage, AStorage* SecondStorage, int32 const ResourceType, int32 const ResourceCount)
{
	// Filling in the task structure
	FTask NewTask;
	NewTask._firstStorage = FirstStorage;
	NewTask._secondStorage = SecondStorage;
	NewTask._resourceType = ResourceType;
	NewTask._resourceCount = ResourceCount;

	// Update the job count for related storages
	NewTask._firstStorage->TaskCounterUpdate(true);
	NewTask._secondStorage->TaskCounterUpdate(true);

	// The task either goes straight to the nearest free loader or to the waiting list
	if (FreeLoaders.IsEmpty())
		AwaitingTasks.Add(NewTask);
	else
		GiveTaskForNearestLoader(NewTask);

	// Reserve resource on storages to eliminate counting errors
	FirstStorage->ReservedResourceCount = FirstStorage->ReservedResourceCount + -1 * ResourceCount;
	SecondStorage->ReservedResourceCount = SecondStorage->ReservedResourceCount + ResourceCount;
}

void AManager::CreateTasksOfSelectedResource(int32 ResourceType, TMap<AStorage*, int32> StoragesHaveMoreThenNeed, TMap<AStorage*, int32> StoragesHaveLessThenNeed)
{
	// Filling in arrays for more convenient operation
	TArray<AStorage*> StoragesWithMoreResources;
	StoragesHaveMoreThenNeed.GetKeys(StoragesWithMoreResources);
	TArray<AStorage*> StoragesWithLessResources;
	StoragesHaveLessThenNeed.GetKeys(StoragesWithLessResources);
	TArray<int32> MoreResourcesAmount;
	StoragesHaveMoreThenNeed.GenerateValueArray(MoreResourcesAmount);
	TArray<int32> LessResourcesAmount;
	StoragesHaveLessThenNeed.GenerateValueArray(LessResourcesAmount);
	
	int32 ResourceMoreCounter = 0;
	int32 ResourceLessCounter = 0;
	
	AStorage* CurrentMoreStorage;
	int32 CurrentMoreResourceAmount;
	AStorage* CurrentLessStorage;
	int32 CurrentLessResourceAmount;
	
	TArray<AStorage*> StoragesForDistributionOfRemainder = ResourceStoragesMap.Find(ResourceType)->_storages;;
	int32 ResourceCounter;

	// While there are untapped by counters storages with an overabundance of resources and a lack of resources at the same time 
	while (StoragesHaveMoreThenNeed.Num()-1 >= ResourceMoreCounter && StoragesHaveLessThenNeed.Num()-1 >= ResourceLessCounter)
	{
		CurrentMoreStorage = StoragesWithMoreResources[ResourceMoreCounter];
		CurrentLessStorage = StoragesWithLessResources[ResourceLessCounter];
		CurrentMoreResourceAmount = MoreResourcesAmount[ResourceMoreCounter];
		CurrentLessResourceAmount = LessResourcesAmount[ResourceLessCounter];

		if (CurrentMoreResourceAmount == CurrentLessResourceAmount)
		{
			CreateTask(CurrentMoreStorage, CurrentLessStorage, ResourceType, CurrentMoreResourceAmount);
			ResourceLessCounter++;
			ResourceMoreCounter++;
		}
		else if (CurrentMoreResourceAmount > CurrentLessResourceAmount)
		{
			CreateTask(CurrentMoreStorage, CurrentLessStorage, ResourceType, CurrentLessResourceAmount);
			MoreResourcesAmount[ResourceMoreCounter] = CurrentMoreResourceAmount-CurrentLessResourceAmount;
			ResourceLessCounter++;
		}
		else if (CurrentMoreResourceAmount < CurrentLessResourceAmount)
		{
			CreateTask(CurrentMoreStorage, CurrentLessStorage, ResourceType, CurrentMoreResourceAmount);
			LessResourcesAmount[ResourceLessCounter] = CurrentLessResourceAmount-CurrentMoreResourceAmount;
			ResourceMoreCounter++;
		}
		
	}
	// If overabundance turned out to be more than the shortage and the list with storages with an excessive amount of resources was not passed
	if (StoragesHaveMoreThenNeed.Num()-1 >= ResourceMoreCounter)
	{
		while (StoragesHaveMoreThenNeed.Num()-1 >= ResourceMoreCounter)
		{
			CurrentMoreStorage = StoragesWithMoreResources[ResourceMoreCounter];
			ResourceCounter = MoreResourcesAmount[ResourceMoreCounter];

			// We want to distribute the surplus to neighboring storages with the same resource,
			// so we need to create tasks to transfer "count" of resources to all neighboring storages

			// Use the ratio of the amount of remaining resource to the number of all storages of the same type
			// Use whole part to determine how many resources should be allocated to each neighboring storage
			float ResourceCountForOneStorage_WholePart = UE4::SSE::TruncToInt32((float)MoreResourcesAmount[ResourceMoreCounter] / (float)StoragesForDistributionOfRemainder.Num());
			// Use fractional to carry a little more to several storages if necessary
			float ResourceCountForOneStorage_FractionalPart = FMath::Fmod ((float)MoreResourcesAmount[ResourceMoreCounter], (float)StoragesForDistributionOfRemainder.Num());

			for (AStorage* Storage : StoragesForDistributionOfRemainder)
			{
				if (Storage != CurrentMoreStorage)
				{
					CurrentLessStorage = Storage;
					// if fractional part exists then transfer one more resource or only necessary number
					CurrentLessResourceAmount =
						ResourceCountForOneStorage_FractionalPart  * StoragesForDistributionOfRemainder.Num() + 1 > StoragesForDistributionOfRemainder.Find(Storage) ?
							ResourceCountForOneStorage_WholePart + 1 : ResourceCountForOneStorage_WholePart;
					if(CurrentLessResourceAmount > 0)
					{
						if ( FMath::Max(ResourceCounter - CurrentLessResourceAmount, 0) > 0 )
						{
							ResourceCounter = FMath::Max(ResourceCounter - CurrentLessResourceAmount, 0);
						}
						else
						{
							if (ResourceCounter > 0)
							{
								CurrentLessResourceAmount = ResourceCounter;
								ResourceCounter = 0;
							}
							else
								break;
						}
						CreateTask(CurrentMoreStorage, CurrentLessStorage, ResourceType, CurrentLessResourceAmount);
					}
					else
						break;
				}
			};
			ResourceMoreCounter++;
		}
	}

	if (StoragesHaveLessThenNeed.Num()-1 >= ResourceLessCounter)
	{
		while (StoragesHaveLessThenNeed.Num()-1 >= ResourceLessCounter)
		{
			CurrentLessStorage = StoragesWithLessResources[ResourceLessCounter];
			ResourceCounter = LessResourcesAmount[ResourceLessCounter];
			
			float ResourceCountForOneStorage_WholePath = UE4::SSE::TruncToInt32((float)LessResourcesAmount[ResourceLessCounter] / (float)StoragesForDistributionOfRemainder.Num());
			float ResourceCountForOneStorage_FractionalPath = FMath::Fmod((float)LessResourcesAmount[ResourceLessCounter], (float)StoragesForDistributionOfRemainder.Num());
			for (AStorage* Storage : StoragesForDistributionOfRemainder)
			{
				if (Storage != CurrentLessStorage)
				{
					CurrentMoreStorage = Storage;
					CurrentMoreResourceAmount =
						((ResourceCountForOneStorage_FractionalPath * StoragesForDistributionOfRemainder.Num() + 1) > StoragesForDistributionOfRemainder.Find(Storage) ) ?
							(ResourceCountForOneStorage_WholePath + 1) : ResourceCountForOneStorage_WholePath;
					if(CurrentMoreResourceAmount > 0)
					{
						if ( FMath::Max(ResourceCounter - CurrentMoreResourceAmount, 0) > 0)
						{
							ResourceCounter = FMath::Max(ResourceCounter - CurrentMoreResourceAmount, 0);
						}
						else
						{
							if (ResourceCounter > 0)
							{
								CurrentMoreResourceAmount = ResourceCounter;
								ResourceCounter = 0;
							}
							else
								break;
						}
						CreateTask(CurrentMoreStorage, CurrentLessStorage, ResourceType, CurrentMoreResourceAmount);
					}
					else
						break;
				}
			};
			ResourceLessCounter++;
		}
	}
}

void AManager::GiveTaskForNearestLoader(FTask Task)
{
	if (Task._firstStorage)
	{
		float Distance = 0.0f;
		FindNearestLoader(Task._firstStorage->GetActorLocation(),FreeLoaders,Distance)->StartTask(Task);
	}
	
}
ALoader* AManager::FindNearestLoader(FVector Origin, const TArray<ALoader*>& LoadersToCheck, float& Distance)
{
	ALoader* NearestActor = nullptr;
	float DistanceFromNearestActor = Distance = TNumericLimits<float>::Max();

	for (ALoader* LoaderToCheck : LoadersToCheck)
	{
		if (LoaderToCheck)
		{
			const float DistanceFromActorToCheck = (Origin - LoaderToCheck->GetActorLocation()).SizeSquared();
			if (DistanceFromActorToCheck < DistanceFromNearestActor)
			{
				NearestActor = LoaderToCheck;
				DistanceFromNearestActor = DistanceFromActorToCheck;
			}
		}
	}

	if (NearestActor)
	{
		Distance = FMath::Sqrt(DistanceFromNearestActor);
	}

	return NearestActor;
}

void AManager::RegisterLoader(ALoader* Loader)
{
	Loader->OnBeBusy.AddDynamic(this,&AManager::OnLoaderBeBusy);
	Loader->OnBeFree.AddDynamic(this,&AManager::OnLoaderBeFree);
	OnLoadersSpeedChange.AddDynamic(Loader,&ALoader::OnLoaderSpeedChange);
	FreeLoaders.AddUnique(Loader);
}

void AManager::OnLoaderBeBusy(ALoader* Loader)
{
	FreeLoaders.Remove(Loader);
}

void AManager::OnLoaderBeFree(ALoader* Loader)
{
	if(AwaitingTasks.IsEmpty())
	{
		FreeLoaders.AddUnique(Loader);
	}
	else
	{
		Loader->StartTask(AwaitingTasks[0]);
		AwaitingTasks.RemoveAt(0);
		
	}
}

void AManager::ChangeLoadersSpeed(ESpeed Speed)
{
	OnLoadersSpeedChange.Broadcast(Speed);
}

void AManager::RegisterStorage(AStorage* Storage)
{
	
}

void AManager::UpdateMapsForAllStorages()
{
	TArray<AActor*> Storages;
	UGameplayStatics::GetAllActorsOfClass(this, AStorage::StaticClass(), Storages);
	for (AActor* Storage : Storages)
	{
		if (Cast<AStorage>(Storage)->bInSystem)
		{
			UpdateMapsForStorage(Cast<AStorage>(Storage), true);
		}
	}
}

void AManager::UpdateMapsForStorage(AStorage* CurrentStorage, bool const bShouldBeInSystem)
{
	UpdateResourceStoragesMap(CurrentStorage, bShouldBeInSystem);
	UpdateResourceAmountMap(CurrentStorage, bShouldBeInSystem);
	UpdateResourceLimitMap(CurrentStorage, bShouldBeInSystem);
}

void AManager::UpdateResourceStoragesMap(AStorage* CurrentStorage, bool const bShouldBeInSystem)
{
	if (bShouldBeInSystem)
		ResourceStoragesMap[CurrentStorage->ResourceType]._storages.AddUnique(CurrentStorage);
	else
		ResourceStoragesMap[CurrentStorage->ResourceType]._storages.Remove(CurrentStorage);
}

void AManager::UpdateResourceAmountMap(AStorage* CurrentStorage, bool const bShouldBeInSystem)
{
	int32 multiplier = bShouldBeInSystem ? 1 : -1;
	int32 count = ResourceAmountMap[CurrentStorage->ResourceType] + (CurrentStorage->ResourceCount * multiplier) + CurrentStorage->ReservedResourceCount;
	ResourceAmountMap.Add(CurrentStorage->ResourceType, count);
}

void AManager::UpdateResourceLimitMap(AStorage* CurrentStorage, bool bShouldBeInSystem)
{
	int32 multiplier = bShouldBeInSystem ? 1 : -1;
	int32 count = ResourceLimitMap[CurrentStorage->ResourceType] + (CurrentStorage->GetLimit() * multiplier);
	ResourceLimitMap.Add(CurrentStorage->ResourceType, count);
}

void AManager::SetStorageSystemStatus(AStorage* CurrentStorage, bool bShouldBeInSystem)
{
	// On add storage in system
	if (bShouldBeInSystem)
	{
		OutOfSystemStorages.Remove(CurrentStorage);
		// Then update resources maps by storage info and call resource redistribution
		UpdateMapsForStorage(CurrentStorage, bShouldBeInSystem);
		ManageSelectedResource(CurrentStorage->ResourceType,nullptr);
	}
	// On remove storage from system
	else
	{
		OutOfSystemStorages.AddUnique(CurrentStorage);
		// If the warehouse is being removed from the system, it must first be unloaded
		// Carefully update the Resource Type : Limit map with bool "not in system"
		UpdateResourceLimitMap(CurrentStorage, bShouldBeInSystem);
		// Cache current storage limit and set new limit as zero
 		ELimit BufferLimit = CurrentStorage->ResourceLimit;
		CurrentStorage->ResourceLimit = ELimit::Zero;
		// Calling the resource management function which should empty the storage according to the new limit
		ManageSelectedResource(CurrentStorage->ResourceType, CurrentStorage);
		// Update map by removing storage  from it
		UpdateResourceStoragesMap(CurrentStorage, bShouldBeInSystem);
		// Return cached limit to storage
		CurrentStorage->ResourceLimit = BufferLimit;

		// Storage not in system now, so it can't participate in the allocation of resources cos it excluded from maps
	}
}

void AManager::ChangeResourcesAmountOnStorage(AStorage* CurrentStorage, bool const bIsAddedResource, int32 const ResourceAmount)
{
	if (CurrentStorage->bInSystem)
	{
		// If the storage is already in the system, then carefully update the Resource Type : Amount map
		UpdateResourceAmountMap(CurrentStorage,false);
		CurrentStorage->ResourceCount += ResourceAmount * (bIsAddedResource ? 1 : -1);
		UpdateResourceAmountMap(CurrentStorage,true);
		// Calling the resource management function after changing the amount of a resource
		ManageSelectedResource(CurrentStorage->ResourceType,nullptr);
	}
	else
		CurrentStorage->ResourceCount += ResourceAmount * (bIsAddedResource ? 1 : -1);
}

void AManager::ChangeStorageResourceType(AStorage* CurrentStorage, int32 NewType)
{
	CurrentStorage->ResourceType = NewType;
}

float AManager::FindAveragePercentForResource(int32 ResourceType)
{
	return (float)ResourceAmountMap[ResourceType]/(float)ResourceLimitMap[ResourceType];
}

void AManager::SetEmptyResourcesMap()
{
	for(int i = 1; i <= 50; i++)
	{
		ResourceAmountMap.Add(i,0);
	}
}

void AManager::SetEmptyStoragesMap()
{
	for (int i = 1; i <= 50; i++)
	{
		ResourceStoragesMap.Add(i, FStorages());
	}
}

void AManager::SetEmptyLimitsMap()
{
	for(int i = 1; i <= 50; i++)
	{
		ResourceLimitMap.Add(i,0);
	}
}

void AManager::ManageSelectedResource(const int32 ResourceType, AStorage* IgnoreStorage)
{
	float const Percent =  FindAveragePercentForResource(ResourceType);
	TMap<AStorage*, int32> StoragesHaveMoreThenNeed;
	TMap<AStorage*, int32> StoragesHaveLessThenNeed;

	// For all warehouses with a specific resource type
	for (AStorage* Storage : ResourceStoragesMap[ResourceType]._storages)
	{
		// If the difference between the quantity corresponding to the average percentage and the quantity of the resource in the warehouse is more than 1
		if (!FMath::IsNearlyEqual((float)(Storage->ResourceCount + Storage->ReservedResourceCount),Percent * (float)Storage->GetLimit(), 1.0f))
		{
			// Add the storage to the maps depending on whether they have more or less resources than necessary and the difference in quantity
			if ((float)(Storage->ResourceCount + Storage->ReservedResourceCount) > Percent * (float)Storage->GetLimit())
			{
				StoragesHaveMoreThenNeed.Add(Storage, (int32)abs((float)(Storage->ResourceCount + Storage->ReservedResourceCount)-(Percent * (float)Storage->GetLimit())));
			}
			else
			{
				if(!(IgnoreStorage!=nullptr && Storage==IgnoreStorage))
				{
					StoragesHaveLessThenNeed.Add(Storage, (int32)abs((float)(Storage->ResourceCount + Storage->ReservedResourceCount)-(Percent * (float)Storage->GetLimit())));
				}
			}
				
		}
	}
	// After the separation start creating tasks for resource redistribution
	CreateTasksOfSelectedResource(ResourceType, StoragesHaveMoreThenNeed, StoragesHaveLessThenNeed);
}

