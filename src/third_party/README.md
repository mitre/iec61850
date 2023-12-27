# `third_party/`

The payload binary may optionally be built offline by copying the required dependencies to this directory. Having a static copy of the source code here also allows building the `libiec61850` optional dependencies as described below.

## Adding `libiec61850` optional dependencies

After initial build, the `third_party/libiec61850-1.5.1/` directory will be created and populated with the source code and some additional build files. Additional library functionality can be built by adding dependencies to the `third_party/libiec61850-1.5.1/third_party` directory as described below. After copying in the new dependencies, `libiec61850` must be cleaned, reconfigured, and rebuilt. None of the additional functionality is required to build the standard Caldera payload.

### GOOSE Support
On Windows, Npcap is required to build `libiec61850` with GOOSE support. The library's README calls for installing `winpcap`, but this project is no longer supported and `npcap` can serve as a direct replacement in this case. 
1. From the [Npcap downloads page](https://npcap.com/#download) download the Npcap SDK (current version 1.13)
2. Unzip
3. Copy the `Include` directory into `third_party\libiec61850-1.5.1\third_party\winpcap`
4. Create the directory `third_party\libiec61850-1.5.1\third_party\winpcap\Lib`
5. Copy `Packet.lib` and `wpcap.lib` from the `npcap-sdk-1.13\Lib\x64` directory into `third_party\libiec61850-1.5.1\third_party\winpcap\Lib`

### SQLite Logging
SQLite C source files are required to build the logging test examples in `libiec61850`.
1. From the [SQLite Download Page](https://www.sqlite.org/download.html), download the C source code as an amalgamation.
2. Copy `sqlite3.c`, `sqlite3ext.h`, and `sqlite3.h` into `third_party/libiec61850-1.5.1/third_party/sqlite`
