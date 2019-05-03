# windows-native-registry

[![Build status](https://ci.appveyor.com/api/projects/status/6c27yfv8vkav1lw0?svg=true)](https://ci.appveyor.com/project/Eugeny/windows-native-registry)

```ts
export interface RegistryValue {
    name: string
    type: REG
    value: any
}

export enum HK {
    CR = 0x80000000,
    CU = 0x80000001,
    LM = 0x80000002,
    U = 0x80000003,
}

export enum REG {
    SZ = 1,
    EXPAND_SZ = 2,
    BINARY = 3,
    DWORD = 4,
}

export function getRegistryKey (root: HK, path: string): {[name: string]: RegistryValue}

export function getRegistryValue (root: HK, path: string, name: string): any

export function setRegistryValue (root: HK, path: string, name: string, type: REG, value: string): any

export function listRegistrySubkeys (root: HK, path: string): string[]

export function createRegistryKey (root: HK, path: string)

export function deleteRegistryKey (root: HK, path: string)
```
