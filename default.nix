let
  pkgs = import <nixpkgs> { };
in
{
    qtmips = pkgs.libsForQt5.callPackage (import extras/packages/nix/qtmips.nix) {};
}
