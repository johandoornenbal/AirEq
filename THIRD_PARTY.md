# Third-party provenance

## Airwindows PearLiteEQ and BezEQ3

Source: https://github.com/airwindows/airwindows
Commit: `7fd5f5cae262a41e6dd7d9a83d60c46e84d3092a` (retrieved 2026-11-06).
The original `plugins/WinVST/PearLiteEQ/` and `plugins/WinVST/BezEQ3/`
headers, wrappers and processing sources are retained unchanged under
`third_party/airwindows/`, together with the upstream MIT license.
Copyright Chris Johnson / Airwindows.

`Source/PearLiteEQEngine.*` and `Source/BezEQ3Engine.*` replace VST2 metadata
and state management with small standalone engines. The float/double processing
bodies are unchanged apart from method signatures. Initialization is exposed as
`reset(sampleRate)`, normalized parameter setters clamp to 0–1, and noise seeds
are fixed at `0x12345678` / `0x87654321` for deterministic tests. This changes
dither/noise sequences relative to an arbitrarily seeded upstream instance.

Tests compile the untouched original sources behind the independently written
minimal VST2 test interface in `tests/shim/audioeffectx.h`; no VST2 SDK is used.
Matched initial seeds allow exact sample comparisons against the adapted engines.

Input/output trims, gain linking, LUFS matching and Original/Processed comparison
are separate wrapper features. At unity trims with no learned match and Processed
selected, the wrapper returns the selected original engine output. No additional
EQ, normalization, limiter or algorithm crossfade is inserted. Both algorithms
have zero fixed plugin latency, with their original filter phase response intact.

## Tape / ButterComp UI and utilities

The JUCE wrapper, rotary/analogue meter style, metering, level matching, gesture
linking, test structure, and build/sign/install scripts are adapted from
https://github.com/johandoornenbal/tape at
`aae9dbe86de5334558ca6c9e3f7cd89ef5874499` (MIT).
Its copyright notice is retained in LICENSE.
Tape's UI originated in ButterComp Studio,
https://github.com/johandoornenbal/codex-tryout,
commit `3d2daf0d2f350d73640bd03d3f9210cd1b09bbf6` (MIT).
ButterComp's level matching originated at
`c60fa1b798b02b158287f14783efa18aaa85b721`.
Its license notice is retained in `third_party/ButterComp-LICENSE`.

## Loudness reference

Stereo BS.1770 K-weighting filter design follows libebur128 by Jan Kokemueller
(MIT), pinned at `67b33abe1558160ed76ada1322329b0e9e058b02` from
https://github.com/jiixyj/libebur128.
The original C source and license are retained under `third_party/libebur128`
as an independent test oracle, not linked into the plugin. The queue compatibility
code retains its embedded BSD notices. `Source/LoudnessMatch.h` uses bounded
400 ms windows, 100 ms hops, a −70 LUFS absolute gate and −10 LU relative gate.

## JUCE and VST3

JUCE 8.0.12: `29396c22c93392d6738e021b83196283d6e4d850`.
https://github.com/juce-framework/JUCE/blob/8.0.12/LICENSE.md

JUCE is dual-licensed under AGPLv3 and commercial terms. Distribution must comply
with the applicable JUCE license, including corresponding source obligations when
using AGPLv3, or use an appropriate commercial license. The wrapper's MIT license
does not relicense JUCE. Retain the VST3 SDK notices bundled with JUCE.
This is an independent adaptation, not an official Airwindows or JUCE release.
