{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        pico-sdk151 = with pkgs;
          (pico-sdk.overrideAttrs (o: rec {
            pname = "pico-sdk";
            version = "1.5.1";
            src = fetchFromGitHub {
              fetchSubmodules = true;
              owner = "raspberrypi";
              repo = pname;
              rev = version;
              sha256 = "sha256-GY5jjJzaENL3ftuU5KpEZAmEZgyFRtLwGVg3W1e/4Ho=";
            };
          }));
      in {
        devShells.default = pkgs.mkShell rec {
          name = "GrooveCoasterController";

          packages = with pkgs; [
            python3
            cmake
            clang-tools
            gcc-arm-embedded
            pico-sdk151
          ];

          shellHook = let icon = "f121";
          in ''
            export PS1="$(echo -e '\u${icon}') {\[$(tput sgr0)\]\[\033[38;5;228m\]\w\[$(tput sgr0)\]\[\033[38;5;15m\]} (${name}) \\$ \[$(tput sgr0)\]"
            export PICO_SDK_PATH="${pico-sdk151}/lib/pico-sdk"
          '';
        };
      });

}
