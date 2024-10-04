// Copyright Epic Games, Inc. All Rights Reserved.

#include "VoxelSandbox2GameMode.h"
#include "VoxelSandbox2Character.h"
#include "UObject/ConstructorHelpers.h"

AVoxelSandbox2GameMode::AVoxelSandbox2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
