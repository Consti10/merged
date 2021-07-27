/// <reference types="emscripten" />
/** Above will import declarations from @types/emscripten, including Module etc. */

// This will merge to the existing EmscriptenModule interface from @types/emscripten
// If this doesn't work, try globalThis.EmscriptenModule instead.
export interface DecoderModule extends EmscriptenModule {
	// Module.cwrap() will be available by doing this.
	// Requires -s "EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap']"
  cwrap: typeof cwrap;
  // Requires -s EXTRA_EXPORTED_RUNTIME_METHODS="['addFunction']"
  addFunction: typeof addFunction;
  // ErrorCode initDecoder(int fileSize, int logLv);
  _initDecoder(fileSize: number, logLevel: number): number;
  // ErrorCode openDecoder(int *paramArray, int paramCount, long videoCallback, long audioCallback);
  _openDecoder(ptr: number, cnt: number, videoCallback: number, audioCallback: number, requestCallback: number): number;
  // ErrorCode decodeOnePacket();
  _decodeOnePacket(): number;
  // int sendData(unsigned char *buff, int size);
  _sendData(ptr: number, size: number): number
  //
  _seekTo(ms: number, accurateSeek: number): number;
  // ErrorCode closeDecoder();
  _closeDecoder(): number;
  // ErrorCode uninitDecoder();
  _uninitDecoder(): number;
}

// Declare any name
declare const decoderModule: DecoderModule;
// Only for -s MODULARIZE=1 -s EXPORT_ES6=1
export default decoderModule;
