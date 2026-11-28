/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

var index = require('../index');

if (process.platform === 'linux') {
  console.log('getKeyboardBackends: ', index.getKeyboardBackends());
  console.log('-------------')
}
console.log('getCurrentKeyboardLayout: ', index.getCurrentKeyboardLayout());
console.log('-------------')
console.log('getKeyMap: ', index.getKeyMap());
