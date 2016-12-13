/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

var getNativeModuleSafe = (function() {
  var keymapping = null;
  var tried = false;
  return function getNativeModuleSafe() {
    if (!tried) {
      tried = true;
      try {
        keymapping = require('./build/Release/keymapping');
      } catch(err) {
        console.error(err);
      }
    }
    return keymapping;
  }
})();

exports.getKeyMap = function() {
  var keymapping = getNativeModuleSafe();
  if (!keymapping) {
    return [];
  }

  var r = [];
  try {
    r = keymapping.getKeyMap();
  } catch(err) {
    console.error(err);
  }
  return r;
};

exports.getCurrentKeyboardLayoutName = function() {
  var keymapping = getNativeModuleSafe();
  if (!keymapping) {
    return '';
  }

  var r = '';
  try {
    r = keymapping.getCurrentKeyboardLayoutName();
  } catch (err) {
    console.error(err);
  }
  return r;
}
