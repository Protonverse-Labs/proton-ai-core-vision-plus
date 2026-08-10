# FAQ

**What is Proton AI Core Vision+?**
An ESP32-S3-based hardware board with an onboard OV5640 camera, IMU, secure element, I/O expander, microphone, and microSD storage, made by Protonverse. It's a developer-focused hardware platform, currently at the working-hardware-prototype stage.

**Which development environment is supported?**
The Arduino ESP32 core, via Arduino IDE. See [Flashing Guide](flashing.md). ESP-IDF and PlatformIO have not been set up or tested for this firmware.

**Which camera is tested?**
The onboard OV5640, 5MP, on the board's 24-pin FPC camera connector. See [Camera Guide](camera.md).

**Does the board include on-device AI acceleration?**
Not currently. The board has "AI" in its name because it's part of the Protonverse ecosystem and is intended as a platform for camera/vision projects, but there is no dedicated neural processing unit and no on-device inference implemented in the current firmware. This may change in the future — see [Roadmap](roadmap.md).

**Is the firmware open?**
Yes — the Arduino firmware in this repository is the actual working firmware used to test the hardware, not a simplified stand-in. A specific open-source license has not been finalized yet; see [README.md](../README.md#licensing).

**Is Protonverse Studio required to use this board?**
No. This repository's firmware and examples work standalone with the Arduino IDE. Protonverse Studio is a separate, proprietary software platform in the Protonverse ecosystem; it is not required to flash or use Proton AI Core Vision+.

**Is the board available yet?**
No. It is a functional hardware prototype; mass production has not launched. A Crowd Supply application is in preparation. See [Project Status](project-status.md).

**Is the hardware fully open source?**
No. Firmware, schematic, and documentation are published to help developers use and build on the board. PCB layout/manufacturing source files, Gerbers, and the full production BOM are intentionally not included. See [hardware/README.md](../hardware/README.md).

**How can I contribute?**
Bug reports, documentation improvements, new examples, and firmware pull requests are welcome — see [CONTRIBUTING.md](../CONTRIBUTING.md).

**Where do I report a security issue?**
See [SECURITY.md](../SECURITY.md) — please don't open a public issue for security reports.
