# Demo Recording Checklist

This repository does not yet include a hardware demo video/GIF. `Videos/IMG_4918.MOV` from the source materials was reviewed and excluded — its content appears to be a screen recording of internal Protonverse software rather than raw hardware footage, and is out of scope for this public repository. This checklist describes exactly what to record instead: a short, real, hardware-only demonstration.

## Recommended Shot List

Record these in order, as one continuous take if possible (cuts are fine if a continuous take isn't practical):

1. **Clean shot of the physical board** — Proton AI Core Vision+ alone, in focus, against a plain/uncluttered background, before anything is connected. 2–3 seconds, static or a slow pan.
2. **Camera connected** — show the OV5640 camera module attached to the board via its FPC cable, close enough that the connection is clearly visible.
3. **USB connection** — show the USB-C cable being plugged into the board.
4. **Power-on / boot** — show the board powering on (status LED activity is a good visual cue here).
5. **Firmware running** — a screen capture or over-the-shoulder shot of the Serial Monitor showing the real boot banner and `status` output.
6. **Real camera output / live stream** — the actual MJPEG stream (`stream_on`, opened in a browser) or a `cam_capture` JPEG result, shown on screen. This must be genuine firmware output on real hardware, not a mockup.
7. **Wrap shot** — a final clean shot of the whole setup (board + camera + cable) running.

## Recommended Duration

**15–30 seconds total.** Long enough to show the full boot → camera → output sequence, short enough to loop cleanly as a README GIF or autoplay clip. If recording for Crowd Supply separately, a longer 1–2 minute version with brief voiceover/captions explaining each step is reasonable, but the README asset itself should stay short.

## Recommended Framing

- Landscape orientation, since this will be embedded in a README/webpage.
- Good, even lighting — avoid strong backlighting that silhouettes the board.
- Steady camera (tripod or resting on a surface) — avoid handheld shake, especially during the close-up FPC/USB shots.
- Keep the board large in frame for the close-up shots (camera FPC, USB-C connection) so viewers can actually see the connection being made.
- For the Serial Monitor / browser stream shots, either screen-record directly or film the monitor straight-on (not at an angle) to avoid glare and keep text legible.

## What Should NOT Appear in the Recording

- **Real Wi-Fi credentials** — if Serial Monitor or firmware source is visible on screen, make sure `WIFI_SSID`/`WIFI_PASS` show placeholder values, not a real network name/password.
- **Protonverse Studio or any other internal/proprietary software UI** — this repository and its media are about Proton AI Core Vision+ hardware and its open firmware, not the proprietary Studio platform. Don't include Studio screens unless a separate, explicit decision is made to publish that content.
- **BOM, pricing, supplier names, or any manufacturing/purchasing documents** — do not have `BOM.xlsx` or similar files open or visible in the background.
- **Any other confidential internal document, chat, or email** on a visible screen or desktop.
- **Unverified performance claims spoken or captioned** — e.g., don't caption the stream with a specific FPS number unless it has actually been measured (see [Project Status](project-status.md)).

## After Recording

1. Trim to the shortest clip that still tells the full story.
2. Export a compressed, web-friendly version (H.264 MP4 and/or an optimized GIF/WebP for README embedding — keep file size reasonable so it doesn't slow down page loads).
3. Update the **See It in Action** section of [README.md](../README.md) to embed the new asset once it's ready, and remove the reference to this checklist once a real demo is in place.
