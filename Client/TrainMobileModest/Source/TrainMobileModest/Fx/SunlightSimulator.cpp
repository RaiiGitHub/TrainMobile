// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "SunlightController.h"
#include "SunlightSimulator.h"

SunlightSimulator::SunlightSimulator()
	: Simulating(false)
	, SimulateCount(-1)
	, SimulateCurIndex(0)
	, SunHeightCurPercent(0.f)
	, SunHeightPercentPerSecond(0.1f)
	, SunHeightPercentSystem(0.f)
	, SunlightController(nullptr)
{

}

SunlightSimulator::~SunlightSimulator()
{
}

void SunlightSimulator::SetSunlightController(ASunlightController * sc)
{
	SunlightController = sc;
}

void SunlightSimulator::SetSimulateSpeed(float percentPerSecond)
{
	SunHeightPercentPerSecond = percentPerSecond;
}

void SunlightSimulator::SetSimulateCount(int count)
{
	SimulateCount = count;
}

void SunlightSimulator::SetSimulateSunHeightPercent(float percent)
{
	//take effect.
	if (SunlightController)
	{
		SunlightController->SetSunHeight(percent);
	}
}

void SunlightSimulator::SetSystemSunHeightPercent(float percent)
{
	SunHeightPercentSystem = percent;
}

void SunlightSimulator::Begin()
{
	if (Simulating)
		return;
	Simulating = true;
}

void SunlightSimulator::Pause()
{
	Simulating = false;
}

void SunlightSimulator::Reset()
{
	Simulating = false;
	SimulateCurIndex = 0;
	SunHeightCurPercent = 0.f;
}

void SunlightSimulator::Tick(float DeltaTime)
{
	if (nullptr == SunlightController || !Simulating)
		return;
	float deltaPercent = DeltaTime * SunHeightPercentPerSecond;
	SunHeightCurPercent += deltaPercent;
	if (SunHeightCurPercent >= 1.0)
	{
		SunHeightCurPercent = 0.f;
		if (SimulateCount > 0)
		{
			SimulateCurIndex++;
			if (SimulateCurIndex >= SimulateCount)
			{
				Reset();
				SunlightController->SetSunHeight(SunHeightPercentSystem);
				return;//reach max simulatetime,finished simulating and restore the sun height.
			}
		}
	}

	//take effect.
	SunlightController->SetSunHeight(SunHeightCurPercent);
}
