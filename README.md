Ripple
======

An OpenGL editor for [Tidal](http://tidalcycles.github.io/).  Inspired in
[(fluxus)](http://www.pawfal.org/fluxus/) and written in
[openframeworks](http://openframeworks.cc/).

## Install

Refer to the official OF setup instructions for your operating system.
To install the addons, clone them on `addons/` inside your OF root directory.

You might have to update the `OF_ROOT` variable in `config.make` to your
openFrameworks path, by default it's `/opt/of`.

## Dependencies

  - [openFrameworks 0.8.4](http://openframeworks.cc/download/)
  - [ofxGLEditor](https://github.com/munshkr/ofxGLEditor) addon

Builds are made against
[munshkr/ofxGLEditor](https://github.com/munshkr/ofxGLEditor) instead of the
official repo for now, as it's still unstable too and API might change.

## Usage

  - `ctrl + e`: Evaluate current paragraph or selected text
  - `ctrl + s`: Toggle syntax highlighting
  - `ctrl + f`: Toggle fullscreen
  - `ctrl + n`: Toggle line numbers
  - `ctrl + z`: Toggle auto focus
  - `ctrl + o`: Toggle REPL output buffer
  - `ctrl + q`: Exit Ripple

## License

Ripple is released with GPL version 3.  Please refer to the LICENSE file.
