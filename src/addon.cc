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
      for (int i = 0; i < dataLength; i++) {
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
      break;
    }

    auto obj = New<v8::Object>();
    auto jsName = New(reinterpret_cast<uint16_t*>(name)).ToLocalChecked();
    Set(ret, index++, jsName);
  }

  if (key) {
    RegCloseKey(key);
  }
}

NAN_MODULE_INIT(Init) {
  Export(target, "getKey", getKey);
  Export(target, "listSubkeys", listSubkeys);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
