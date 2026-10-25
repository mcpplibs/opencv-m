export module mcpplibs.mwrap.opencv;

// Preload the internal producers before core: GCC's cross-package self-test
// mapper otherwise falls back to bare BMI names while reading core's imports.
export import mcpplibs.mwrap.opencv.detail.core;
export import mcpplibs.mwrap.opencv.detail.matx;
export import mcpplibs.mwrap.opencv.core;
export import mcpplibs.mwrap.opencv.macros;
export import mcpplibs.mwrap.opencv.detail.selected;
