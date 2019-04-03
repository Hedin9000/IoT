const {
    Action,
    Event,
    Property,
    SingleThing,
    Thing,
    Value,
    WebThingServer,
  } = require('webthing');

  const uuidv4 = require('uuid/v4');

  const SerialPort = require('serialport');

  const Readline = require('@serialport/parser-readline');

  class PressEvent extends Event {
    constructor(thing, data) {
      super(thing, 'Press', data);
    }
  }
  
class LdpButton extends Thing {
  constructor(lpdCode){
    super('LDP433 button'); //TODO: больше описания
    this.LdpCode = lpdCode;
    this.addAvailableEvent(
      'Press',
      {
        description: 'Button press',
        type: 'number'
      });
  }
}
  
  // Все доступные коды LDP устройств. Остальные будут игнорироваться.
  function GetLdpCodes(){
    return ['123123','123123123'];
  }

  function runServer() {

    var things = [];
    var registeredCodes = GetLdpCodes();
    // На каждый код создается новый экземпляр устройства.
    registeredCodes.forEach(function (code,i){
        var ldpButton = new LdpButton(code);
        things.push(ldpButton);
    }); 
   
    // Инициализация сервера с массивом устройств.
    const server = new WebThingServer(new MultipleThings(things,'LdpButtons'),8888);

    // Обработка остановки сервера.
    process.on('SIGINT', () => {
      server.stop().then(() => process.exit()).catch(() => process.exit());
    });
  
    server.start().catch(console.error);

    // Инициализация серийного порта.
    const port = new SerialPort('/dev/ttyUSB0', { baudRate: 9600 });
    const parser = port.pipe(new Readline({ delimiter: '\n' }));

    port.on("open", function () { console.log('Srial port open'); });

    parser.on('data', data =>{
        console.log('got word from arduino:', data);
        // Ищем утсройство с пришедшим кодом.
        var lpdButton = things.find(function(button) {
          return button.LdpCode == data;
        });
        // Если найдено - бросаем событие
        if(ldpButton != null)
          ldpButton.addEvent(new PressEvent(ldpButton, data));
      });    
  }
  
  runServer();