{
  description = "Description for the project";

  inputs = {
    devshell.url = "github:numtide/devshell";
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = inputs @ {flake-parts, ...}:
    flake-parts.lib.mkFlake {inherit inputs;} {
      imports = [
        inputs.devshell.flakeModule
      ];
      systems = ["x86_64-linux" "aarch64-linux" "aarch64-darwin" "x86_64-darwin"];
      perSystem = {
        config,
        self',
        inputs',
        pkgs,
        system,
        ...
      }: let
        # https://github.com/NixOS/nixpkgs/issues/175297
        pico-sdk151 = with pkgs; (pico-sdk.overrideAttrs (o: rec {
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
        devshells.default = {
          packages = [
            pico-sdk151
            pkgs.cmake
            pkgs.gcc
            pkgs.gcc-arm-embedded
            pkgs.gnumake
            pkgs.python3
            pkgs.openocd
            pkgs.gdb
            pkgs.picotool
          ];

          env = [
            {
              name = "PICO_SDK_PATH";
              value = "${pico-sdk151}/lib/pico-sdk";
            }
          ];
        };
      };
    };
}
