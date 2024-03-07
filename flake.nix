{
  description = "Tiny but powerfull logging for C++";

  outputs = {
    self,
    flake-utils,
    nixpkgs,
  }: let
    inherit (flake-utils.lib) eachDefaultSystem;
    inherit (nixpkgs.lib) hasSuffix;
    rev = self.shortRev or self.dirtyShortRev or "unknown";

    necrolog = {
      stdenv,
      fetchFromGitHub,
      cmake,
    }:
      stdenv.mkDerivation {
        name = "necrolog-${rev}";
        src = builtins.path {
          path = ./.;
          filter = path: type: ! hasSuffix ".nix" path;
        };
        nativeBuildInputs = [cmake];
      };
  in
    {
      overlays = {
        default = final: prev: {
          necrolog = final.callPackage necrolog {};
        };
      };
    }
    // eachDefaultSystem (system: let
      pkgs = nixpkgs.legacyPackages.${system}.extend self.overlays.default;
    in {
      packages.default = pkgs.necrolog;
      legacyPackages = pkgs;

      formatter = pkgs.alejandra;
    });
}
