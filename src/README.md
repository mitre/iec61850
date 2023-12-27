# IEC 61850 Plugin Code

Where possible, the Caldera for OT plugins leverage open-source libraries and payloads, unifying their exposure through the Caldera Adversary Emulation framework.

- The IEC 61850 plugin is written in C using the open-source [`libiec61850`](https://github.com/mz-automation/libiec61850) library - version 1.5.1 (Mar. 11, 2022).

- The `libiec61850` library is licensed with the [GPL 3.0 License](https://github.com/mz-automation/libiec61850/blob/v1.5/COPYING)

- A custom command-line interface was developed by our team for the `libiec61850` library to allow for the unified exposure of IEC 61850 services as Caldera Ability commands. The CLI payload comes precompiled with the plugin, but can be recompiled following the [instructions below](#reproducing-builds).


## Code Structure

- `app/` - Application code provides the command line interface used by Caldera agents and dispatches function calls to the library.
- `lib/` - Library code adds a wrapper on `libiec61850` to produce an API suited to the needs of a Caldera payload.
- `test/` - Python integration testing of the compiled binary.
- `third_party/` - Location to store static copies of the source code for dependent libraries.

## Reproducing Builds

### Build System Configuration
| Item | Windows binary | Linux binary | macOS binary |
|:-----|:---------------|:-------------|:-------------|
| OS ver. | Windows 10 v21H2 | Ubuntu 22.04.3 LTS | Ventura |
| compiler | GNU 13.1.0 (MinGW UCRT64) | GNU 11.4.0 | AppleClang 15.0.0.15000100 |
| CMake ver. | 3.27.4 | 3.22.1 | 3.28.0 |
| binary name | `iec61850_actions.exe` | `iec61850_actions` | `iec61850_actions_darwin` |

### Step-by-Step Instructions
1. Clone repository:
```
git clone https://github.com/mitre/iec61850.git
```
2. Build the source using CMake:
```bash
# from the src/ directory
mkdir build && cd build
cmake ..
cmake --build .
```
This will build the payload and its dependent libraries, placing the final binary in the `build/bin` directory.

### Offline Builds

The normal build instructions above will fetch the required dependencies during the configuration process. If this is not possible or not desired, the dependencies can be manually copied to the `third_party/` directory using these steps:
1. Retrieve the `libiec61850` version 1.5.1 source code:
  - Download the source code from the library's GitHub repository [here](https://github.com/mz-automation/libiec61850/releases/tag/v1.5.1) as either a `.zip` or `.tar.gz` archive. 
  - Unzip the archive file. This should produce a directory named `libiec61850-1.5.1`.
  - Copy that directory into the `third_party/` directory
2. (Optional) Add optional `libiec61850` dependencies (more information in the [library documentation](https://github.com/mz-automation/libiec61850#building-on-windows-with-goose-support)). *No optional dependencies are required to build the plugin payloads*
3. Retrieve the `argtable3` version 3.2.2 source code:
  - Download the source code from the library's GitHub repository [here](https://github.com/argtable/argtable3/releases/tag/v3.2.2.f25c624) as either a `.zip` or `.tar.gz` archive. 
  - Unzip the archive file. This should produce a directory named `argtable3-3.2.2.f25c624`.
  - Copy that directory into the `third_party/` directory
4. Build the source using CMake:
```bash
# from the src/ directory
mkdir build && cd build
cd build
cmake -DBUILD_LOCAL=ON ..
cmake --build .
```

### Windows Build Environment

The recommended Windows build environment is [MSYS2 UCRT64](https://www.msys2.org/docs/environments/), which uses the `gcc` toolchain. Test builds using the Microsoft Visual C++
(MSVC) compiler have not been successful due to issues in the CMake setup of `libiec61850`.

## Adding Custom Functionality

The application code in `app/app.c` contains the `set_custom_connection()` function to customize the connection settings used throughout the plugin. If your application requires a unique setup (such as a password to connect to the server), this is likely the right place to make the necessary changes.

There are several ways this source could be modified to add more complex functionality. The recommended method would be to create a new C file in the `app/` directory. From the new file, use the API created by the `src/client.h` header to write your new code. Add your new code as a target in the CMake file `app/CMakeLists.txt` and build, following the steps above.

## Testing
`libiec61850` includes a number of test servers that have been the primary testing targets during development. During the build process, the test server binaries will be saved to the `build/bin/examples/` directory. Most of the actions in the payload can be executed against the `server_example_basic_io` server. The `test/` directory contains a Python integration test script that will automate the testing of the compiled binary against these test servers.

## Usage

The `iec61850_actions` executable supports the following general usage:
```
iec61850_actions [--help] <command> [<args>]...
```

Use `iec61850_actions --help` to get started
