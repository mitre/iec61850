# MITRE Caldera™ for OT plugin: IEC 61850

A [MITRE Caldera™ for OT](https://github.com/mitre/caldera-ot) plugin supplying [Caldera](https://github.com/mitre/caldera) with IEC 61850 standard series TTPs mapped to MITRE ATT&CK® for ICS [v14](https://attack.mitre.org/resources/updates/updates-october-2023/). This is part of a series of plugins that provide added threat emulation capability for Operational Technology (OT) environments. 

Currently this plugin provides coverage for IEC 61850 services that use Manufacturing Message Specification (MMS) protocol messages. These are unicast-type messages used to exchange analog or digital state information about the controlled process. The other message types defined by the IEC 61850 series, including Generic Object Oriented System Event (GOOSE) and Sampled Value (SV) messages, are not supported in this release.

Full IEC 61850 plugin [documentation](docs/iec61850.md) can be viewed as part of fieldmanual, once the Caldera server is running. 

## Installation

To run Caldera along with the IEC 61850 plugin:
1. Download Caldera as detailed in the [Installation Guide](https://github.com/mitre/caldera)
2. Copy this repository into Caldera's plugin directory: `caldera/plugins`. You can do this in (at least) two ways:
    1. Download the source code from the __Releases__ section of this repository and extract the archive file into the `caldera/plugins` directory.
    2. Use the command line to clone the repository. Navigate to the `caldera/plugins` directory and enter the following command:
```
git clone https://github.com/mitre/iec61850.git
```
3. Download the required compiled payload(s) from the [__Releases__](https://github.com/mitre/iec61850-payloads/releases) section of the `iec61850-payloads` repository. The downloadable payloads are available under the __Assets__ header of the latest release.
4. Save the downloaded payload file(s) in the `caldera/plugins/iec61850/payloads` directory of your Caldera installation.
5. Enable the iec61850 plugin. To do this, add `- iec61850` to the list of enabled plugins in either `conf/local.yml` or `conf/default.yml` (if running Caldera in insecure mode)

### Version
This plugin has been tested with Caldera v4.2.0 and v5.0.0. The latest version of Caldera can be cloned using the following method:
```
git clone https://github.com/mitre/caldera.git --recursive
```

### Plugin Payload Source Code
For additional information on the IEC 61850 plugin payload source code, please see the [`iec61850-payloads`](https://github.com/mitre/iec61850-payloads) repository.

## Usage
1. Install and enable the plugin as described [above](#installation).
2. Optionally, create a fact source to store attributes of the target system. An example is provided [here](./data/sources/48db3ad5-c003-40af-aaee-54f2377da2e5.yml).
3. Start the Caldera server
4. Create a new Operation, optionally using the fact source from step 2.
5. Use "Add Potential Link" to run a specific ability from this plugin. Fact values can can be entered manually, or selected from a fact source.
