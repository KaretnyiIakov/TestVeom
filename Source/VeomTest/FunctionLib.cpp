// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLib.h"

void UFunctionLib::ClearTask(FTask& Task)
{
	Task._firstStorage = nullptr;
	Task._secondStorage = nullptr;
	Task._resourceCount = 0;
	Task._resourceType = 0;
}
