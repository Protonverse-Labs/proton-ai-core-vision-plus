# Hardware

This folder contains the hardware reference material approved for public release.

## What's Here

- **[Schematic.pdf](Schematic.pdf)** — the full board schematic, showing the ESP32-S3 module, camera interface, power system, and all onboard peripherals (IMU, secure element, I/O expander, microphone, microSD, LEDs, USB-C, battery charging).

See [docs/hardware-overview.md](../docs/hardware-overview.md) for a plain-language summary of the schematic, and [docs/pinout.md](../docs/pinout.md) for the GPIO map (sourced primarily from the firmware, cross-checked against this schematic).

## What's Not Here (and Why)

Proton AI Core Vision+ has a public GitHub repository, but that does not mean every design file is public. Consistent with protecting manufacturing IP, this repository does **not** include:

- PCB layout / PCB design source files
- Gerber files
- NC drill files
- Pick-and-place (CPL) files
- Panelization files
- The full production Bill of Materials (exact reference designators, manufacturer/supplier part numbers, supplier names, and pricing)
- Any other manufacturing or assembly package

This is a deliberate choice, not an oversight — it lets developers understand, verify, and build on the hardware (via the schematic and documentation) without publishing a ready-to-clone manufacturing package. A simplified, non-reconstructable list of major onboard components is available in [docs/hardware-overview.md](../docs/hardware-overview.md).

## Using the Schematic

The schematic is provided for reference and verification — to understand signal names, confirm a GPIO assignment, or check how a subsystem (camera, power, I2C bus) is wired. It is not intended as a starting point for fabricating or cloning the board.
