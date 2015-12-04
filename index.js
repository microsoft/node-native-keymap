/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

var keymapping = require('./build/Release/keymapping');

exports.getKeyMap = function() {
  return keymapping.getKeyMap();
};
