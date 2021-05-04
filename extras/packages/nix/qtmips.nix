{ lib, stdenv, cmake, wrapQtAppsHook, qtbase, qtsvg }:
stdenv.mkDerivation {
    name = "QtMips";
    src = builtins.fetchGit ../../..;
    nativeBuildInputs = [ cmake wrapQtAppsHook ];
    buildInputs = [ qtbase ];
    meta = {
        description = "MIPS CPU simulator for education purposes.";
        longDescription = ''
          MIPS CPU simulator for education purposes with pipeline and cache visualization.
          Developed at FEE CTU for computer architectures classes.
        '';
        homepage = "https://github.com/cvut/qtmips";
        license = lib.licenses.gpl3Plus;
        maintainers = [ "Jakub Dupak <dev@jakubdupak.com>" ];
    };
}