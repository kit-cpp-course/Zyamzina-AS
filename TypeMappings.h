#pragma once
#define arrsize(arr) sizeof(arr) / sizeof(arr[0])
namespace catalogize {
	const wchar_t* MUSIC_FILES[] = { L"wav", L"mp3", L"ogg", L"flac", L"alac", L"wma", L"cue", L"mod", L"med", L"midi", L"aac" };
	const wchar_t* IMAGE_FILES[] = { L"jpg", L"jpeg", L"png", L"apng", L"tiff", L"bmp", L"psd", L"cdr", L"webp", L"xps", L"svg", L"gif" };
	const wchar_t* VIDEO_FILES[] = { L"avi", L"mov", L"mkv", L"webm", L"mp4", L"mp2", L"webm", L"veg", L"ae", L"prproj" };
	const wchar_t* DOC_FILES[] = { L"doc", L"docx", L"xls", L"xlsx", L"csv", L"ppt", L"pptx", L"rtf", L"pdf", L"odt", L"txt", L"ods", L"ots", L"sxc", L"stc", L"odp" };
	const wchar_t* TYPE_NAMES[] = { L"Music", L"Images", L"Videos", L"Documents" };
	const int TYPE_LENGTHS[] = { arrsize(MUSIC_FILES), arrsize(IMAGE_FILES), arrsize(VIDEO_FILES), arrsize(DOC_FILES) };
	const wchar_t** TYPES[] = { MUSIC_FILES, IMAGE_FILES, VIDEO_FILES, DOC_FILES };
}


