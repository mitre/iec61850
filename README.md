# MITRE Caldera™ for OT plugin: IEC 61850

A [MITRE Caldera™ for OT](https://github.com/mitre/caldera-ot) plugin supplying [Caldera](https://github.com/mitre/caldera) with IEC 61850 standard series TTPs mapped to MITRE ATT&CK® for ICS [v14](https://attack.mitre.org/resources/updates/updates-october-2023/). This is part of a series of plugins that provide added threat emulation capability for Operational Technology (OT) environments. 

Currently this plugin provides coverage for IEC 61850 services that use Manufacturing Message Specification (MMS) protocol messages. These are unicast-type messages used to exchange analog or digital state information about the controlled process. The other message types defined by the IEC 61850 series, including Generic Object Oriented System Event (GOOSE) and Sampled Value (SV) messages, are not supported in this release.

Full IEC 61850 plugin [documentation](docs/iec61850.md) can be viewed as part of fieldmanual, once the Caldera server is running. 

## Installation

To run Caldera along with the IEC 61850 plugin:
1. Download Caldera as detailed in the [Installation Guide](https://github.com/mitre/caldera)
2. Install the iec61850 plugin in Caldera's plugin directory: `caldera/plugins`
3. Enable the iec61850 plugin by adding `- iec61850` to the list of enabled plugins in `conf/local.yml` or `conf/default.yml` (if running Caldera in insecure mode)

### Version
This plugin has been tested with Caldera v4.2.0 (released 19 June 2023). This can be cloned using the following method:
```
git clone https://github.com/mitre/caldera.git --recursive --branch 4.2.0
```

### Tested OS Versions for Plugin Payload(s)

Building of the IEC 61850 plugin payloads has been tested on multiple operating systems using CMake as described [here](src/README.md#reproducing-builds). See the corresponding plugin payload source code for further build information.

Testing of the binaries has occurred on:
* Microsoft Windows 10 v21H2
* Ubuntu 22.04.3 LTS
* macOS Ventura

### Plugin Payload Source Code
For additional information on the IEC 61850 plugin payload source code, please see the `src/` directory, which contains additional licensing and build guidance.

## Plugin Usage
 - Import the plugin, and optionally set up the required facts (i.e. like the fact sources provided). 
 - Start an operation, optionally using the fact source you set up. 
 - Use "Add Potential Link" to run a specific ability from this plugin. You can enter the fact values manually, or use the ones from your fact source. 
