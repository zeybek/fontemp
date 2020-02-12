#include <node.h>
#include <v8.h>
#include <stdio.h>
#include <nan.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

using v8::Array;
using v8::Context;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

using namespace v8;

const char *ToCString(const v8::String::Utf8Value &value)
{
	return *value ? *value : "<string conversion failed>";
}

void activateFonts(const FunctionCallbackInfo<Value> &args)
{
	Isolate *isolate = args.GetIsolate();

	Local<Context> context = isolate->GetCurrentContext();
	Local<Array> fonts = Local<Array>::Cast(args[0]);

	HKEY fontRegKey = nullptr;

	// Create Registry Key
	RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, nullptr, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, nullptr, &fontRegKey, nullptr);

	for (int i = 0; i < fonts->Length(); i++)
	{
		Local<Object> font = fonts->Get(i)->ToObject(context).ToLocalChecked();
		Local<Array> props = font->GetOwnPropertyNames(context).ToLocalChecked();

		v8::String::Utf8Value psnUtf8(font->Get(context, props->Get(0)).ToLocalChecked());
		v8::String::Utf8Value pathUth8(font->Get(context, props->Get(1)).ToLocalChecked());

		wchar_t psn[1024];
		wchar_t path[1024];

		// UTF16 Conversion
		MultiByteToWideChar(CP_UTF8, 0, ToCString(psnUtf8), -1, psn, 1024);
		MultiByteToWideChar(CP_UTF8, 0, ToCString(pathUth8), -1, path, 1024);

		wsprintfW(psn, L"%s (fontemp)\n", psn);

		// Activate Font
		RegSetValueExW(fontRegKey, psn, 0, REG_SZ, (LPBYTE)path, sizeof(wchar_t) * (wcslen(path)));
		AddFontResourceW(path);
	}

	// Change Notification
	PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	RegCloseKey(fontRegKey);

	args.GetReturnValue().Set(true);
}

void deactivateFonts(const FunctionCallbackInfo<Value> &args)
{
	Isolate *isolate = args.GetIsolate();

	Local<Context> context = isolate->GetCurrentContext();
	Local<Array> fonts = Local<Array>::Cast(args[0]);

	HKEY fontRegKey = nullptr;

	RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", NULL, KEY_SET_VALUE, &fontRegKey);

	for (int i = 0; i < fonts->Length(); i++)
	{
		Local<Object> font = fonts->Get(i)->ToObject(context).ToLocalChecked();
		Local<Array> props = font->GetOwnPropertyNames(context).ToLocalChecked();

		v8::String::Utf8Value psnUtf8(font->Get(context, props->Get(0)).ToLocalChecked());
		v8::String::Utf8Value pathUth8(font->Get(context, props->Get(1)).ToLocalChecked());

		wchar_t psn[1024];
		wchar_t path[1024];

		// UTF16 Conversion
		MultiByteToWideChar(CP_UTF8, 0, ToCString(psnUtf8), -1, psn, 1024);
		MultiByteToWideChar(CP_UTF8, 0, ToCString(pathUth8), -1, path, 1024);

		wsprintfW(psn, L"%s (fontemp)\n", psn);

		// Deactivate Font
		RemoveFontResourceW(path);
		RegDeleteValueW(fontRegKey, psn);
	}

	// Change Notification
	PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	RegCloseKey(fontRegKey);

	args.GetReturnValue().Set(true);
}

void init(Local<Object> exports)
{
	NODE_SET_METHOD(exports, "activateFonts", activateFonts);
	NODE_SET_METHOD(exports, "deactivateFonts", deactivateFonts);
}

NODE_MODULE(fontemp, init)
