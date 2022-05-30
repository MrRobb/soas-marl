# soas-marl
Multi-Agent Reinforcement Learning implementation for Self Organizing MultiAgent Systems (SOAS)

Based on the paper "Reward Machines for Cooperative Multi-Agent Reinforcement Learning" by Neary, et al.

## Compiling

This has been compiled and tested on Linux Mint 20.2 Uma and Mac OS. 

> This is an OpenFrameworks app --> You have to move the src, bin and other configs to your OpenFrameworks environment to make it work or set OF_ROOT in config.make (explained below).

### Instructions

#### Prerequisite: xtensor
Note: For Linux, the xtensor and xtl packages needed to be installed from source because the packages provided by Linux Mint were not up to date enough to work with openFrameworks. It is a simple installation as long as you have cmake on your system. Follow the instructions in each of these repositories to install xtl and then xtensor:
* https://github.com/xtensor-stack/xtl
* https://github.com/xtensor-stack/xtensor

Essentially, you simply need to run a command like this, where `path_to_prefix` is likely `/usr` (although you may use a different path if you pass the correct cflags and ldflags to the compiler via config.make):
`cmake -DCMAKE_INSTALL_PREFIX=path_to_prefix ..`
`make install`

For Mac OS, the command `brew install xtensor` should install the required libraries.

#### Prerequisite: openFrameworks

1. Go to [OpenFrameworks](http://openframeworks.cc/download/) and download the proper one for your OS. We tested using verion 0.11.2.
2. **Follow their instructions** to completely install OpenFrameworks.
3. Try to execute the `emptyExample` inside of the `apps` folder.
4. If it's working, **go to 5**. Else you need to **check step 2**, you're missing something.
5. Set the `OF_ROOT` environment variable to point to the installation of openFrameworks (Example: `OF_ROOT=/home/user/apps/of_v0.11.2_linux64gcc6_release`). You can do this through either an `export` command added to .bashrc or by setting `OF_ROOT` in config.make.

#### SOAS MARL application
1. Run: `git clone https://github.com/MrRobb/soas-marl`
2. Enter the newly-created directory (`cd soas-marl`)
3. To build, run the command: `make`
4. To run, run the command: `make run`
