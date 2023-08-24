// New file to contain all former parent window functions

setLanguage();

SnapExtensions.primitives.set(
  'ex_connect()',
  function (robot) {
    findAndConnect();
  }
);

SnapExtensions.primitives.set(
  'ex_isConnected()',
  function () {
    return deviceConnected ;
  } 
  ); 