{inputs, ...}: {
  imports = [inputs.devshell.flakeModule];
  perSystem = {pkgs, ...}: let
    local-pico-sdk =
      pkgs.pico-sdk.override
      {
        withSubmodules = true;
      };
  in {
    devshells.default = {
      packages = with pkgs; [
        gcc-arm-embedded
        local-pico-sdk
        picotool

        cmake
        gnumake
        python3
        clang-tools
        udisks # Interact with bootloader filesystem
        tio # terminal program to interface with serial
      ];

      env = [
        {
          name = "PICO_SDK_PATH";
          value = "${local-pico-sdk}/lib/pico-sdk";
        }
      ];
    };
  };
}
