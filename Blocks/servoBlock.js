
              var thisCommand = {
                robot: 'A',
                cmd: "servo",
                port: port,
                value: position
              }

              window.birdbrain.sendCommand(thisCommand);
            