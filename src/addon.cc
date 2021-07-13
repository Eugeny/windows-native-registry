#include <napi.h>
#include <uv.h>
#include <windows.h>
using namespace Napi;
#define VALUE_MAX 32767
#define DATA_MAX (1024 * 1024)

HKEY openKey(const Napi::CallbackInfo& info, REGSAM access) {
  HKEY ret = 0;
  auto root = (HKEY)(int64_t)info[0].ToNumber();
  auto path = info[1].ToString().Utf16Value();

  LSTATUS error;
  if ((error = RegOpenKeyExW(root, (LPCWSTR)path.c_str(), 0, access, &ret)) != ERROR_SUCCESS) {
    return 0;
  }
  return ret;
}

static WCHAR name[VALUE_MAX];
static BYTE data[DATA_MAX];

Napi::Value getKey(const Napi::CallbackInfo& info) {
  auto env = info.Env();

  auto key = openKey(info, KEY_READ);
  if (!key) {
    return info.Env().Null();
  }

  DWORD index = 0;
  DWORD valueType;
  DWORD nameLength;
  DWORD dataLength;

  auto ret = Array::New(env);

  LSTATUS error;

  while (TRUE) {
    nameLength = VALUE_MAX - 1;
    dataLength = DATA_MAX - 1;
    if ((error = RegEnumValueW(key, index, (LPWSTR)&name, &nameLength, NULL, &valueType, (LPBYTE)&data, &dataLength)) != ERROR_SUCCESS) {
      if (error == ERROR_NO_MORE_ITEMS) {
        break;
      }
      return info.Env().Null();
    }
    auto obj = Object::New(env);
    auto jsName = String::New(env, reinterpret_cast<char16_t*>(name));
    obj.Set("name", jsName);
    obj.Set("type", Number::New(env, (uint32_t)valueType));
    if (valueType == REG_SZ || valueType == REG_EXPAND_SZ) {
      data[dataLength] = 0;
      data[dataLength + 1] = 0;
      obj.Set("value", String::New(env, reinterpret_cast<char16_t*>(data)));
    }
    if (valueType == REG_DWORD) {
      obj.Set("value", Number::New(env, *(uint32_t*)&data[0]));
    }
    if (valueType == REG_BINARY) {
      auto val = Array::New(env);
      for (DWORD i = 0; i < dataLength; i++) {
        val.Set(i, Number::New(env, (uint32_t)data[i]));
      }
      obj.Set("value", val);
    }
    ret.Set(index++, obj);
  }

  if (key) {
    RegCloseKey(key);
  }

  return ret;
}

Napi::Value setValue(const Napi::CallbackInfo& info) {
  auto env = info.Env();
  auto key = openKey(info, KEY_WRITE);
  if (!key) {
    return env.Null();
  }

  auto valueType = (DWORD)(int64_t)info[2].ToNumber();
  DWORD dataLength = 0;

  if (valueType == REG_SZ || valueType == REG_EXPAND_SZ) {
    auto value = info[4].ToString().Utf16Value();
    wcscpy((wchar_t*)data, (LPCWSTR)value.c_str());
    dataLength = value.length() * 2 + 2;
    data[dataLength] = 0;
  }
  if (valueType == REG_DWORD) {
    *((DWORD*)&data) = (DWORD)(int64_t)info[4].ToNumber();
    dataLength = 4;
  }

  auto name = info[3].ToString().Utf16Value();

  LSTATUS error;
  if ((error = RegSetValueExW(key, (LPCWSTR)name.c_str(), NULL, valueType, (LPBYTE)&data, dataLength)) != ERROR_SUCCESS) {
    RegCloseKey(key);
    return Number::New(env, (uint32_t)error);
  }

  RegCloseKey(key);
  return env.Null();
}

Napi::Value listSubkeys(const Napi::CallbackInfo& info) {
  auto env = info.Env();
  auto key = openKey(info, KEY_ENUMERATE_SUB_KEYS);
  if (!key) {
    return env.Null();
  }

  DWORD index = 0;
  DWORD nameLength = VALUE_MAX - 1;

  auto ret = Array::New(env);

  LSTATUS error;

  while (TRUE) {
    if ((error = RegEnumKeyW(key, index, (LPWSTR)&name, nameLength)) != ERROR_SUCCESS) {
      if (error == ERROR_NO_MORE_ITEMS) {
        break;
      }
      RegCloseKey(key);
      return Number::New(env, (uint32_t)error);
    }

    ret[index++] = String::New(env, reinterpret_cast<char16_t*>(name));
  }

  RegCloseKey(key);
  return ret;
}

Napi::Value createKey(const Napi::CallbackInfo& info) {
  HKEY key = 0;
  auto root = (HKEY)(int64_t)info[0].ToNumber();
  auto path = (LPCWSTR)info[1].ToString().Utf16Value().c_str();

  LSTATUS error;
  if ((error = RegCreateKeyExW(root, path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL)) != ERROR_SUCCESS) {
    return Number::New(info.Env(), error);
  }

  if (key) {
    RegCloseKey(key);
  }
  return info.Env().Null();
}

Napi::Value deleteKey(const Napi::CallbackInfo& info) {
  auto root = (HKEY)(int64_t)info[0].ToNumber();
  auto path = (LPCWSTR)info[1].ToString().Utf16Value().c_str();
  RegDeleteTreeW(root, path);
  RegDeleteKeyW(root, path);
  return info.Env().Null();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("getKey", Napi::Function::New(env, getKey));
  exports.Set("setValue", Napi::Function::New(env, setValue));
  exports.Set("listSubkeys", Napi::Function::New(env, listSubkeys));
  exports.Set("createKey", Napi::Function::New(env, createKey));
  exports.Set("deleteKey", Napi::Function::New(env, deleteKey));
  return exports;
}

NODE_API_MODULE(addon, Init)
