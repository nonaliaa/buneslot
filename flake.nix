{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;

          overlays = [
            (final: prev: {
              buneslots = final.callPackage ./nix/pkgs/buneslots.nix { };
            })
          ];
        };
        win64Pkgs = pkgs.callPackage ./nix/cross/win64.nix { };

        shell = pkgs.mkShell {
          packages = with pkgs; [
            gdb
            valgrind
          ];

          inputsFrom = with pkgs; [
            buneslots
          ];
        };
      in
      {
        formatter = pkgs.nixfmt-tree;

        devShells.default = shell;

        packages = rec {
          inherit (pkgs) buneslots;
          buneslots-win64 = win64Pkgs.buneslots;

          default = buneslots;
        };
      }
    );
}
