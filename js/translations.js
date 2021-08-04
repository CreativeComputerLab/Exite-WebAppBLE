/**
 * Handle translations of all text visible in the interface.
 */

/**
 * Table of all keys in all supported languages.
 */
const fullTranslationTable = {
  en: {
    say_this: 'Say This',
    find_robots: 'Find Robots',
    finding_robots: 'Finding Robots',
    connected: 'Connected',
    start_programming: 'Start Programming',
    device_disconnected: 'Device (A or B) Disconnected',
    reconnecting: 'Reconnecting',
    connect_dongle: 'Connect Bluetooth Dongle',
    CompassCalibrate: 'Calibrate Compass',
    Update_firmware: 'Update Firmware',
    Connection_Failure: 'Connection Failure',
    Incompatible_Browser: 'Incompatible Browser',
    Use_Chrome: 'Please visit this page in Google Chrome (version 70 or later).',
    Use_Chrome89: 'Please visit this page in Google Chrome (version 89 or later).',
    No_Ble: 'No Bluetooth Detected',
    Ble_Required: 'This app requires Bluetooth.',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  ko: {
    say_this: '(Slot 1 = 안녕!) 말하기',
    find_robots: '로봇 찾기',
    finding_robots: '로봇 찾는 중',
    connected: '연결 완료',
    start_programming: '프로그래밍 시작하기',
    device_disconnected: '기기 (A 또는 B) 연결 끊김',
    reconnecting: '다시 연결 중',
    connect_dongle: '블루투스 동글 연결하기',
    CompassCalibrate: '나침반 센서 보정',
    Update_firmware: '펌웨어 업데이트',
    Connection_Failure: '연결 실패',
    Incompatible_Browser: '올바르지 않은 브라우저입니다.',
    Use_Chrome: '이 페이지를 구글 크롬에서 열어주세요(버전 70 또는 그 후).',
    Use_Chrome89: '이 페이지를 구글 크롬에서 열어주세요(버전 89 또는 그 후).',
    No_Ble: '블루투스 장치를 찾을 수 없습니다.',
    Ble_Required: '이 앱은 블루투스 통신이 필요합니다.',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  de: {
    say_this: 'Sage',
    find_robots: 'Suche Roboter',
    finding_robots: 'Suche Roboter',
    connected: 'Verbunden',
    start_programming: 'Beginne Programmierung',
    device_disconnected: 'Gerät (A oder B) getrennt',
    reconnecting: 'Wiederverbinden',
    connect_dongle: 'Verbinde Bluetooth Dongle',
    CompassCalibrate: 'Kompass kalibrieren',
    Update_firmware: 'Firmware Updaten',
    Connection_Failure: 'Verbindung fehlgeschlagen',
    Incompatible_Browser: 'Browser nicht kompatibel',
    Use_Chrome: 'Bitte besuche diese Seite in Google Chrome (Version 70 oder höher).',
    Use_Chrome89: 'Bitte besuche diese Seite in Google Chrome (Version 89 oder höher).',
    No_Ble: 'Kein Bluetooth gefunden',
    Ble_Required: 'Diese App benötigt Bluetooth',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  pt: {
    say_this: 'Diga Isso',
    find_robots: 'Encontre Robôs',
    finding_robots: 'Encontrando Robôs',
    connected: 'Conectado',
    start_programming: 'Iniciar a programação',
    device_disconnected: 'Dispositivo (A ou B) Desconectado',
    reconnecting: 'Reconectando',
    connect_dongle: 'Conecte o Dongle Bluetooth',
    CompassCalibrate: 'Calibrar Bússola',
    Update_firmware: 'Atualizar Firmware',
    Connection_Failure: 'Falha na Conexão',
    Incompatible_Browser: 'Navegador incompatível',
    Use_Chrome: 'Visite esta página no Google Chrome (versão 70 ou posterior).',
    Use_Chrome89: 'Visite esta página no Google Chrome (versão 89 ou posterior).',
    No_Ble: 'Nenhum Bluetooth Detectado',
    Ble_Required: 'Este aplicativo requer Bluetooth.',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  fr: {
    say_this: 'Dites ceci',
    find_robots: 'Trouvez des robots',
    finding_robots: 'Trouver des robots',
    connected: 'Connecté',
    start_programming: 'Lancez la programmation',
    device_disconnected: 'Le Périphérique (A ou B) déconnecté',
    reconnecting: 'Reconnecter',
    connect_dongle: 'Connectez le bluetooth dongle',
    CompassCalibrate: 'Calibrer le compas',
    Update_firmware: 'Mettez à Jour le Firmware',
    Connection_Failure: 'Échec de connexion',
    Incompatible_Browser: 'Navigateur incompatible',
    Use_Chrome: 'Veuillez visiter cette page dans Google Chrome (version 70 ou version ultérieure).',
    Use_Chrome89: 'Veuillez visiter cette page dans Google Chrome (version 89 ou version ultérieure).',
    No_Ble: 'Aucun Bluetooth détecté',
    Ble_Required: 'Cette application nécessite Bluetooth.',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  nl: {
    say_this: 'Zeg Dit',
    find_robots: 'Zoek Naar Robots',
    finding_robots: 'Zoeken Naar Robots',
    connected: 'Verbonden',
    start_programming: 'Begin met Programmeren',
    device_disconnected: 'Apparaat (A of B) Losgekoppeld',
    reconnecting: 'Verbinding Opniew Maken',
    connect_dongle: 'Sluit Bluetooth-dongle aan',
    CompassCalibrate: 'Kompas Kalibreren',
    Update_firmware: 'Update Firmware',
    Connection_Failure: 'Verbindingsfout',
    Incompatible_Browser: 'Browser niet compatibel',
    Use_Chrome: 'Bezoek deze pagina in Google Chrome (versie 70 of hoger).',
    Use_Chrome89: 'Bezoek deze pagina in Google Chrome (versie 89 of hoger).',
    No_Ble: 'Bluetooth niet gedetecteerd',
    Ble_Required: 'Deze app vereist Bluetooth.',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  zh_Hans: {
    say_this: '说',
    find_robots: '寻找机器人',
    finding_robots: '寻找机器人中',
    connected: '已连接',
    start_programming: '开始编程',
    device_disconnected: '设备（A或B）已断开连接',
    reconnecting: '重新连接',
    connect_dongle: '连接蓝牙',
    CompassCalibrate: '校准指南针',
    Update_firmware: '更新固件',
    Connection_Failure: '连接失败',
    Incompatible_Browser: '浏览器不兼容。',
    Use_Chrome: '请使用谷歌浏览器。 (版本号 70或者此后的版本)',
    Use_Chrome89: '请使用谷歌浏览器。 (版本号 89或者此后的版本)',
    No_Ble: '未检测到蓝牙。',
    Ble_Required: '该设备需要链接蓝牙。',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  zh_Hant: {
    say_this: '說',
    find_robots: '尋找機器人',
    finding_robots: '尋找機器人中',
    connected: '已連接',
    start_programming: '開始編程',
    device_disconnected: '設備（A或B）已斷開連接',
    reconnecting: '重新連接',
    connect_dongle: '連接藍牙',
    CompassCalibrate: '校準指南針',
    Update_firmware: '更新固件',
    Connection_Failure: '連接失敗',
    Incompatible_Browser: '不兼容的瀏覽器',
    Use_Chrome: '請用Google Chrome瀏覽器（版本70或更高版本）瀏覽此頁面。',
    Use_Chrome89: '請用Google Chrome瀏覽器（版本89或更高版本）瀏覽此頁面。',
    No_Ble: '未能檢測到藍牙',
    Ble_Required: '此應用程式需要藍牙',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  ar: {
    say_this: 'قل هذا',
    find_robots: 'ابحث عن روبوت',
    finding_robots: 'إيجاد روبوت',
    connected: 'متصل',
    start_programming: 'ابدأ البرمجة',
    device_disconnected: 'الجهاز أ أو ب غير متصل',
    reconnecting: 'إعادة الاتصال',
    connect_dongle: 'اتصال عن طريق البلوتوث',
    CompassCalibrate: 'معايرة البوصلة',
    Update_firmware: 'تحديث البرامج الثابتة',
    Connection_Failure: 'فشل الاتصال',
    Incompatible_Browser: 'Incompatible Browser',
    Use_Chrome: 'Please visit this page in Google Chrome (version 70 or later).',
    Use_Chrome89: 'Please visit this page in Google Chrome (version 89 or later).',
    No_Ble: 'No Bluetooth Detected',
    Ble_Required: 'This app requires Bluetooth.',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  da: {
    say_this: 'Sig dette',
    find_robots: 'Find robotter',
    finding_robots: 'Finder robotter',
    connected: 'Forbundet',
    start_programming: 'Start programmering',
    device_disconnected: 'Forbindelse til enhed (A eller B) er afbrudt',
    reconnecting: 'Opretter forbindelse igen',
    connect_dongle: 'Forbind bluetooth dongle',
    CompassCalibrate: 'Kalibrér kompas',
    Update_firmware: 'Opdatér Firmware',
    Connection_Failure: 'Forbindelse mislykket',
    Incompatible_Browser: 'Incompatible Browser',
    Use_Chrome: 'Please visit this page in Google Chrome (version 70 or later).',
    Use_Chrome89: 'Please visit this page in Google Chrome (version 89 or later).',
    No_Ble: 'No Bluetooth Detected',
    Ble_Required: 'This app requires Bluetooth.',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  he: {
    say_this: 'להגיד',
    find_robots: 'למצוא רובוטים',
    finding_robots: 'מחפשים רובוטים',
    connected: 'מחובר',
    start_programming: 'התחל תכנות',
    device_disconnected: 'עתקן ( א או ב) מנותק',
    reconnecting: 'מחברים מחדש',
    connect_dongle: 'מחברים הדונגל לבלוטוס',
    CompassCalibrate: 'כיול מצפן',
    Update_firmware: 'עדכון קשוחה',
    Connection_Failure: 'חיבור נכשל',
    Incompatible_Browser: 'דפדפן לא תואם',
    Use_Chrome: 'אנא בקר באתר זה ב- Google Chrome (גרסה 70 ואילך).',
    Use_Chrome89: 'אנא בקר באתר זה ב- Google Chrome (גרסה 89 ואילך).',
    No_Ble: 'לא זוהה Bluetooth',
    Ble_Required: 'אפליקציה זו דורשת Bluetooth.',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  es: {
    say_this: 'Decir esto',
    find_robots: 'Encontrar robots',
    finding_robots: 'Encontrando robots',
    connected: 'Conectado',
    start_programming: 'Iniciar programacion',
    device_disconnected: 'Dispositivo (A o B) Desconectado',
    reconnecting: 'Reconectando',
    connect_dongle: 'Conectar el dongle del bluethoot',
    CompassCalibrate: 'Calibrar la brujula',
    Update_firmware: 'Actualizar Firmware',
    Connection_Failure: 'Coneccion fallada',
    Incompatible_Browser: 'Navegador no compatible',
    Use_Chrome: 'Por favor, utiliza Google Chrome (versión 70 o superior) para visitar esta página.',
    Use_Chrome89: 'Por favor, utiliza Google Chrome (versión 89 o superior) para visitar esta página.',
    No_Ble: 'Bluetooth no detectado',
    Ble_Required: 'Esta aplicación requiere Bluetooth.',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  ca: {
    say_this: 'Digues això',
    find_robots: 'Cerca robots',
    finding_robots: 'Cercant robots',
    connected: 'Connectat',
    start_programming: 'Comença a programar',
    device_disconnected: 'Dispositiu (A o B) desconnectat',
    reconnecting: 'Reconnectant',
    connect_dongle: 'Connecta llapis Bluetooth',
    CompassCalibrate: 'Calibratge de la brúixola',
    Update_firmware: 'Actualitza el Firmware',
    Connection_Failure: 'Error de connexió',
    Incompatible_Browser: 'Navegador no compatible',
    Use_Chrome: 'Si us plau, utilitza Google Chromve (versió 70 o superior) per visitar aquesta pàgina.',
    Use_Chrome89: 'Si us plau, utilitza Google Chromve (versió 89 o superior) per visitar aquesta pàgina.',
    No_Ble: 'Bluetooth no detectat',
    Ble_Required: 'Aquesta aplicació requereix Bluetooth.',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  fi: {
    say_this: 'Sano tämä',
    find_robots: 'Etsi robotteja',
    finding_robots: 'Etsii robotteja',
    connected: 'Yhdistetty',
    start_programming: 'Aloita ohjelmointi',
    device_disconnected: 'Yhteys katkennut laitteeseen (A tai B)',
    reconnecting: 'Yhdistää uudelleen',
    connect_dongle: 'Yhdistä Bluetooth-palikka',
    CompassCalibrate: 'Kalibroi kompassi',
    Update_firmware: 'Päivitä laiteohjelma',
    Connection_Failure: 'Virhe yhdistettäessä',
    Incompatible_Browser: 'Incompatible Browser',
    Use_Chrome: 'Please visit this page in Google Chrome (version 70 or later).',
    Use_Chrome89: 'Please visit this page in Google Chrome (version 89 or later).',
    No_Ble: 'No Bluetooth Detected',
    Ble_Required: 'This app requires Bluetooth.',
    Choose_Snap_Level: 'Choose Snap! Level'
  },
  sv: {
    say_this: 'Säg detta',
    find_robots: 'Hitta robotar',
    finding_robots: 'Hittar robotar',
    connected: 'Kopplade',
    start_programming: 'Börja programmera',
    device_disconnected: 'Enhet (A eller B) frånkopplad',
    reconnecting: 'Omkopplar',
    connect_dongle: 'Koppla Bluetooth dosa',
    CompassCalibrate: 'Kalibrera kompass',
    Update_firmware: 'Uppdatera Programvara',
    Connection_Failure: 'Problem med kopplingen',
    Incompatible_Browser: 'Incompatible Browser',
    Use_Chrome: 'Please visit this page in Google Chrome (version 70 or later).',
    Use_Chrome89: 'Please visit this page in Google Chrome (version 89 or later).',
    No_Ble: 'No Bluetooth Detected',
    Ble_Required: 'This app requires Bluetooth.',
    Choose_Snap_Level: 'Choose Snap! Level'
  }
};

//Table to use for translations
var thisLocaleTable = null;

/**
 * translateStrings - Translate all strings initially present in the UI, if a
 * translation table has been selected.
 */
function translateStrings() {
  if (thisLocaleTable == null) { return; }
  // Set up defaults
  $('#findBtnText').text(" " + thisLocaleTable["find_robots"]);
  $('#connection-state').html(thisLocaleTable["connected"]);
  $('#start_programming').html(thisLocaleTable["start_programming"]);
}

/**
 * setLanguage - Set the app language based on the navigator language. Set to
 * English if the language is not supported. Translate initial strings once set.
 */
function setLanguage() {
  language = window.navigator.language;
  //console.log("window.navigator.language = " + language);

  if (language.startsWith("zh")) {
    if (language == "zh-TW" || language == "zh-tw") { language = "zh_Hant"; } // Specify trad chinese
    else { language = "zh_Hans"; } // Default to simplified chinese for any other variant
  } else {
    language = language.substring(0, 2); // require the 2 letter code.
  }
  // Convert old code for Hebrew to new
  if (language == "iw") { language = "he"; }

  //console.log("Language code used: " + language);

  thisLocaleTable = fullTranslationTable[language];
  if (thisLocaleTable === null) {
    //console.log("Language unsupported. Defaulting to English (en)");
    language = "en";
    thisLocaleTable = fullTranslationTable[language]; // populate the locale phrases
  }

  //console.log("thisLocaleTable:");
  //console.log(thisLocaleTable);

  translateStrings();
}
