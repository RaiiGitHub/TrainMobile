// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "Engine/Canvas.h"
#include "CanvasItem.h"
#include "DisplayContent.h"

DECLARE_LOG_CATEGORY_CLASS(RaiiLogTrack, Log, All);
// Sets default values
ADisplayContent::ADisplayContent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ContentPageIndex = 1;
	PageSize = 7;
	bAutoAddTime = true;
	WidgetWidth = 512;
	FontHeight = 16.f;
	SetActorHiddenInGame(true);
}

// Called when the game starts or when spawned
void ADisplayContent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADisplayContent::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ADisplayContent::SetPageSize(int size)
{
	PageSize = size;
}

void ADisplayContent::SetAutoAddTime(bool bAdd)
{
	bAutoAddTime = bAdd;
}

void ADisplayContent::SetWidgetWidth(float width)
{
	WidgetWidth = width;
}

void ADisplayContent::SetFontHeight(float font_height)
{
	FontHeight = font_height;
}

int ADisplayContent::AddContent(const FString & ct)
{
	//添加时，刷新当前页
	FString auto_add_sep;
	if (bAutoAddTime)
	{
		FDateTime msg_time = FDateTime::Now();
		auto_add_sep = FString::Printf(TEXT("%04d-%02d-%02d %02d:%02d:%02d "),
			msg_time.GetYear(), msg_time.GetMonth(), msg_time.GetDay(),
			msg_time.GetHour(), msg_time.GetMinute(), msg_time.GetSecond()) + ct;
	}
	else
	{
		auto_add_sep = ct;
	}
	int end_pos = Content.Add(AutoChangeLine(auto_add_sep,FontHeight,WidgetWidth));
	//pageshould be autoupdated.
	ContentPageIndex = FMath::CeilToInt((float)(end_pos + 1) / (float)PageSize);
	++NewMsgNum;
	//auto refresh...
	NotifyMessageReceived();
	return end_pos;
}

void ADisplayContent::ClearContent()
{	 
	Content.Empty();
}

bool ADisplayContent::PagingContent(int pagingStep)
{
	if (ContentPageIndex + pagingStep <= 0 || PageSize <= 0)
		return false;
	if (pagingStep > 0)
	{
		int len = Content.Num();
		int pageCount = FMath::CeilToInt((float)len / (float)PageSize);
		if (ContentPageIndex + pagingStep > pageCount)
			return false;
	}
	ContentPageIndex += pagingStep;
	return true;
}

FString ADisplayContent::GetContent(int relativeIndex)
{
	//reverse getting.
	//int startIndex = (ContentPageIndex - 1)*PageSize;
	//int contentNum = Content.Num();
	//int reverse_content_index = FMath::Min(startIndex + PageSize, contentNum)- relativeIndex -1;
	//if (reverse_content_index >= Content.Num() || reverse_content_index < 0)
	//	return FString();
	//return Content[reverse_content_index];

	int startIndex = (ContentPageIndex - 1)*PageSize;
	int contentNum = Content.Num();
	int auto_fill_count = PageSize-(FMath::Min(startIndex + PageSize, contentNum) - startIndex);
	startIndex = FMath::Max(0, startIndex - auto_fill_count);
	int content_index = startIndex + relativeIndex;
	if (content_index >= contentNum || content_index < 0)
		return FString();
	return Content[content_index].Replace(TEXT("$"),TEXT(""));
}

void ADisplayContent::MarkAsRead()
{
	NewMsgNum = 0;
}

void ADisplayContent::Reset()
{
	ClearContent();
	ContentPageIndex = 1;
	PageSize = 7;
	NewMsgNum = 0;
}
void ADisplayContent::WriteToLog(const FString & log)
{
	UE_LOG(RaiiLogTrack, Log, TEXT("%s"),*log);
}

FString ADisplayContent::SubStringByCharacterOtherwishAll(const FString & origin, const FString& sep)
{
	int sep_index = origin.Find(sep);
	if (sep_index < 0)
		return origin;
	FString sub = origin.Mid(0, sep_index);
	sub.Append("...");
	return sub;
}

FString ADisplayContent::AutoChangeLine(const FString & origin, const float word_size, const float line_maxwidth)
{
	FString auto_add_sep = origin;
	UFont* ft = GEngine->GetMediumFont();
	float scale = word_size / 16.f * 1.7f;
	//iteratoring to adjust seprating.
	TArray<int> indexInsert;
	int content_len = origin.Len();
	for (int content_index = 1, seg = 0; content_index <= content_len; ++content_index)
	{
		FTextSizingParameters content_iter(ft, scale, scale);
		UCanvas::CanvasStringSize(content_iter, *auto_add_sep.Left(content_index));
		if (content_iter.DrawXL - seg * line_maxwidth >= line_maxwidth)
		{
			indexInsert.Add(content_index - 1);
			seg++;
		}
	}
	for (int sep = 0; sep < indexInsert.Num(); ++sep)
	{
		int insertAt = indexInsert[sep] + sep;
		if (insertAt < auto_add_sep.Len())
			auto_add_sep.InsertAt(insertAt, TEXT("\n"));
	}
	return auto_add_sep;
}

void ADisplayContent::AutoWrapText(FString& out_format, int32& out_height, int32& out_width, const FString & origin, const float line_maxwidth, const FSlateFontInfo& fi)
{
	if (!fi.HasValidFont()
		|| origin.IsEmpty() 
		|| 0 >= line_maxwidth)
		return;
	FString origin_no_r = origin.Replace(TEXT("\r\n"), TEXT("\n"));
	const UFont* font = static_cast<const UFont*>(fi.FontObject);
	out_format.Empty();
	out_height = 0.f;
	out_width = 0.f;
	int content_len = origin_no_r.Len();
	int origin_width = font->GetStringSize(*origin_no_r);
	float cur_width = 0.f;
	const TCHAR sep = '\n';
	const float rate = fi.Size / 10.f;
	for (int i = 0; i < content_len; ++i )
	{
		if (i > 0 && out_format.Right(1)[0] == sep && origin_no_r[i] == sep)
			continue;
		float char_width = 0.f;
		float char_hegiht = 0.f;
		font->GetCharSize(origin_no_r[i], char_width, char_hegiht);
		cur_width += char_width * rate;
		out_format.AppendChar(origin_no_r[i]);
		if (0 == out_height)
			out_height = char_hegiht * rate;
		out_width = FMath::Max((float)out_width, cur_width);
		if (origin_no_r[i] == sep)
		{
			out_height += char_hegiht * rate;
			cur_width = 0.f;
		}
		else if (cur_width >= line_maxwidth && i != content_len - 1)
		{
			out_format.AppendChar(sep);
			out_height += char_hegiht * rate;
			cur_width = 0.f;

		}
	}
}

bool ADisplayContent::AddPswContent(const FString& key, const FString & psw)
{
	bool add_ok = false;
	FString* str = MapPswContent.Find(key);
	if (nullptr == str)
	{
		MapPswContent.Add(key, psw);
		add_ok = true;
	}
	else
	{
		add_ok = false;
		const int len = psw.Len();
		int pre_len = str->Len();
		for (int i = 0; i < pre_len - len; ++i)
			str->RemoveAt(str->Len()-1);
		pre_len = str->Len();
		for (int i = 0; i < len; ++i)
		{
			const FString c = psw.Mid(i, 1);
			if (!c.Equals("*"))
			{
				add_ok = true;
				if (i < pre_len)
				{
					str->RemoveAt(i);
					str->InsertAt(i, c);
				}
				else
					str->Append(c);
			}
		}
	}
	return add_ok;
}

int ADisplayContent::RemovePswContent(const FString & key)
{
	return MapPswContent.Remove(key);
}

void ADisplayContent::ClearPswContent(const FString & key)
{
	MapPswContent.Empty();
}

FString ADisplayContent::GetPswContent(const FString & key, bool encrypt)
{
	FString content;
	const FString* str = MapPswContent.Find(key);
	if (nullptr == str)
		return content;
	if (encrypt)
	{
		int len = str->Len();
		for (int i = 0; i < len; ++i)
			content.Append("*");
	}
	else
		content = *str;
	return content;
}
