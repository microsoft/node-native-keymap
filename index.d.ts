/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

export interface IWindowsKeyMapping {
	vkey: string;
	value: string;
	withShift: string;
	withAltGr: string;
	withShiftAltGr: string;
}
export interface IWindowsKeyboardMapping {
	[code: string]: IWindowsKeyMapping;
}
export interface ILinuxKeyMapping {
	value: string;
	withShift: string;
	withAltGr: string;
	withShiftAltGr: string;
	/** The character produced with the ISO_Level5_Shift modifier (e.g. Neo's Mod4). */
	withLevel5?: string;
	/** The character produced with both the ISO_Level3_Shift and ISO_Level5_Shift modifiers. */
	withLevel3Level5?: string;
}
export interface ILinuxKeyboardMapping {
	[code: string]: ILinuxKeyMapping;
}
export interface IMacKeyMapping {
	value: string;
	valueIsDeadKey: boolean;
	withShift: string;
	withShiftIsDeadKey: boolean;
	withAltGr: string;
	withAltGrIsDeadKey: boolean;
	withShiftAltGr: string;
	withShiftAltGrIsDeadKey: boolean;
}
export interface IMacKeyboardMapping {
	[code: string]: IMacKeyMapping;
}

export type IKeyboardMapping = IWindowsKeyboardMapping | ILinuxKeyboardMapping | IMacKeyboardMapping;

/**
 * Where the keyboard layout is read from on Linux: the X server (also XWayland) or the Wayland compositor.
 */
export type KeyboardBackend = 'x11' | 'wayland';

/**
 * Which Linux backends can be read, and which one the getters read without a backend argument.
 */
export interface IKeyboardBackends {
	/** Whether an X server can be reached, also XWayland. */
	x11: boolean;
	/** Whether the Wayland compositor provides a keymap: `false` when `WAYLAND_DISPLAY` is unset or empty, when the module is built without the Wayland backend, or when libwayland-client or libxkbcommon can't be loaded. */
	wayland: boolean;
	/** The backend the getters read without a backend argument. */
	default: KeyboardBackend;
}

/**
 * @param backend On Linux, read the X server or the Wayland compositor instead of the default backend.
 * The result is empty when that backend can't be read. Ignored on Windows and macOS. On Linux, any other value logs a TypeError and gives an empty result.
 */
export function getKeyMap(backend?: KeyboardBackend): IKeyboardMapping;

export interface IWindowsKeyboardLayoutInfo {
	name: string;
	id: string;
	text: string;
}

export interface ILinuxKeyboardLayoutInfo {
	model: string;
	group: number;
	layout: string;
	variant: string;
	options: string;
	rules: string;
}

export interface IMacKeyboardLayoutInfo {
	id: string;
	localizedName: string;
	lang: string;
}

export type IKeyboardLayoutInfo = IWindowsKeyboardLayoutInfo | ILinuxKeyboardLayoutInfo | IMacKeyboardLayoutInfo;

/**
 * @param backend On Linux, read the X server or the Wayland compositor instead of the default backend.
 * The result is `null` when that backend can't be read. Ignored on Windows and macOS. On Linux, any other value logs a TypeError and gives `null`.
 */
export function getCurrentKeyboardLayout(backend?: KeyboardBackend): IKeyboardLayoutInfo;

export function onDidChangeKeyboardLayout(callback: () => void): void;

export function isISOKeyboard(): boolean | undefined;

/**
 * On Linux, which backends can be read and which one is the default. `undefined` on Windows and macOS.
 */
export function getKeyboardBackends(): IKeyboardBackends | undefined;
