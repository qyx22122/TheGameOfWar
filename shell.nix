{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    nativeBuildInputs = with pkgs; [
      gnumake
      gcc
      glibc
      libxkbcommon
      xorg.libpthreadstubs
      xorg.libX11
      xorg.libxcb
      xorg.libXext
      xorg.libXft
      xorg.libXi
      xorg.libXcursor
      xorg.libXrandr
      xorg.libXinerama
      xorg.xcbproto
      xorg.xcbutil
      xorg.xcbutilcursor
      xorg.xcbutilerrors
      xorg.xcbutilkeysyms
      xorg.xcbutilrenderutil
      xorg.xcbutilwm
      xorg.xorgproto
      glfw
 
    ];
}
