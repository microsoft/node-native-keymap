/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

function NativeBinding() {
  this._tried = false;
  this._keymapping = null;
}
NativeBinding.prototype._init = function() {
  if (this._tried) {
    return;
  }
  this._tried = true;
  this._keymapping = require('./build/Release/keymapping');
};
NativeBinding.prototype.getKeyMap = function() {
  try {
    this._init();
    return this._keymapping.getKeyMap();
  } catch(err) {
    console.error(err);
    return [];
  }
};
NativeBinding.prototype.getCurrentKeyboardLayout = function() {
  try {
    this._init();
    return this._keymapping.getCurrentKeyboardLayout();
  } catch(err) {
    console.error(err);
    return null;
  }
};

var binding = new NativeBinding();

exports.getCurrentKeyboardLayout = function() {
  return binding.getCurrentKeyboardLayout();
};
exports.getKeyMap = function() {
  return binding.getKeyMap();
};
