#include "LuaProviderPrivatePCH.h"
#include "JavaDataAccepter.h"
#include "Misc/Paths.h"
#include "LuaMachine.h"
#if PLATFORM_ANDROID
#include "Runtime/Launch/Public/Android/AndroidJNI.h"
#include "Runtime/Core/Public/Android/AndroidApplication.h"
#endif
TSharedPtr<FJavaDataAccepter> FJavaDataAccepter::JavaDataAccepterIns = MakeShareable<FJavaDataAccepter>(new FJavaDataAccepter);

#if PLATFORM_ANDROID


//This function is declared in the Java-defined class, GameActivity.java: "public native void nativeSetInnerPath(String PathName,String CPathName);"
extern "C" void Java_com_epicgames_ue4_GameActivity_nativeSetInnerPath(JNIEnv* jenv, jobject thiz, jstring PathName, jstring CPathName )
{
	const char* JavaPathChars = jenv->GetStringUTFChars(PathName, 0);
	const char* JavaCPathChars = jenv->GetStringUTFChars(CPathName, 0);
	FLuaMachine::LuaSearchPath = UTF8_TO_TCHAR(JavaPathChars);
	FLuaMachine::LuaSearchCPath = UTF8_TO_TCHAR(JavaCPathChars);
	//Release the strings
	jenv->ReleaseStringUTFChars(PathName, JavaPathChars);
	jenv->ReleaseStringUTFChars(CPathName, JavaCPathChars);
}

extern "C" void Java_com_epicgames_ue4_WebViewControl_00024ChromeClient_onJsSendMessageToCpp(JNIEnv* JEnv, jobject thiz, jstring Message)
{
	const char* jsMsg = JEnv->GetStringUTFChars(Message, 0);
	FString msg(UTF8_TO_TCHAR(jsMsg));
	UE_LOG(LuaProviderPlugin, Log, TEXT("JS TO CPP,HERE'S THE CONTENT:%s"), *msg);
	FJavaDataAccepter::JavaDataAccepterIns->JsCmdEnqueue(msg);
	JEnv->ReleaseStringUTFChars(Message, jsMsg);
	
}
extern "C" void Java_com_epicgames_ue4_GameActivity_nativeDownloadPercent(JNIEnv* jenv, jobject thiz, jint threadid, jint blocktotal, jint blockdownloaded)
{
	FJavaDataAccepter::JavaDataAccepterIns->AddToDownloadMap(threadid, blocktotal, blockdownloaded);
}
static jmethodID AndroidThunkJava_DownloadAndInstallAPK;
void AndroidThunkCpp_DownloadAndInstallAPK(const FString& APKUrl,const FString& Msg)
{
	static bool find = false;
	if (!find)
	{
		AndroidThunkJava_DownloadAndInstallAPK = FJavaWrapper::FindMethod(FAndroidApplication::GetJavaEnv(), FJavaWrapper::GameActivityClassID, "AndroidThunkJava_DownloadAndInstallAPK", "(Ljava/lang/String;Ljava/lang/String;)V", false);
		find = true;
	}
	FString Result = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring apk_url = Env->NewStringUTF(TCHAR_TO_UTF8(*APKUrl));
		jstring confirm_msg = Env->NewStringUTF(TCHAR_TO_UTF8(*Msg));
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AndroidThunkJava_DownloadAndInstallAPK, apk_url, confirm_msg);
		Env->DeleteLocalRef(apk_url);
		Env->DeleteLocalRef(confirm_msg);
	}
}
#endif

bool FJavaDataAccepter::GetDownloadLengthStatistics(int & total, int & download)
{
	total = 0;
	download = 0;
	for (auto& d : DownloadMap)
	{
		TPair<int, int>& v = d.Value;
		total += v.Key;
		download += v.Value;
	}
	return DownloadMap.Num() > 0 ;
}

bool FJavaDataAccepter::GetJsCmdInQueue(FString & s, bool pop)
{
	FScopeLock Lock(&JavaVmCritical);
	return pop ?JsCmdCacheQueue.Dequeue(s): JsCmdCacheQueue.Peek(s);
}

void FJavaDataAccepter::JsCmdEnqueue(const FString & s)
{
	FScopeLock Lock(&JavaVmCritical);
	JsCmdCacheQueue.Enqueue(s);
}

void FJavaDataAccepter::AddToDownloadMap(const int & tid, const int & block_total, const int & block_downloaded)
{
	DownloadMap.FindOrAdd(tid) = TPairInitializer<int, int>(block_total, block_downloaded);
}

#if PLATFORM_ANDROID
void FJavaDataAccepter::UpdateAPK(const FString & apk_url, const FString & confirm_msg)
{
	AndroidThunkCpp_DownloadAndInstallAPK(apk_url, confirm_msg);
}
#endif