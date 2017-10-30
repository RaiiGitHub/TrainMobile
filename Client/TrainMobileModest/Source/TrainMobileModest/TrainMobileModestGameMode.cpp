// Fill out your copyright notice in the Description page of Project Settings.

#include "TrainMobileModest.h"
#include "TrainMobileModestGameMode.h"
#include "HumanCharacter.h"
#include "ScenePlayerController.h"
#include "TrainGameInstance.h"
ATrainMobileModestGameMode::ATrainMobileModestGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AScenePlayerController::StaticClass();
	DefaultPawnClass = AHumanCharacter::StaticClass();
}

