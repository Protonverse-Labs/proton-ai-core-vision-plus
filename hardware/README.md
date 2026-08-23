# Hardware

This folder contains the hardware reference material approved for public release.

## What's Here

- **[Schematic.pdf](Schematic.pdf)** — the full board schematic, showing the ESP32-S3 module, camera interface, power system, and all onboard peripherals (IMU, secure element, I/O expander, microphone, microSD, LEDs, USB-C, battery charging).
- **[BOM.csv](BOM.csv)** — a parts-level bill of materials (reference designators, values, manufacturer part numbers, short descriptions), derived directly from the schematic above. This is **not** the production BOM — see below.

See [docs/hardware-overview.md](../docs/hardware-overview.md) for a plain-language summary of the schematic, and [docs/pinout.md](../docs/pinout.md) for the GPIO map (sourced primarily from the firmware, cross-checked against this schematic).

## What's Not Here (and Why)

Proton AI Core Vision+ has a public GitHub repository, but that does not mean every design file is public. Consistent with protecting manufacturing IP, this repository does **not** include:

- PCB layout / PCB design source files
- Gerber files
- NC drill files
- Pick-and-place (CPL) files
- Panelization files
- The production Bill of Materials (supplier names, supplier part numbers, pricing, order quantities, and assembly-house-specific fields — as distinct from the parts-level `BOM.csv` above, which has neither)
- Any other manufacturing or assembly package

This is a deliberate choice, not an oversight — it lets developers understand, verify, source parts for, and build on the hardware (via the schematic, parts-level BOM, and documentation) without publishing a ready-to-clone manufacturing package.

## Licence

The schematic, `BOM.csv`, and this documentation are licensed under **CERN-OHL-P v2** (permissive) — see [LICENSE](../LICENSE) in the repository root. Firmware and example sketches are licensed separately; see [README.md § Licence](../README.md#licence) for the full breakdown.

## Using the Schematic

The schematic is provided for reference and verification — to understand signal names, confirm a GPIO assignment, or check how a subsystem (camera, power, I2C bus) is wired. It is not intended as a starting point for fabricating or cloning the board.
