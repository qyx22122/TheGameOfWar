{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    nativeBuildInputs = with pkgs; [
      gnumake
      gcc
      glibc.dev
      libxkbcommon.dev
      xorg.libpthreadstubs
      xorg.libX11.dev
      xorg.libxcb.dev
      xorg.libXext.dev
      xorg.libXi.dev
      xorg.libXcursor
      xorg.libXrandr
      xorg.libXinerama
      xorg.xcbproto
      xorg.xcbutil.dev
      xorg.xcbutilcursor.dev
      xorg.xcbutilerrors
      xorg.xcbutilkeysyms.dev
      xorg.xcbutilrenderutil.dev
      xorg.xcbutilwm.dev
      xorg.xorgproto
      glfw
 
    ];
}
