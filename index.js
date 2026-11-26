/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

// The Linux backend a getter reads: undefined for the default.
function checkBackend(backend) {
  if (backend !== undefined && backend !== 'x11' && backend !== 'wayland') {
    throw new TypeError('The keyboard backend must be \'x11\', \'wayland\' or undefined.');
  }
}

function NativeBinding() {
  this._tried = false;
  this._keymapping = null;
}
NativeBinding.prototype._init = function() {
  if (this._tried) {
    return;
  }
  this._tried = true;
  try {
    this._keymapping = require('./build/Release/keymapping');
  } catch (err) {
    // fallback to the debug build
    this._keymapping = require('./build/Debug/keymapping');
  }
};
NativeBinding.prototype.getKeyMap = function(backend) {
  checkBackend(backend);
  try {
    this._init();
    return this._keymapping.getKeyMap(backend);
  } catch(err) {
    console.error(err);
    return [];
  }
};
NativeBinding.prototype.getCurrentKeyboardLayout = function(backend) {
  checkBackend(backend);
  try {
    this._init();
    return this._keymapping.getCurrentKeyboardLayout(backend);
  } catch(err) {
    console.error(err);
    return null;
  }
};
NativeBinding.prototype.onDidChangeKeyboardLayout = function(callback) {
  try {
    this._init();
    this._keymapping.onDidChangeKeyboardLayout(callback);
  } catch(err) {
    console.error(err);
  }
}
NativeBinding.prototype.isISOKeyboard = function(callback) {
  try {
    this._init();
    return this._keymapping.isISOKeyboard();
  } catch(err) {
    return false;
  }
}
NativeBinding.prototype.getKeyboardBackends = function() {
  try {
    this._init();
    return this._keymapping.getKeyboardBackends();
  } catch(err) {
    console.error(err);
    return undefined;
  }
}

var binding = new NativeBinding();

exports.getCurrentKeyboardLayout = function(backend) {
  return binding.getCurrentKeyboardLayout(backend);
};
exports.getKeyMap = function(backend) {
  return binding.getKeyMap(backend);
};
exports.onDidChangeKeyboardLayout = function(callback) {
  return binding.onDidChangeKeyboardLayout(callback);
};
exports.isISOKeyboard = function(callback) {
  return binding.isISOKeyboard(callback);
};
exports.getKeyboardBackends = function() {
  return binding.getKeyboardBackends();
};
