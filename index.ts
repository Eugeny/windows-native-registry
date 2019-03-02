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
    PD = 0x80000004,
    CC = 0x80000005,
    DD = 0X80000006
}

export enum REG {
    SZ = 1,
    EXPAND_SZ = 2,
    BINARY = 3,
    DWORD = 4,
    DWORD_BIG_ENDIAN = 5,
    DWORD_LITTLE_ENDIAN = 4,
    LINK = 6,
    MULTI_SZ = 7,
    RESOURCE_LIST = 8
}

let native
function getNative () {
    if (!native) {
        native = require('./build/Release/native.node')
    }
    return native
}

export function getRegistryKey (root: HK, path: string): {[name: string]: RegistryValue} {
    let ret = {}
    let key = getNative().getKey(root, path)
    if (!key) {
        return null
    }
    for (let value of key) {
        ret[value.name] = value
    }
    return ret
}

export function getRegistryValue (root: HK, path: string, name: string): any {
    let key = getRegistryKey(root, path)
    if (!key || !key[name]) {
        return null
    }
    return key[name].value
}

export function setRegistryValue (root: HK, path: string, name: string, type: REG, value: string): any {
    return getNative().setValue(root, path, type, name, value)
}

export function listRegistrySubkeys (root: HK, path: string): string[] {
  return getNative().listSubkeys(root, path)
}

export function createRegistryKey (root: HK, path: string) {
  return getNative().createKey(root, path)
}

export function deleteRegistryKey (root: HK, path: string) {
  return getNative().deleteKey(root, path)
}
