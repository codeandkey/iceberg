![screenshot](https://github.com/molecuul/iceberg/raw/master/preview.png)

### iceberg [![Build Status](https://travis-ci.com/molecuul/iceberg.svg?branch=master)](https://travis-ci.com/molecuul/iceberg)
iceberg is a work-in-progress game/game engine with a functional design

### architecture
iceberg uses SDL for graphics and OpenAL for audio.
With the intent of being easy to modify, no fancy shaders or OpenGL pipelines are used.
libxml2 is used to parse maps from Tiled.

### features
- intent to support many features offered in Tiled
- objects can easily receive custom parameters
- animated tiles/tilesets
- multiple layers/tilesets
- reliable tile texture loading regardless of tile definition (if the resources can be located locally)
- simple engine interface, verbose logging
- versatile event queue (much of the engine is built around it)
