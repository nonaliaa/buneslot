{
  lib,
  pkgs,
  overlays,
}:

import pkgs.path {
  inherit (pkgs.stdenv.hostPlatform) system;
  crossSystem = lib.systems.examples.mingw-msvcrt-x86_64;

  overlays = overlays ++ [
    (final: prev: {
      giflib = prev.giflib.overrideAttrs (
        oldAttrs:
        lib.optionalAttrs final.stdenv.hostPlatform.isMinGW rec {
          patches = [
            (final.fetchpatch2 {
              url = "https://raw.githubusercontent.com/msys2/MINGW-packages/664b63009684f6d59cb942eaffd8716688bbeb6a/mingw-w64-giflib/001-mingw-build.patch";
              hash = "sha256-DBkl6UgVRgvcvKGyrR9KgOTbcpVuV6tUG1A4gecbqkE=";
            })

            "${pkgs.path}/pkgs/by-name/gi/giflib/mingw-install-exes.patch"
          ];

          postPatch = "";

          makeFlags = [
            "PREFIX=${placeholder "out"}"
            "UNAME=MINGW"
          ];
        }
      );

      libhwy = prev.libhwy.overrideAttrs (oldAttrs: {
        meta.platforms = oldAttrs.meta.platforms ++ final.lib.platforms.windows;
      });

      libjxl =
        if final.stdenv.hostPlatform.isMinGW then
          (prev.libjxl.override {
            inherit (pkgs) makeWrapper;

            gdk-pixbuf = null;
            enablePlugins = false;
          }).overrideAttrs
            (oldAttrs: {
              outputs = [ "out" ];

              postInstall = "";
            })
        else
          prev.libjxl;

      libwebp = prev.libwebp.override (
        lib.optionalAttrs final.stdenv.hostPlatform.isMinGW {
          pngSupport = false;
          jpegSupport = false;
          tiffSupport = false;
          gifSupport = false;
        }
      );

      sdl3-image =
        if final.stdenv.hostPlatform.isMinGW then
          (prev.sdl3-image.override {
            libavif = null;
            libwebp = null;
          }).overrideAttrs
            (oldAttrs: {
              cmakeFlags = oldAttrs.cmakeFlags ++ [
                (lib.cmakeBool "SDLIMAGE_AVIF" false)
                (lib.cmakeBool "SDLIMAGE_WEBP" false)
              ];
            })
        else
          prev.sdl3-image;
    })
  ];
}
