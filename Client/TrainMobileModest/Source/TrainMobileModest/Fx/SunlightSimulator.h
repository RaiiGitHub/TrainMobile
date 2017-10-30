// As Part of GuangZhou Training.

#pragma once

/**
 * 
 */
class TRAINMOBILEMODEST_API SunlightSimulator
{
public:
	SunlightSimulator();
	~SunlightSimulator();
private:
	//simulate sunlight
	bool  Simulating;
	int   SimulateCount;
	int   SimulateCurIndex;
	float SunHeightCurPercent;
	float SunHeightPercentPerSecond;
	float SunHeightPercentSystem;
	class ASunlightController* SunlightController;
public:
	void SetSunlightController(class ASunlightController* sc);
	void SetSimulateSpeed(float percentPerSecond);
	void SetSimulateCount(int count);//-1 for infinited.
	void SetSimulateSunHeightPercent(float percent);//usually fix setting
	void SetSystemSunHeightPercent(float percent);
	void Begin();
	void Pause();
	void Reset();
	void Tick(float DeltaTime);
};
