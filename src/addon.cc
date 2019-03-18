#include <nan.h>
#include <windows.h>
using namespace Nan;
#define VALUE_MAX 32767
#define DATA_MAX (1024 * 1024)

HKEY openKey(const FunctionCallbackInfo<v8::Value> &info, REGSAM access) {
  HKEY ret = 0;
  auto root = (HKEY)info[0]->IntegerValue();
  auto path = (LPCWSTR)*v8::String::Value(info.GetIsolate(), info[1]);

  LSTATUS error;
  if ((error = RegOpenKeyExW(root, path, 0, access, &ret)) != ERROR_SUCCESS) {
    return 0;
  }
  return ret;
}

static WCHAR name[VALUE_MAX];
static BYTE data[DATA_MAX];

NAN_METHOD(getKey) {
  auto key = openKey(info, KEY_READ);
  if (!key) {
    return;
  }

  DWORD index = 0;
  DWORD valueType;
  DWORD nameLength;
  DWORD dataLength;

  auto ret = New<v8::Array>();
  info.GetReturnValue().Set(ret);

  LSTATUS error;

  while (TRUE) {
    nameLength = VALUE_MAX - 1;
    dataLength = DATA_MAX - 1;
    if ((error = RegEnumValueW(key, index, (LPWSTR)&name, &nameLength, NULL, &valueType, (LPBYTE)&data, &dataLength)) != ERROR_SUCCESS) {
      if (error == ERROR_NO_MORE_ITEMS) {
        break;
      }
      info.GetReturnValue().Set(Null());
      break;
    }
    auto obj = New<v8::Object>();
    auto jsName = New(reinterpret_cast<uint16_t*>(name)).ToLocalChecked();
    Set(obj, New("name").ToLocalChecked(), jsName);
    Set(obj, New("type").ToLocalChecked(), New((uint32_t)valueType));
    if (valueType == REG_SZ || valueType == REG_EXPAND_SZ) {
      data[dataLength] = 0;
      Set(obj, New("value").ToLocalChecked(), New(reinterpret_cast<uint16_t*>(data)).ToLocalChecked());
    }
    if (valueType == REG_DWORD) {
      Set(obj, New("value").ToLocalChecked(), New((uint32_t)data[0]));
    }
    if (valueType == REG_BINARY) {
      auto val = New<v8::Array>();
      for (DWORD i = 0; i < dataLength; i++) {
        Set(val, i, New((uint32_t)data[i]));
      }
      Set(obj, New("value").ToLocalChecked(), val);
    }
    Set(ret, index++, obj);
  }

  if (key) {
    RegCloseKey(key);
  }
}

NAN_METHOD(setValue) {
  auto key = openKey(info, KEY_WRITE);
  if (!key) {
    return;
  }

  auto valueType = (DWORD)info[2]->IntegerValue();
  std::wstring name((wchar_t*)*v8::String::Value(info.GetIsolate(), info[3]));
  DWORD dataLength = 0;

  if (valueType == REG_SZ || valueType == REG_EXPAND_SZ) {
    auto value = (LPCWSTR)*v8::String::Value(info.GetIsolate(), info[4]);
    wcscpy((wchar_t*)data, value);
    dataLength = wcslen(value) * 2 + 2;
    data[dataLength] = 0;
  }
  if (valueType == REG_DWORD) {
    *((DWORD*)&data) = (DWORD)info[4]->IntegerValue();
  }

  LSTATUS error;
  if ((error = RegSetValueExW(key, (LPCWSTR)name.c_str(), NULL, valueType, (LPBYTE)&data, dataLength)) != ERROR_SUCCESS) {
    info.GetReturnValue().Set((uint32_t)error);
    return;
  }

  RegCloseKey(key);
}

NAN_METHOD(listSubkeys) {
  auto key = openKey(info, KEY_ENUMERATE_SUB_KEYS);
  if (!key) {
    return;
  }

  DWORD index = 0;
  DWORD nameLength = VALUE_MAX - 1;

  auto ret = New<v8::Array>();
  info.GetReturnValue().Set(ret);

  LSTATUS error;

  while (TRUE) {
    if ((error = RegEnumKeyW(key, index, (LPWSTR)&name, nameLength)) != ERROR_SUCCESS) {
      if (error == ERROR_NO_MORE_ITEMS) {
        break;
      }
      info.GetReturnValue().Set(Null());
      return;
    }

    Set(ret, index++, New(reinterpret_cast<uint16_t*>(name)).ToLocalChecked());
  }

  if (key) {
    RegCloseKey(key);
  }
}

NAN_METHOD(createKey) {
  HKEY key = 0;
  auto root = (HKEY)info[0]->IntegerValue();
  auto path = (LPCWSTR)*v8::String::Value(info.GetIsolate(), info[1]);

  LSTATUS error;
  if ((error = RegCreateKeyExW(root, path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL)) != ERROR_SUCCESS) {
    return;
  }

  if (key) {
    RegCloseKey(key);
  }
}

NAN_METHOD(deleteKey) {
  auto root = (HKEY)info[0]->IntegerValue();
  auto path = (LPCWSTR)*v8::String::Value(info.GetIsolate(), info[1]);
  RegDeleteTreeW(root, path);
  RegDeleteKeyW(root, path);
}

NAN_MODULE_INIT(Init) {
  Export(target, "getKey", getKey);
  Export(target, "setValue", setValue);
  Export(target, "listSubkeys", listSubkeys);
  Export(target, "createKey", createKey);
  Export(target, "deleteKey", deleteKey);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
