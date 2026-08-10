# Security Policy

## Reporting a Vulnerability

If you find a security issue in the Proton AI Core Vision+ firmware, examples, or documentation (for example: a way the firmware could be made to leak data, crash unsafely, or an unsafe default configuration), please report it privately rather than opening a public issue.

**Preferred method:** use [GitHub's private vulnerability reporting](https://docs.github.com/en/code-security/security-advisories/guidance-on-reporting-and-writing/privately-reporting-a-security-vulnerability) (Security tab → Report a vulnerability) once enabled on this repository.

`TODO: Verification required` — a dedicated security contact email has not yet been provided by the project owner. Add one here once available (see `GITHUB_SETUP_INTERNAL.md` in the internal review materials).

## What to Include

- A clear description of the issue and its potential impact
- Steps to reproduce, if applicable
- Firmware version / commit affected

## Scope

This policy covers the firmware and example code in this repository. It does not cover:
- Physical/hardware attacks requiring direct access to the board
- Issues in third-party libraries used by the firmware (please also report those upstream)
- Protonverse Studio (a separate, proprietary product) — do not report Studio-related issues here

## What to Expect

We aim to acknowledge reports promptly and will work with you on a fix before any public disclosure. Please give us a reasonable amount of time to address the issue before disclosing it publicly.

## Known Note

An earlier internal firmware copy contained a hardcoded Wi-Fi network name and password used for hardware bring-up testing. The version published in this repository has those values replaced with placeholders (`YOUR_WIFI_SSID` / `YOUR_WIFI_PASSWORD`). If you're working from an older internal copy of this firmware, replace those values before use and do not commit real credentials.
