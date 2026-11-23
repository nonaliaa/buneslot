{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }: 
    flake-utils.lib.eachDefaultSystem (system: 
      let 
        pkgs = nixpkgs.legacyPackages.${system};

        buneslots = pkgs.stdenv.mkDerivation {
          pname = "buneslots";
          version = "0";

          nativeBuildInputs = with pkgs; [
            cmake
            ninja
          ];

          buildInputs = with pkgs; [
            sdl3
            sdl3-image
          ];

          src = self;

          installPhase = ''
            mkdir $out
            cp -r bin/ $out
          '';
        };

        shell = pkgs.mkShell {
          packages = with pkgs; [
            gdb
            valgrind
          ];

          inputsFrom = [
            buneslots
          ];
        };
      in
      {
        formatter = pkgs.nixfmt-tree;

        devShells.default = shell;

        packages.buneslots = buneslots;
      });
}