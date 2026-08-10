# Contributing to Proton AI Core Vision+

Thanks for considering a contribution. This project is a working hardware prototype with open firmware — bug reports, documentation fixes, new examples, and firmware improvements are all genuinely useful.

## Ways to Contribute

- **Bug reports** — firmware behaving unexpectedly, a documentation error, a broken link
- **Documentation** — clarifications, fixes, missing steps you hit yourself
- **Examples** — small, focused sketches that demonstrate one subsystem well (see [examples/](examples/) for the current style)
- **Firmware improvements** — bug fixes, cleaner code, new tested functionality

## Before You Start

- For anything beyond a small fix, please open an issue first to discuss the approach — this avoids wasted work on both sides.
- If your change affects hardware-dependent configuration (GPIO mappings, camera init parameters, clock/partition/flash settings), please explain **why** the change is needed and confirm it was tested on real hardware. This firmware has been physically validated as-is; changes to hardware-dependent code carry real risk of breaking that.

## Reporting Bugs

Use the [Bug Report](.github/ISSUE_TEMPLATE/bug_report.md) issue template. Include:
- What you expected vs. what happened
- Output of the `status` Serial command, if relevant
- Your Arduino IDE / board package version
- Steps to reproduce

## Documentation Contributions

Documentation issues and fixes are welcome via pull request. Please keep technical claims verifiable — if you're not sure a spec is correct, flag it rather than guessing (see [docs/project-status.md](docs/project-status.md) for what's currently verified vs. not).

## Submitting a Pull Request

1. Fork the repository and create a branch for your change.
2. Make your change. Keep unrelated changes out of the same PR.
3. If you touched firmware, describe what hardware you tested it on.
4. Fill out the [pull request template](.github/PULL_REQUEST_TEMPLATE.md).
5. Open the PR against the default branch.

## Style Expectations

- Firmware: match the existing style in `firmware/proton-ai-core-vision-plus/` (clear section comments, descriptive Serial output for test/status commands).
- Documentation: plain, direct technical language — avoid unverified claims and marketing language. If something can't be verified, say so explicitly rather than omitting it or guessing.
- Examples: one subsystem per example, with a README covering purpose, requirements, setup, build, flash, expected result, and troubleshooting (see any folder in [examples/](examples/) for the pattern).

No formal CLA or heavy process — just clear commits and an explanation of what changed and why.
