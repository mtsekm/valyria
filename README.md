# Valyria GFX Benchmark Tool

**Version**: 1.0.0

Valyria is a lightweight graphics benchmarking tool tailored for RDK-based devices, designed to analyze OpenGL ES rendering performance.

## Features
- Measures and analyzes frame rate and render time for various scenes rendered using OpenGL ES.
- Captures key metrics, including CPU and memory usage, with options for SoC-specific data collection.

## Prerequisites
- OpenGL ES 2.0
- libpng and libjpeg
- Essos library (>= 1.0)

## Command-Line Options
The following options are available:

- **`benchmark_duration`**: Duration in seconds for running each render task.
  - Default: `30`
  - Example: `--benchmark_duration=60`

- **`log_level`**: Logging level for Valyria's output, controlling verbosity.
  - Options: `TRACE`, `DEBUG`, `INFO`, `WARN`, `ERROR`
  - Default: `INFO`
  - Example: `--log_level=DEBUG`

- **`direct_mode`**: Sets the rendering mode to Essos direct mode or Wayland client mode.
  - Options: `true` (direct mode), `false` (Wayland client)
  - Default: `false`
  - Example: `--direct_mode=true`

- **`sampling_rate`**: Rate in milliseconds at which metrics are sampled and collected.
  - Default: `1000` ms
  - Example: `--sampling_rate=500`

- **`asset_dir`**: Directory where assets (textures, shaders, etc.) are located.
  - Default: `ASSET_BASE_DIR` (configured during build time to `/usr/share/valyria/assets`)
  - Example: `--asset_dir=/opt/valyria/assets`

- **`target_frame_rate`**: Sets a target frame rate for rendering.
  - Default: `60`
  - Example: `--target_frame_rate=30`

- **`window_width`**: Width of the application window. Setting this to `0` will enable fullscreen mode.
  - Default: `0`
  - Example: `--window_width=1280`

- **`window_height`**: Height of the application window. Setting this to `0` will enable fullscreen mode.
  - Default: `0`
  - Example: `--window_height=720`

- **`output_dir`**: Directory to save benchmark results (JSON and HTML reports).
  - Default: `/tmp`
  - Example: `--output_dir=/opt/persistent/valyria_results`

## Example Usage
Run a benchmark for 60 seconds, with metrics sampled every 500 ms, a target frame rate of 60, and assets loaded from `/opt/valyria/assets`. Save the results to `/opt/persistent/valyria_results`:

```
valyria --benchmark_duration=60 \
        --sampling_rate=500 \
        --target_frame_rate=60 \
        --window_width=1280 \
        --window_height=720 \
        --asset_dir=/opt/valyria/assets \
        --output_dir=/opt/persistent/valyria_results
```

## Example Output
Valyria outputs FPS to the console and generates a report upon completion. Example console output:

```
[INF] Valyria version: 1.0.0
[INF] Running 'Clear' for 30 seconds.
[INF] FPS: 60.000000  -  Frame time: 16.666667 ms
...
[INF] Benchmark run completed.
[INF] JSON report: /tmp/valyria_report.json
[INF] HTML report: /tmp/valyria_report.html
```
