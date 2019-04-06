/**
 * example-adapter.js - Example adapter.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.*
 */

'use strict';

const {
  Action,     // Action base class
  Adapter,    // Adapter base class
  Constants,  // Constants used throughout the package
  Database,   // Class for interacting with the gateway's settings database
  Deferred,   // Wrapper for a promise, primarily used internally
  Device,     // Device base class
  Event,      // Event base class
  Property,   // Property base class
  Utils,      // Utility functions
} = require('gateway-addon');


const SerialPort = require('serialport');

const Readline = require('@serialport/parser-readline');


class LowPowerButton extends Device{
  constructor(adapter,config){
    super(adapter, "lpd-" + config.ldpCode);
    this.name = config.name;
    this.LpdCode = config.ldpCode;
    this.addEvent('Pressed', {
      '@type': 'PressedEvent',
    });
    this.adapter.handleDeviceAdded(this);    
  }
}


class LowPowerButtonAdapter extends Adapter {
  constructor(addonManager, manifest) {
    super(addonManager, 'LowPowerButtonAdapter', manifest.Name);
    addonManager.addAdapter(this);
    this.runSerialMonitor('/dev/ttyUSB0');
    for (const config of manifest.moziot.config.ldpButtons) {
      new LowPowerButton(this, config);
    }
  }
  runSerialMonitor(comPort) {
    const port = new SerialPort(comPort, { baudRate: 9600 });
    const parser = port.pipe(new Readline({ delimiter: '\n' }));
  
    port.on("open", function () { console.log('Srial port open'); });
  
    parser.on('data', data =>{
     
        console.log('got word from arduino:', data);
        // Ищем утсройство с пришедшим кодом.
        const things = this.getDevices();
        var lpdButton = things.find(function(button) {
          return button.LdpCode == data;
        });
        // Если найдено - бросаем событие
        if(ldpButton != null)
          ldpButton.eventNotify(new Event(device, 'Pressed'));
     
       
      });    
  }
}


function loadLowPowerButtonAdapter(addonManager, manifest, _errorCallback) {
  // Attempt to move to new config format
  if (Database) {
    const db = new Database(manifest.name);
    db.open().then(() => {
      return db.loadConfig();
    }).then((_config) => {
      new LowPowerButtonAdapter(addonManager, manifest);
    });
  }
}


module.exports = loadLowPowerButtonAdapter;
