#pragma once

class LUAPROVIDER_API FJavaDataAccepter
{
public:
	FJavaDataAccepter() {};
	~FJavaDataAccepter() {};
public:
	static TSharedPtr<FJavaDataAccepter> JavaDataAccepterIns;
public:
	bool GetDownloadLengthStatistics(int& total, int&download);
	bool GetJsCmdInQueue(FString& s,bool pop = true);
	void JsCmdEnqueue(const FString& s);
	void AddToDownloadMap(const int& tid, const int& block_total, const int& block_downloaded);
#if PLATFORM_ANDROID
	static void UpdateAPK(const FString& apk_url, const FString& confirm_msg);
#endif
private:
	FCriticalSection JavaVmCritical;
	TQueue<FString> JsCmdCacheQueue; //cache command from javascript
	TMap<int, TPair<int, int>> DownloadMap;//first is thread id,current download length from java
};

