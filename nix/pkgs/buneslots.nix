{
  cmake,
  lib,
  ninja,
  sdl3-image,
  sdl3,
  stdenv,
}:

stdenv.mkDerivation rec {
  pname = "buneslots";
  version = "2026.11.1";

  src = ../..;

  nativeBuildInputs = [
    cmake
    ninja
  ];

  buildInputs = [
    sdl3
    sdl3-image
  ];

  cmakeFlags = [
    (lib.cmakeFeature "BUNESLOTS_VERSION" version)
  ];
}
