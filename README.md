# Badges
<Plugin Github Action Badger>

# Ledger squid Plugin

This is a plugin for the Ethereum application which helps parsing and displaying relevant information when signing a squid transaction.

## Prerequisite

Clone the plugin to a new folder.

```shell
git clone <Plugin Repo>
```

Then in the same folder clone two more repositories, which is the plugin-tools and app-ethereum.

```shell
git clone https://github.com/LedgerHQ/plugin-tools.git                          #plugin-tools
git clone --recurse-submodules https://github.com/LedgerHQ/app-ethereum.git     #app-ethereum
```
## Documentation

Need more information about the interface, the architecture, or general stuff about ethereum plugins? You can find more about them in the [ethereum-app documentation](https://github.com/LedgerHQ/app-ethereum/blob/master/doc/ethapp_plugins.asc).

## Smart Contracts

Smart contracts covered by this plugin are:

| Network | Version | Smart Contract |
| ---       | --- | --- |
| Ethereum  | V3  | `0x00000000000000000000000000`|


## Build

Go to the plugin-tools folder and run the "./start" script.
```shell
cd plugin-tools  # go to plugin folder
./start.sh       # run the script start.sh
```
The script will build a docker image and attach a console.
When the docker image is running go to the "app-plugin-squid" folder and build the ".elf" files.
```shell
cd app-plugin-squid/tests       # go to the tests folder in app-plugin-squid
./build_local_test_elfs.sh              # run the script build_local_test_elfs.sh
```

## Tests

To test the plugin go to the tests folder from the "app-plugin-squid" and run the script "test"
```shell
cd app-plugin-squid/tests       # go to the tests folder in app-plugin-squid
yarn test                       # run the script test
```
## Continuous Integration


The flow processed in [GitHub Actions](https://github.com/features/actions) is the following:

- Code formatting with [clang-format](http://clang.llvm.org/docs/ClangFormat.html)
- Compilation of the application for Ledger Nano S in [ledger-app-builder](https://github.com/LedgerHQ/ledger-app-builder)
