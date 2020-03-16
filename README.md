# Azure Sphere DA7212 I2S demo

This sample demonstrates I2S audio capability on Azure Sphere rtcore. A DA7212 codec from Nordic is used with I2S driver from [Codethink MT3260 driver](https://github.com/CodethinkLabs/mt3620-m4-drivers) to demostrate a 16kHz voice recording and playback.

## To build and run the sample

### Prep your device and environment

1. Ensure that your Azure Sphere MT3620 is connected to your PC, and your PC is connected to the internet.

2. Even if you've performed this set up previously, ensure that you have Azure Sphere SDK version 20.01 or above. In an Azure Sphere Developer Command Prompt, run **azsphere show-version** to check. Download and install the [latest SDK](https://aka.ms/AzureSphereSDKDownload) as needed.
   
4. At the command prompt issue the following command:

   ```
   azsphere dev edv --enablertcoredebugging
   ```

   This command must be run as administrator when you enable real-time core debugging because it installs USB drivers for the debugger.

4. This sample use [ARD-AUDIO-DA7212](https://www.dialog-semiconductor.com/content/ard-audio-da7212) evaluation board. Below table describes hardware connection with MT3620 RDB. 
   
    | DA7212 | ARD-AUDIO-DA7212 | MT3620 RDB  | MT3620 |
    |  ----  | ---- | ---- | ---- | 
    | I2S_DO | J2-15  | H1-3 | GPIO59_I2S0_RX |
    | MCLK   | J2-7   | H1-9 | GPIO57_I2S0_MCLK |
    | I2S_DI | J2-5 | H1-5 | GPIO56_I2S0_TX | 
    | I2S_WCLK  | J2-3 | H1-7 | GPIO58_I2S0_FS |
    | I2S_BLCK | J2-1 | H1-11 | GPIO60_I2S0_BCLK |
    | I2C_SCL | J1-20 | H4-12 | GPIO37_MOSI2_RTS2_CLK2 |
    | I2C_SDA | J1-18 | H4-6  | GPIO38_MISO2_RXD2_DATA2 |
    | VCC  | J7-4  | H3-3 | VCC |
    | GND  | J7-14  | H3-2 | GND |

5. Plug a earphone to J6 on ARD-AUDIO-DA7212. 

### Build and run Sample

1. Clone this project using `git clone https://github.com/xiongyu0523/azure-sphere-rtcore-da7212`
2. Start Visual Studio.
3. From the **File** menu, select **Open > CMake...** and navigate to the folder that contains the sample to load
4. Select the file CMakeLists.txt and then click **Open**. 
5. In Solution Explorer, right-click the *CMakeLists.txt* file, and select **Generate Cache for azure-sphere-extmcu-ota**. This step performs the cmake build process to generate the native ninja build files. 
6. In Solution Explorer, right-click the *CMakeLists.txt* file, and select **Build** to build the project and generate .imagepackage target.
7. Double click *CMakeLists.txt* file and press F5 to start the application with debugging. 

### Play with the demo

1. Press Button A to record voice for 5 seconds. 
2. Press Button B to playback recorded voice. (cast to both L and R channel)
3. Color LED will represent status of the demo
   
    | LED status | Note |
    |  ----  | ---- |
    | OFF    | Idle |
    | RED    | Recording |
    | GREEN  | A fresh recroding is finished |
    | BLUE   | Playing |

## OSS and License

This project has submoduled several open sources project on github, please refer to their license for details. 

- [Codethink MT3620 drvier](https://github.com/ARMmbed/littlefs)
- [NXP MCUXpresso SDK for FRDM-K66 (DA7212 driver)](https://mcuxpresso.nxp.com/en/welcome)

All other code developed within project are MIT license

```
MIT License

Copyright (c) 2020 neo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```