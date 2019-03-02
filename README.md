# windows-native-registry

## Usage

```js
import { getRegistryKey, listRegistrySubkeys } from 'windows-native-registry'


console.log(
  getRegistryKey('HKLM', 'SOFTWARE\\Microsoft\\DirectMusic'),
  listRegistrySubkeys('HKLM', 'SOFTWARE\\Microsoft'),
)

```

For the full API look at the [typings file](./typings/windows-native-registry.d.ts).
