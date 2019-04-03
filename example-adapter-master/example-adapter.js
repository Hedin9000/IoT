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




class ExampleProperty extends Property {
  constructor(device, name, propertyDescription) {
    super(device, name, propertyDescription);
    this.setCachedValue(propertyDescription.value);
    this.device.notifyPropertyChanged(this);
  }

  /**
   * Set the value of the property.
   *
   * @param {*} value The new value to set
   * @returns a promise which resolves to the updated value.
   *
   * @note it is possible that the updated value doesn't match
   * the value passed in.
   */
  setValue(value) {
    return new Promise((resolve, reject) => {
      super.setValue(value).then((updatedValue) => {
        resolve(updatedValue);
        this.device.notifyPropertyChanged(this);
      }).catch((err) => {
        reject(err);
      });
    });
  }
}
class LowPowerButton extends Device{
  constructor(adapter,deviceId, lpdCode){
    super(adapter, id);
    this.name = "LowPowerButton"
    this.LpdCode = LpdCode
    this.addEvent('pressed', {
      '@type': 'PressedEvent',
    });    
  }
}
class ExampleDevice extends Device {
  constructor(adapter, id, deviceDescription) {
    super(adapter, id);
    
    this.name = deviceDescription.name;
    this.type = deviceDescription.type;
    this['@type'] = deviceDescription['@type'];
    this.description = deviceDescription.description;
    for (const propertyName in deviceDescription.properties) {
      const propertyDescription = deviceDescription.properties[propertyName];
      const property = new ExampleProperty(this, propertyName,
                                           propertyDescription);
      this.properties.set(propertyName, property);
    }
  }
}

class ExampleAdapter extends Adapter {
  constructor(addonManager, packageName) {
    super(addonManager, 'LowPowerButtonAdapter', packageName);
    addonManager.addAdapter(this);
    this.paring = false;
    this.runSerialMonitor('/dev/ttyUSB0');
  }

  /**
   * Example process to add a new device to the adapter.
   *
   * The important part is to call: `this.handleDeviceAdded(device)`
   *
   * @param {String} deviceId ID of the device to add.
   * @param {String} deviceDescription Description of the device to add.
   * @return {Promise} which resolves to the device added.
   */
 

  addDevice(deviceId, lpdCode) {
    return new Promise((resolve, reject) => {
      if (deviceId in this.devices) {
        reject(`Device: ${deviceId} already exists.`);
      } else {
        const device = new LowPowerButton(this, deviceId, lpdCode);
        this.handleDeviceAdded(device);
        resolve(device);
      }
    });
  }

  /**
   * Example process ro remove a device from the adapter.
   *
   * The important part is to call: `this.handleDeviceRemoved(device)`
   *
   * @param {String} deviceId ID of the device to remove.
   * @return {Promise} which resolves to the device removed.
   */
  removeDevice(deviceId) {
    return new Promise((resolve, reject) => {
      const device = this.devices[deviceId];
      if (device) {
        this.handleDeviceRemoved(device);
        resolve(device);
      } else {
        reject(`Device: ${deviceId} not found.`);
      }
    });
  }

  /**
   * Start the pairing/discovery process.
   *
   * @param {Number} timeoutSeconds Number of seconds to run before timeout
   */
  startPairing(_timeoutSeconds) {
    console.log('ExampleAdapter:', this.name,
                'id', this.id, 'pairing started');
                this.paring = true;
  }

  /**
   * Cancel the pairing/discovery process.
   */
  cancelPairing() {
    console.log('ExampleAdapter:', this.name, 'id', this.id,
                'pairing cancelled');
                this.paring = false;
  }

  /**
   * Unpair the provided the device from the adapter.
   *
   * @param {Object} device Device to unpair with
   */
  removeThing(device) {
    console.log('ExampleAdapter:', this.name, 'id', this.id,
                'removeThing(', device.id, ') started');

    this.removeDevice(device.id).then(() => {
      console.log('ExampleAdapter: device:', device.id, 'was unpaired.');
    }).catch((err) => {
      console.error('ExampleAdapter: unpairing', device.id, 'failed');
      console.error(err);
    });
  }

  /**
   * Cancel unpairing process.
   *
   * @param {Object} device Device that is currently being paired
   */
  cancelRemoveThing(device) {
    console.log('ExampleAdapter:', this.name, 'id', this.id,
                'cancelRemoveThing(', device.id, ')');
  }

  runSerialMonitor(comPort) {
    const port = new SerialPort(comPort, { baudRate: 9600 });
    const parser = port.pipe(new Readline({ delimiter: '\n' }));
  
    port.on("open", function () { console.log('Srial port open'); });
  
    parser.on('data', data =>{
      if(!this.paring){
        console.log('got word from arduino:', data);
        // Ищем утсройство с пришедшим кодом.
        const things = this.getDevices();
        var lpdButton = things.find(function(button) {
          return button.LdpCode == data;
        });
        // Если найдено - бросаем событие
        if(ldpButton != null)
          ldpButton.eventNotify(new Event(device, 'pressed'));
      }
      else{
        var newButton = new LowPowerButton(this,'lpd-'+data, data)
        this.handleDeviceAdded(newButton);
      }
       
      });    
  }
}

function loadExampleAdapter(addonManager, manifest, _errorCallback) {
  const adapter = new ExampleAdapter(addonManager, manifest.name);
  const device = new ExampleDevice(adapter, 'example-plug-2', {
    name: 'example-plug-2',
    '@type': ['OnOffSwitch'],
    type: 'onOffSwitch',
    description: 'Example Device',
    properties: {
      on: {
        '@type': 'OnOffProperty',
        label: 'On/Off',
        name: 'on',
        type: 'boolean',
        value: false,
      },
    },
  });
  adapter.handleDeviceAdded(device);
  

 
}



module.exports = loadExampleAdapter;
