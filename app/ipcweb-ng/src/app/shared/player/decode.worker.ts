/// <reference lib="webworker" />
/// <reference types="emscripten" />

import Logger from "src/app/logger";
import { DecoderResponse, DecoderRequest, WorkerMessage } from './worker-message.model';

import decoderModule from './wasm/wasmdecoder';
let WasmDecoderModule = decoderModule(
  {
    onRuntimeInitialized: () => {
      onWasmLoaded();
    },
  }
);

class Decoder {
  logger: Logger;
  coreLogLevel: number;
  wasmLoaded: boolean;
  tmpReqQue: WorkerMessage[];
  cacheBuffer: any;
  decodeTimer: NodeJS.Timer;
  videoCallback: number;
  audioCallback: number;
  accurateSeek: boolean;
  requestCallback: number;

  constructor() {
    this.logger = new Logger('Decoder');
    // this.coreLogLevel = 2;
    this.coreLogLevel = 1;
    this.accurateSeek = true;
    this.wasmLoaded = false;
    this.tmpReqQue = [];
    this.cacheBuffer = null;
    this.decodeTimer = null;
    this.videoCallback = null;
    this.audioCallback = null;
  }

  initDecoder(fileSize: number, chunkSize: number) {
    let ret = WasmDecoderModule._initDecoder(fileSize, this.coreLogLevel);
    this.logger.info("initDecoder return " + ret + ".");
    if (0 == ret) {
      this.cacheBuffer = WasmDecoderModule._malloc(chunkSize);
    }
    postMessage(new WorkerMessage(DecoderResponse.kInitDecoderRsp, { e: ret }));
  }

  uninitDecoder() {
    let ret = WasmDecoderModule._uninitDecoder();
    this.logger.info("Uninit ffmpeg decoder return " + ret + ".");
    if (this.cacheBuffer != null) {
      WasmDecoderModule._free(this.cacheBuffer);
      this.cacheBuffer = null;
    }
  }

  openDecoder() {
    let paramCount = 7, paramSize = 4;
    let paramByteBuffer = WasmDecoderModule._malloc(paramCount * paramSize);
    // console.error('send open decoder msg start');
    let ret = WasmDecoderModule._openDecoder(paramByteBuffer, paramCount, this.videoCallback, this.audioCallback, this.requestCallback);
    // console.error('send open decoder msg end', ret);
    this.logger.info("openDecoder return " + ret);

    if (ret == 0) {
      let paramIntBuff = paramByteBuffer >> 2;
      let paramArray = new Uint32Array(WasmDecoderModule.HEAP32.subarray(paramIntBuff, paramIntBuff + paramCount));
      let duration = paramArray[0];
      let videoPixFmt = paramArray[1];
      let videoWidth = paramArray[2];
      let videoHeight = paramArray[3];
      let audioSampleFmt = paramArray[4];
      let audioChannels = paramArray[5];
      let audioSampleRate = paramArray[6];

      let objData: WorkerMessage = {
        type: DecoderResponse.kOpenDecoderRsp,
        data: {
          e: ret,
          v: {
            d: duration,
            p: videoPixFmt,
            w: videoWidth,
            h: videoHeight
          },
          a: {
            sampleFormat: audioSampleFmt,
            channelCount: audioChannels,
            sampleRate: audioSampleRate
          }
        }
      };
      postMessage(objData);
    } else {
      postMessage(new WorkerMessage(DecoderResponse.kOpenDecoderRsp, { e: ret }));
    }
    WasmDecoderModule._free(paramByteBuffer);
  }

  closeDecoder() {
    this.logger.info("closeDecoder.");
    if (this.decodeTimer) {
      clearInterval(this.decodeTimer);
      this.decodeTimer = null;
      this.logger.info("Decode timer stopped.");
    }

    let ret = WasmDecoderModule._closeDecoder();
    this.logger.info("Close ffmpeg decoder return " + ret + ".");
    postMessage(new WorkerMessage(DecoderResponse.kCloseDecoderRsp, { e: 0 }));
  }

  startDecoding(interval: number) {
    // this.logger.info("Start decoding.");
    if (this.decodeTimer) {
      clearInterval(this.decodeTimer);
    }
    this.decodeTimer = setInterval(this.decode, interval);
  }

  pauseDecoding() {
    //this.logger.info("Pause decoding.");
    if (this.decodeTimer) {
      clearInterval(this.decodeTimer);
      this.decodeTimer = null;
    }
  }

  decode() {
    // console.error("Decode: in decode");
    let ret = WasmDecoderModule._decodeOnePacket();
    if (ret == 7) {
      decoder.logger.info("Decoder finished.");
      decoder.pauseDecoding();
      postMessage(new WorkerMessage(DecoderResponse.kDecodeFinishedEvt, null));
    }
    while (ret == 9) {
      ret = WasmDecoderModule._decodeOnePacket();
    }
  }

  sendData(data: any) {
    let typedArray = new Uint8Array(data);
    //this.logger.info("sendData size " + typedArray.length)
    WasmDecoderModule.HEAPU8.set(typedArray, this.cacheBuffer);
    WasmDecoderModule._sendData(this.cacheBuffer, typedArray.length);
  }

  seekTo(ms: number) {
    let accurateSeek = this.accurateSeek ? 1 : 0;
    let ret = WasmDecoderModule._seekTo(ms, accurateSeek);
    postMessage(new WorkerMessage(DecoderResponse.kSeekToRsp, { r: ret }));
  }

  processReq(req: WorkerMessage) {
    //this.logger.info("processReq " + req.type + ".");
    // console.error('Decoder: get msg: ', req);
    switch (req.type) {
      case DecoderRequest.kInitDecoderReq:
        this.initDecoder(req.data.s, req.data.c);
        break;
      case DecoderRequest.kUninitDecoderReq:
        this.uninitDecoder();
        break;
      case DecoderRequest.kOpenDecoderReq:
        this.openDecoder();
        break;
      case DecoderRequest.kCloseDecoderReq:
        this.closeDecoder();
        break;
      case DecoderRequest.kStartDecodingReq:
        this.startDecoding(req.data.i);
        break;
      case DecoderRequest.kPauseDecodingReq:
        this.pauseDecoding();
        break;
      case DecoderRequest.kFeedDataReq:
        this.sendData(req.data);
        break;
      case DecoderResponse.kSeekToRsp:
        this.seekTo(req.data.ms);
      default:
        this.logger.error("Unsupport messsage " + req.type);
    }
  }

  cacheReq(req: WorkerMessage) {
    if (req) {
      this.logger.info("Cache req : " + req.type);
      this.tmpReqQue.push(req);
    }
  }

  onWasmLoaded() {
    this.logger.info("Wasm loaded.");
    this.wasmLoaded = true;
    WasmDecoderModule.then((val) => {
      WasmDecoderModule = val;
      // console.log('then', val);
      this.videoCallback = WasmDecoderModule.addFunction(function(buff: number, size: number, timestamp: number) {
        let data = new Uint8Array(WasmDecoderModule.HEAPU8.subarray(buff, buff + size));
        postMessage(new WorkerMessage(DecoderResponse.kVideoFrame, { s: timestamp, d: data }), [data.buffer]);
      }, 'viid');
      this.audioCallback = WasmDecoderModule.addFunction(function (buff: number, size: number, timestamp: number) {
        let data = new Uint8Array(WasmDecoderModule.HEAPU8.subarray(buff, buff + size));
        postMessage(new WorkerMessage(DecoderResponse.kAudioFrame, { s: timestamp, d: data }), [data.buffer]);
      }, 'viid');

      this.requestCallback = WasmDecoderModule.addFunction(function (offset: number, available: number) {
        let objData: WorkerMessage = {
          type: DecoderResponse.kRequestDataEvt,
          data: {
            o: offset,
            a: available
          }
        };
        postMessage(objData);
      }, 'vii');

      while (this.tmpReqQue.length > 0) {
        let req = this.tmpReqQue.shift();
        this.processReq(req);
      }
    });
  }

  onWasmLoadedOld() {
    this.logger.info("Wasm loaded.");
    this.wasmLoaded = true;
    // WasmDecoderModule.then((val) => {
    //   WasmDecoderModule = val;
    //   // console.log('then', val);
    // });
    this.videoCallback = WasmDecoderModule.addFunction(function(buff: number, size: number, timestamp: number) {
      let data =new Uint8Array(WasmDecoderModule.HEAPU8.subarray(buff, buff + size));
      postMessage(new WorkerMessage(DecoderResponse.kVideoFrame, { s: timestamp, d: data }), [data.buffer]);
    }, 'viid');
    this.audioCallback = WasmDecoderModule.addFunction(function (buff: number, size: number, timestamp: number) {
      let data = new Uint8Array(WasmDecoderModule.HEAPU8.subarray(buff, buff + size));
      postMessage(new WorkerMessage(DecoderResponse.kAudioFrame, { s: timestamp, d: data }), [data.buffer]);
    }, 'viid');

    this.requestCallback = WasmDecoderModule.addFunction(function (offset: number, available: number) {
      let objData: WorkerMessage = {
        type: DecoderResponse.kRequestDataEvt,
        data: {
          o: offset,
          a: available
        }
      };
      postMessage(objData);
    }, 'vii');

    while (this.tmpReqQue.length > 0) {
      let req = this.tmpReqQue.shift();
      this.processReq(req);
    }
  }
}

let decoder = new Decoder;

function onWasmLoaded() {
  console.log("onWasmLoaded")
  if (decoder) {
    decoder.onWasmLoaded();
  } else {
    console.log("[ER] No decoder!");
  }
}

addEventListener('message', ({ data }) => {
  if (!decoder) {
    console.log("[ER] Decoder not initialized!");
    return;
  }

  let req: WorkerMessage = data;
  if (!decoder.wasmLoaded) {
    decoder.cacheReq(req);
    decoder.logger.info("Temp cache req " + req.type + ".");
    return;
  }
  decoder.processReq(req);
});
