import Logger from '../../logger';
import WebGLUtils from './WebGLUtils';
import PCMPlayer from './pcm-player';
import { DownloaderResponse, DecoderResponse, DownloaderRequest,
  DecoderRequest, DownloadProtocol, WorkerMessage } from './worker-message.model';
import { ElementRef } from '@angular/core';
import { ErrorMsg } from './error-msg.model';

const decoderStateIdle: number = 0;
const decoderStateInitializing: number = 1;
const decoderStateReady: number = 2;
const decoderStateFinished: number = 3;

const playerStateIdle: number = 0;
const playerStatePlaying: number = 1;
const playerStatePausing: number = 2;

const maxBufferTimeLength = 1.0;
const downloadSpeedByteRateCoef = 2.0;

class FileInfo {
  url: string;
  size: number;
  offset: number;
  chunkSize: number;

  constructor(url: string) {
    this.url = url;
    this.size = 0;
    this.offset = 0;
    this.chunkSize = 65536;
    // this.chunkSize = 188 * 300;
    // this.chunkSize = 9212000;
  }
}

export default class Player {
  fileInfo: FileInfo;
  pcmPlayer: PCMPlayer;
  canvas: ElementRef<HTMLCanvasElement>;
  webglPlayer: WebGLUtils;
  callback: any;
  waitHeaderLength: number;
  duration: number;
  pixFmt: number;
  videoWidth: number;
  videoHeight: number;
  yLength: number;
  uvLength: number;
  beginTimeOffset: number;
  decoderState: number;
  playerState: number;
  decoding: boolean;
  decodeInterval: number;
  videoRendererTimer: NodeJS.Timer;
  downloadTimer: NodeJS.Timer;
  chunkInterval: number;
  downloadSeqNo: number;
  downloading: boolean;
  downloadProtocol: number;
  timeLabel: HTMLLabelElement;
  timeTrack: HTMLProgressElement;
  trackTimer: NodeJS.Timer;
  trackTimerInterval: number;
  displayDuration: string;
  logger: Logger;
  downloadWorker: Worker;
  decodeWorker: Worker;
  isStream: boolean;
  streamPauseParam: { url: string; canvas: ElementRef<HTMLCanvasElement>; callback: any; waitHeaderLength: number; };
  seeking: boolean;
  frameBuffer: any[];
  buffering: boolean;
  streamReceivedLen: number;
  firstAudioFrame: boolean;
  urgent: boolean;
  seekReceivedLen: number;
  fetchController: AbortController;
  audioEncoding: string;
  audioChannels: number;
  audioSampleRate: number;
  justSeeked: boolean;
  seekWaitLen: number;
  loadingDiv: any;

  constructor() {
    this.fileInfo = null;
    this.pcmPlayer = null;
    this.canvas = null;
    this.webglPlayer = null;
    this.callback = null;
    this.waitHeaderLength = 524288;
    this.duration = 0;
    this.pixFmt = 0;
    this.videoWidth = 0;
    this.videoHeight = 0;
    this.yLength = 0;
    this.uvLength = 0;
    this.beginTimeOffset = 0;
    this.decoderState = decoderStateIdle;
    this.playerState = playerStateIdle;
    this.decoding = false;
    this.decodeInterval = 5;
    this.videoRendererTimer = null;
    this.downloadTimer = null;
    this.chunkInterval = 200;
    this.downloadSeqNo = 0;
    this.downloading = false;
    this.downloadProtocol = DownloadProtocol.kHttp;
    this.timeLabel = null;
    this.timeTrack = null;
    this.trackTimer = null;
    this.trackTimerInterval = 500;
    this.displayDuration = '00:00:00';
    this.audioEncoding = '';
    this.audioChannels = 0;
    this.audioSampleRate = 0;
    this.seeking = false;  // Flag to preventing multi seek from track.
    this.justSeeked = false;  // Flag to preventing multi seek from ffmpeg.
    this.urgent = false;
    this.seekWaitLen = 524288; // Default wait for 512K, will be updated in onVideoParam.
    this.seekReceivedLen = 0;
    this.loadingDiv = null;
    this.buffering = false;
    this.frameBuffer = [];
    this.isStream = false;
    this.streamReceivedLen = 0;
    this.firstAudioFrame = true;
    this.fetchController = null;
    this.streamPauseParam = null;
    this.logger = new Logger('Player');
    this.initDownloadWorker();
    this.initDecodeWorker();
  }

  initDownloadWorker() {
    const self = this;
    this.downloadWorker = new Worker('./download.worker', { type: 'module' });
    this.downloadWorker.onmessage = (evt: MessageEvent) => {
      const msg: WorkerMessage = evt.data;
      // console.error('download signal:' + msg.type);
      switch (msg.type) {
        case DownloaderResponse.kGetFileInfoRsp:
          self.onGetFileInfo(msg.data.i);
          break;
        case DownloaderResponse.kFileData:
          self.onFileData(msg.data.d, msg.data.s, msg.data.e, msg.data.q);
          break;
      }
    };
  }

  initDecodeWorker() {
    const self = this;
    this.decodeWorker = new Worker('./decode.worker', { type: 'module' });
    this.decodeWorker.onmessage = (evt: MessageEvent) => {
      const msg: WorkerMessage = evt.data;
      // console.error('decode signal:' + msg.type);
      switch (msg.type) {
        case DecoderResponse.kInitDecoderRsp:
          self.onInitDecoder(msg);
          break;
        case DecoderResponse.kOpenDecoderRsp:
          console.error('play: get open  decoder signal');
          self.onOpenDecoder(msg);
          break;
        case DecoderResponse.kVideoFrame:
          self.onVideoFrame(msg);
          break;
        case DecoderResponse.kAudioFrame:
          self.onAudioFrame(msg);
          break;
        case DecoderResponse.kDecodeFinishedEvt:
          self.onDecodeFinished(msg);
          break;
        case DecoderResponse.kRequestDataEvt:
          self.onRequestData(msg.data.o, msg.data.a);
          break;
        case DecoderResponse.kSeekToRsp:
          self.onSeekToRsp(msg.data.r);
          break;
      }
    };
  }

  play(url: string, canvas: ElementRef<HTMLCanvasElement>, callback: Function, waitHeaderLength: number, isStream: boolean): any {
    this.logger.info('Play ' + url + '.');

    let ret: ErrorMsg = {
      error: 0,
      msg: 'Success'
    };

    let success = true;
    do {
      if (this.playerState == playerStatePausing) {
        ret = this.resume();
        break;
      }
      if (this.playerState == playerStatePlaying) {
        break;
      }

      if (!url || !canvas || !this.downloadWorker || !this.decodeWorker) {
        ret.error = -1;
        ret.msg = 'Invalid Arguments';
        success = false;
        this.logger.error('play video error, invalid arguments');
        break;
      }

      if (url.startsWith('ws://') || url.startsWith('wss://')) {
        this.downloadProtocol = DownloadProtocol.kWebsocket;
      } else {
        this.downloadProtocol = DownloadProtocol.kHttp;
      }
      this.fileInfo = new FileInfo(url);
      this.canvas = canvas;
      this.callback = callback;
      this.waitHeaderLength = waitHeaderLength || this.waitHeaderLength;
      this.playerState = playerStatePlaying;
      this.isStream = isStream;
      this.startTrackTimer();
      this.displayLoop();

      this.webglPlayer = new WebGLUtils(this.canvas);
      this.webglPlayer.initGL(true);

      if (!this.isStream) {
        const req: WorkerMessage = {
          type: DownloaderRequest.kGetFileInfoReq,
          data: {
            u: url,
            p: this.downloadProtocol
          }
        };
        // console.error('play: start: download info');
        this.downloadWorker.postMessage(req);
      } else {
        this.requestStream(url);
        this.onGetFileInfo({
          sz: -1,
          st: 200
        });
      }

      const self = this;
      this.registerVisibilityEvent((visible: boolean) => {
        if (visible) {
          self.resume();
        } else {
          self.pause();
        }
      });
      this.buffering = true;
      this.showLoading();
    } while (false);

    return ret;
  }

  pauseStream() {
    if (this.playerState != playerStatePlaying) {
      return new ErrorMsg(-1, 'Not Playing');
    }
    this.streamPauseParam = {
      url: this.fileInfo.url,
      canvas: this.canvas,
      callback: this.callback,
      waitHeaderLength: this.waitHeaderLength
    };
    this.logger.info('Stop in stream pause');
    this.stop();

    return new ErrorMsg(0, 'Success');
  }

  pause(): ErrorMsg {
    this.logger.info('Pause.');

    if (this.isStream) {
      this.pauseStream();
    }

    if (this.playerState != playerStatePlaying) {
      return new ErrorMsg(-1, 'Not Playing');
    }
    // Pause video rendering and audio flushing.
    this.playerState = playerStatePausing;
    // Pause audio context.
    if (this.pcmPlayer) {
      this.pcmPlayer.pause();
    }
    // Pause decoding.
    this.pauseDecoding();
    // Stop track timer.
    this.stopTrackTimer();

    // Do not stop downloader for background buffering.
    return new ErrorMsg(0, 'Success');
  }

  resumeStream(): ErrorMsg {
    if (this.playerState != playerStateIdle || !this.streamPauseParam) {
      return new ErrorMsg(-1, 'Not Pausing');
    }
    this.logger.info('Play in stream resume.');
    this.play(this.streamPauseParam.url,
      this.streamPauseParam.canvas,
      this.streamPauseParam.callback,
      this.streamPauseParam.waitHeaderLength,
      true);
    this.streamPauseParam = null;

    return new ErrorMsg(0, 'Success');
  }

  resume(fromSeek: boolean = false): ErrorMsg {
    this.logger.info('Resume.');
    if (this.playerState != playerStatePausing) {
      return new ErrorMsg(-1, 'Not Pausing');
    }

    // Resume audio context.
    if (!fromSeek && this.pcmPlayer) {
      this.pcmPlayer.resume();
    }

    // If there's a flying video renderer op, interrupt it.
    if (this.videoRendererTimer != null) {
      clearTimeout(this.videoRendererTimer);
      this.videoRendererTimer = null;
    }
    // Restart video rendering and audio flushing.
    this.playerState = playerStatePlaying;
    // Restart decoding.
    this.startDecoding();
    // Restart track timer.
    if (!this.seeking) {
      this.startTrackTimer();
    }

    return new ErrorMsg(-1, 'Not Pausing');
  }

  stop(): ErrorMsg {
    this.logger.info('Stop.');
    if (this.playerState == playerStateIdle) {
      return new ErrorMsg(-1, 'Not Playing');
    }

    if (this.videoRendererTimer != null) {
      clearTimeout(this.videoRendererTimer);
      this.videoRendererTimer = null;
      this.logger.info('Video renderer timer stopped.');
    }

    this.stopDownloadTimer();
    this.stopTrackTimer();
    this.hideLoading();

    this.fileInfo = null;
    this.canvas = null;
    this.webglPlayer = null;
    this.callback = null;
    this.duration = 0;
    this.pixFmt = 0;
    this.videoWidth = 0;
    this.videoHeight = 0;
    this.yLength = 0;
    this.uvLength = 0;
    this.beginTimeOffset = 0;
    this.decoderState = decoderStateIdle;
    this.playerState = playerStateIdle;
    this.decoding = false;
    this.frameBuffer = [];
    this.buffering = false;
    this.streamReceivedLen = 0;
    this.firstAudioFrame = true;
    this.urgent = false;
    this.seekReceivedLen = 0;

    if (this.pcmPlayer) {
      this.pcmPlayer.destroy();
      this.pcmPlayer = null;
      this.logger.info('Pcm player released.');
    }

    if (this.timeTrack) {
      this.timeTrack.value = 0;
    }
    this.logger.info('Closing decoder.');
    this.decodeWorker.postMessage(new WorkerMessage(DecoderRequest.kCloseDecoderReq, null));

    this.logger.info('Uniniting decoder.');
    this.decodeWorker.postMessage(new WorkerMessage(DecoderRequest.kUninitDecoderReq, null));

    if (this.fetchController) {
      this.fetchController.abort();
      this.fetchController = null;
    }

    return new ErrorMsg(0, 'Success');
  }

  seekTo(ms: number) {
    if (this.isStream) {
      return;
    }

    // Pause playing.
    this.pause();

    // Stop download.
    this.stopDownloadTimer();

    // Clear frame buffer.
    this.frameBuffer.length = 0;

    // Request decoder to seek.
    this.decodeWorker.postMessage(new WorkerMessage(DecoderRequest.kSeekToReq, { ms: ms }));

    // Reset begin time offset.
    this.beginTimeOffset = ms / 1000;
    this.logger.info('seekTo beginTimeOffset ' + this.beginTimeOffset);

    this.seeking = true;
    this.justSeeked = true;
    this.urgent = true;
    this.seekReceivedLen = 0;
    this.startBuffering();
  }

  fullscreen() {
    if (this.webglPlayer) {
      this.webglPlayer.fullscreen();
    }
  }

  getState() {
    return this.playerState;
  }

  setTrack(timeTrack, timeLabel) {
    this.timeTrack = timeTrack;
    this.timeLabel = timeLabel;

    if (this.timeTrack) {
      const self = this;
      this.timeTrack.oninput = () => {
        if (!self.seeking) {
          self.seekTo(self.timeTrack.value);
        }
      };
      this.timeTrack.onchange = () => {
        if (!self.seeking) {
          self.seekTo(self.timeTrack.value);
        }
      };
    }
  }

  onGetFileInfo(info: any) {
    if (this.playerState == playerStateIdle) {
      return;
    }

    this.logger.info('Got file size rsp:' + info.st + ' size:' + info.sz + ' byte.');
    if (info.st == 200) {
      this.fileInfo.size = info.sz;
      this.logger.info('Initializing decoder.');
      const req: WorkerMessage = {
        type: DecoderRequest.kInitDecoderReq,
        data: {
          s: this.fileInfo.size,
          c: this.fileInfo.chunkSize
        }
      };
      this.decodeWorker.postMessage(req);
    } else {
      this.reportPlayError(-1, info.st, 'onGetFileInfo');
    }
  }

  onFileData(data: any, start: number, end: number, seq: number) {
    // this.logger.info('state ' + this.decoderState + ' Got data bytes=' + start + '-' + end + '.');
    this.downloading = false;
    if (this.playerState == playerStateIdle) {
      return;
    }
    if (seq != this.downloadSeqNo) {
      return;  // Old data.
    }
    if (this.playerState == playerStatePausing) {
      if (this.seeking) {
        this.seekReceivedLen += data.byteLength;
        const left = this.fileInfo.size - this.fileInfo.offset;
        const seekWaitLen = Math.min(left, this.seekWaitLen);
        if (this.seekReceivedLen >= seekWaitLen) {
          this.logger.info('Resume in seek now');
          setTimeout(() => {
            this.resume(true);
          }, 0);
        }
      } else {
        return;
      }
    }

    const len = end - start + 1;
    this.fileInfo.offset += len;
    this.decodeWorker.postMessage(new WorkerMessage(DecoderRequest.kFeedDataReq, data), [data]);
    // console.error('onFileData: send data', data, this.decoderState);
    switch (this.decoderState) {
      case decoderStateIdle:
        this.onFileDataUnderDecoderIdle();
        break;
      case decoderStateInitializing:
        this.onFileDataUnderDecoderInitializing();
        break;
      case decoderStateReady:
        this.onFileDataUnderDecoderReady();
        break;
    }
    if (this.urgent) {
      this.logger.info('test info: onFileData: start ' + start + ',end:' + end + ',seq:' + seq);
      setTimeout(() => {
        this.downloadOneChunk();
      }, 0);
    }
  }

  onFileDataUnderDecoderIdle() {
    if (this.fileInfo.offset >= this.waitHeaderLength) {
      this.logger.info('Opening decoder.');
      this.decoderState = decoderStateInitializing;
      // console.error('onFileDataUnderDecoderIdle:send open decode msg');
      this.decodeWorker.postMessage(new WorkerMessage(DecoderRequest.kOpenDecoderReq, null));
    }
    this.downloadOneChunk();
  }

  onFileDataUnderDecoderInitializing() {
    this.downloadOneChunk();
  }

  onFileDataUnderDecoderReady() { }

  onInitDecoder(msg: WorkerMessage) {
    if (this.playerState == playerStateIdle) {
      return;
    }

    this.logger.info('Init decoder response ' + msg.data.e + '.');
    if (msg.data.e == 0) {
      this.downloadOneChunk();
    } else {
      this.reportPlayError(msg.data.e, -1, 'onInitDecoder');
    }
  }

  onOpenDecoder(msg: WorkerMessage) {
    if (this.playerState == playerStateIdle) {
      return;
    }

    this.logger.info('Open decoder response ' + msg.data.e + '.');
    if (msg.data.e == 0) {
      this.onVideoParam(msg.data.v);
      this.onAudioParam(msg.data.a);
      this.decoderState = decoderStateReady;
      this.logger.info('Decoder ready now.');
      console.error('play:startDecoding func');
      this.startDecoding();
    } else {
      this.reportPlayError(msg.data.e, -1, 'onOpenDecoder');
    }
  }

  onVideoParam(v) {
    if (this.playerState == playerStateIdle) {
      return;
    }

    this.logger.info('Video param duation:' + v.d + ' pixFmt:' + v.p + ' width:' + v.w + ' height:' + v.h + '.');
    this.duration = v.d;
    this.pixFmt = v.p;
    // this.canvas.width = v.w;
    // this.canvas.height = v.h;
    this.videoWidth = v.w;
    this.videoHeight = v.h;
    this.yLength = this.videoWidth * this.videoHeight;
    this.uvLength = (this.videoWidth / 2) * (this.videoHeight / 2);

    if (this.timeTrack) {
      // TODO this.timeTrack.min = 0;
      this.timeTrack.max = this.duration;
      this.timeTrack.value = 0;
      this.displayDuration = this.formatTime(this.duration / 1000);
    }

    const byteRate = 1000 * this.fileInfo.size / this.duration;
    const targetSpeed = downloadSpeedByteRateCoef * byteRate;
    const chunkPerSecond = targetSpeed / this.fileInfo.chunkSize;
    this.chunkInterval = 1000 / chunkPerSecond;
    this.seekWaitLen = byteRate * maxBufferTimeLength * 2;
    this.logger.info('Seek wait len ' + this.seekWaitLen);

    if (!this.isStream) {
      this.startDownloadTimer();
    }

    this.logger.info('Byte rate:' + byteRate + ' target speed:' + targetSpeed + ' chunk interval:' + this.chunkInterval + '.');
  }

  onAudioParam(AudioConfig: { sampleFormat: number; channelCount: number; sampleRate: number; }) {
    if (this.playerState == playerStateIdle) {
      return;
    }

    this.logger.info('Audio param sampleFmt:' + AudioConfig.sampleFormat + ' channels:' + AudioConfig.channelCount
      + ' sampleRate:' + AudioConfig.sampleRate + '.');

    const sampleFmt: number = AudioConfig.sampleFormat;
    const channels: number = AudioConfig.channelCount;
    const sampleRate: number = AudioConfig.sampleRate;

    let encoding = '16bitInt';
    switch (sampleFmt) {
      case 0:
        encoding = '8bitInt';
        break;
      case 1:
        encoding = '16bitInt';
        break;
      case 2:
        encoding = '32bitInt';
        break;
      case 3:
        encoding = '32bitFloat';
        break;
      default:
        this.logger.error('Unsupported audio sampleFmt ' + sampleFmt + '!');
    }
    this.logger.info('Audio encoding ' + encoding + '.');

    this.pcmPlayer = new PCMPlayer({
      encoding: encoding,
      channels: channels,
      sampleRate: sampleRate,
      flushingTime: 5000
    });

    this.audioEncoding      = encoding;
    this.audioChannels      = channels;
    this.audioSampleRate    = sampleRate;
  }

  restartAudio() {
    if (this.pcmPlayer) {
      this.pcmPlayer.destroy();
      this.pcmPlayer = null;
    }

    this.pcmPlayer = new PCMPlayer({
      encoding: this.audioEncoding,
      channels: this.audioChannels,
      sampleRate: this.audioSampleRate,
      flushingTime: 5000
    });
  }

  bufferFrame(frame) {
    // If not decoding, it may be frame before seeking, should be discarded.
    if (!this.decoding) {
      return;
    }
    this.frameBuffer.push(frame);
    // tslint:disable-next-line: max-line-length
    // this.logger.info('bufferFrame ' + frame.data.s + ' buffering ' + this.buffering + ' decoding ' + this.decoding + ' len ' + this.getBufferTimerLength());
    if (this.getBufferTimerLength() >= maxBufferTimeLength || this.decoderState == decoderStateFinished) {
      if (this.decoding) {
        // this.logger.info('Frame buffer time length >= ' + maxBufferTimeLength + ', pause decoding.');
        this.pauseDecoding();
      }
      if (this.buffering) {
        this.stopBuffering();
      }
    }
  }


  displayAudioFrame(frame) {
    if (this.playerState != playerStatePlaying) {
      return false;
    }

    if (this.seeking) {
      this.restartAudio();
      this.startTrackTimer();
      this.hideLoading();
      this.seeking = false;
      this.urgent = false;
    }

    if (this.isStream && this.firstAudioFrame) {
      this.firstAudioFrame = false;
      this.beginTimeOffset = frame.s;
    }

    this.pcmPlayer.play(new Uint8Array(frame.d));
    return true;
  }

  onAudioFrame(frame) {
    this.bufferFrame(frame);
  }

  onDecodeFinished(objData) {
    this.pauseDecoding();
    this.decoderState = decoderStateFinished;
  }

  getBufferTimerLength() {
    if (!this.frameBuffer || this.frameBuffer.length == 0) {
      return 0;
    }

    const oldest = this.frameBuffer[0];
    const newest = this.frameBuffer[this.frameBuffer.length - 1];
    return newest.data.s - oldest.data.s;
  }

  onVideoFrame(frame) {
    this.bufferFrame(frame);
  }

  displayVideoFrame(frame) {
    if (this.playerState != playerStatePlaying) {
      // this.logger.info('displayVideoFrame return state ' + this.playerState);
      return false;
    }

    if (this.seeking) {
      this.restartAudio();
      this.startTrackTimer();
      this.hideLoading();
      this.seeking = false;
      this.urgent = false;
    }

    const audioCurTs = this.pcmPlayer.getTimestamp();
    const audioTimestamp = audioCurTs + this.beginTimeOffset;
    const delay = frame.s - audioTimestamp;

    // tslint:disable-next-line: max-line-length
    // this.logger.info('displayVideoFrame delay=' + delay + '=' + ' ' + frame.s  + ' - (' + audioCurTs  + ' + ' + this.beginTimeOffset + ')' + '->' + audioTimestamp);

    if (audioTimestamp <= 0 || delay <= 0) {
      let data = new Uint8Array(frame.d);
      this.renderVideoFrame(data);
      return true;
    }
    return false;
  }

  onSeekToRsp(ret: number) {
    if (ret != 0) {
      this.justSeeked = false;
      this.seeking = false;
    }
  }

  onRequestData(offset: number, available: number) {
    if (this.justSeeked) {
      this.logger.info('Request data ' + offset + ', available ' + available);
      if (offset == -1) {
        // Hit in buffer.
        let left = this.fileInfo.size - this.fileInfo.offset;
        if (available >= left) {
          this.logger.info('No need to wait');
          this.resume();
        } else {
          this.startDownloadTimer();
        }
      } else {
        if (offset >= 0 && offset < this.fileInfo.size) {
          this.fileInfo.offset = offset;
        }
        this.startDownloadTimer();
      }

      // this.restartAudio();
      this.justSeeked = false;
    }
  }

  displayLoop() {
    requestAnimationFrame(this.displayLoop.bind(this));
    if (this.playerState != playerStatePlaying) {
      // this.logger.info('display when not playing ' + this.playerState);
      return;
    }
    if (this.frameBuffer.length == 0) {
      return;
    }
    if (this.buffering) {
      return;
    }

    // requestAnimationFrame may be 60fps, if stream fps too large,
    // we need to render more frames in one loop, otherwise display
    // fps won't catch up with source fps, leads to memory increasing,
    // set to 2 now.
    for (let i = 0; i < 2; ++i) {
      const frame = this.frameBuffer[0];
      switch (frame.type) {
        case DecoderResponse.kAudioFrame:
          if (this.displayAudioFrame(frame.data)) {
            this.frameBuffer.shift();
          }
          break;
        case DecoderResponse.kVideoFrame:
          if (this.displayVideoFrame(frame.data)) {
            this.frameBuffer.shift();
          }
          break;
        default:
          return;
      }

      if (this.frameBuffer.length == 0) {
        break;
      }
    }

    if (this.getBufferTimerLength() < maxBufferTimeLength / 2) {
      if (!this.decoding) {
        // this.logger.info('Buffer time length < ' + maxBufferTimeLength / 2 + ', restart decoding.');
        this.startDecoding();
      }
    }
    if (this.bufferFrame.length == 0) {
      if (this.decoderState == decoderStateFinished) {
        this.reportPlayError(1, 0, 'Finished');
        this.stop();
      } else {
        this.startBuffering();
      }
    }
  }

  startBuffering() {
    this.buffering = true;
    this.showLoading();
    this.pause();
  }

  stopBuffering() {
    this.buffering = false;
    this.hideLoading();
    this.resume();
  }

  renderVideoFrame(data) {
    this.webglPlayer.renderVideoFrame(data, this.videoWidth, this.videoHeight, this.yLength, this.uvLength);
  }

  downloadOneChunk() {
    if (this.downloading || this.isStream) {
      return;
    }

    const start = this.fileInfo.offset;
    if (start >= this.fileInfo.size) {
      console.log(start, this.fileInfo.size);
      this.logger.error('Reach file end.');
      this.stopDownloadTimer();
      return;
    }

    let end = this.fileInfo.offset + this.fileInfo.chunkSize - 1;
    if (end >= this.fileInfo.size) {
      end = this.fileInfo.size - 1;
    }

    // console.error('downloadOneChunk, start:' + start + ', end:' + end);
    const len = end - start + 1;
    if (len > this.fileInfo.chunkSize) {
      console.log('Error: request len:' + len + ' > chunkSize:' + this.fileInfo.chunkSize);
      return;
    }

    const req: WorkerMessage = {
      type: DownloaderRequest.kDownloadFileReq,
      data: {
        u: this.fileInfo.url,
        s: start,
        e: end,
        q: this.downloadSeqNo,
        p: this.downloadProtocol
      }
    };
    this.downloadWorker.postMessage(req);
  }

  startDownloadTimer() {
    const self = this;
    this.downloadSeqNo++;
    this.downloadTimer = setInterval(() => {
      self.downloadOneChunk();
    }, this.chunkInterval);
  }

  stopDownloadTimer() {
    if (this.downloadTimer != null) {
      clearInterval(this.downloadTimer);
      this.downloadTimer = null;
    }
    this.downloading = false;
  }

  startTrackTimer() {
    const self = this;
    this.trackTimer = setInterval(() => {
      self.updateTrackTime();
    }, this.trackTimerInterval);
  }

  stopTrackTimer() {
    if (this.trackTimer != null) {
      clearInterval(this.trackTimer);
      this.trackTimer = null;
    }
  }

  updateTrackTime() {
    if (this.playerState == playerStatePlaying && this.pcmPlayer) {
      const currentPlayTime = this.pcmPlayer.getTimestamp() + this.beginTimeOffset;
      if (this.timeTrack) {
        this.timeTrack.value = 1000 * currentPlayTime;
      }
      if (this.timeLabel) {
        this.timeLabel.innerHTML = this.formatTime(currentPlayTime) + '/' + this.displayDuration;
      }
    }
  }

  startDecoding() {
    this.decodeWorker.postMessage(new WorkerMessage(
      DecoderRequest.kStartDecodingReq,
      { i: this.urgent ? 0 : this.decodeInterval }
    ));
    this.decoding = true;
  }

  pauseDecoding() {
    this.decodeWorker.postMessage(new WorkerMessage(DecoderRequest.kPauseDecodingReq, null));
    this.decoding = false;
  }

  formatTime(s: number) {
    const h = Math.floor(s / 3600) < 10 ? '0' + Math.floor(s / 3600) : Math.floor(s / 3600);
    const m = Math.floor((s / 60 % 60)) < 10 ? '0' + Math.floor((s / 60 % 60)) : Math.floor((s / 60 % 60));
    const s1 = Math.floor((s % 60)) < 10 ? '0' + Math.floor((s % 60)) : Math.floor((s % 60));
    return h + ':' + m + ':' + s1;
  }

  reportPlayError(error = 0, status = 0, message: string) {
    let e = {
      error: error || 0,
      status: status || 0,
      message: message
    };

    if (this.callback) {
      this.callback(e);
    }
  }

  setLoadingDiv(loadingDiv: any) {
    this.loadingDiv = loadingDiv;
  }

  hideLoading() {
    if (this.loadingDiv != null) {
      // loading.style.display = 'none';
    }
  }

  showLoading() {
    if (this.loadingDiv != null) {
      // loading.style.display = 'block';
    }
  }

  registerVisibilityEvent(cb) {
    var hidden = 'hidden';

    // Standards:
    if (hidden in document) {
      document.addEventListener('visibilitychange', onchange);
    } else if ((hidden = "mozHidden") in document) {
      document.addEventListener('mozvisibilitychange', onchange);
    } else if ((hidden = "webkitHidden") in document) {
      document.addEventListener('webkitvisibilitychange', onchange);
    } else if ((hidden = "msHidden") in document) {
      document.addEventListener('msvisibilitychange', onchange);
    } else if ('onfocusin' in document) {
      // IE 9 and lower.
      // TODO document.onfocusin = document.onfocusout = onchange;
    } else {
      // All others.
      window.onpageshow = window.onpagehide = window.onfocus = window.onblur = onchange;
    }

    function onchange(evt) {
      const v = true;
      const h = false;
      const evtMap = {
        focus: v,
        focusin: v,
        pageshow: v,
        blur: h,
        focusout: h,
        pagehide: h
      };

      evt = evt || window.event;
      let visible = v;
      if (evt.type in evtMap) {
        visible = evtMap[evt.type];
      } else {
        visible = this[hidden] ? h : v;
      }
      cb(visible);
    }

    // set the initial state (but only if browser supports the Page Visibility API)
    if (document[hidden] !== undefined) {
      onchange({ type: document[hidden] ? 'blur' : 'focus' });
    }
  }

  onStreamDataUnderDecoderIdle(length) {
    this.logger.info('in stream decoder');
    if (this.streamReceivedLen >= this.waitHeaderLength) {
      this.logger.info('stream');
      this.logger.info('Opening decoder.');
      this.decoderState = decoderStateInitializing;
      this.decodeWorker.postMessage(new WorkerMessage(DecoderRequest.kOpenDecoderReq, null));
    } else {
      this.streamReceivedLen += length;
    }
  }

  requestStream(url: string) {
    let self = this;
    this.fetchController = new AbortController();
    const signal = this.fetchController.signal;

    fetch(url, { signal }).then(async function respond(response) {
      const reader = response.body.getReader();
      reader.read().then(function processData({ done, value }) {
        if (done) {
          self.logger.info('Stream done.');
          return;
        }

        if (self.playerState != playerStatePlaying) {
          return;
        }

        var dataLength = value.byteLength;
        var offset = 0;
        if (dataLength > self.fileInfo.chunkSize) {
          do {
            let len = Math.min(self.fileInfo.chunkSize, dataLength);
            let data = value.buffer.slice(offset, offset + len);
            dataLength -= len;
            offset += len;
            self.decodeWorker.postMessage(new WorkerMessage(DecoderRequest.kFeedDataReq, data), [data]);
          } while (dataLength > 0)
        } else {
          self.decodeWorker.postMessage(new WorkerMessage(DecoderRequest.kFeedDataReq, value.buffer), [value.buffer]);
        }

        if (self.decoderState == decoderStateIdle) {
          self.onStreamDataUnderDecoderIdle(dataLength);
        }

        return reader.read().then(processData);
      });
    }).catch(err => {
    });
  }

}
