// Player request.
export enum PlayerRequest {
  kPlayVideoReq = 0,
  kPauseVideoReq = 1,
  kStopVideoReq = 2
}

// Player response.
export enum PlayerResponse {
  kPlayVideoRsp = 0,
  kAudioInfo = 1,
  kVideoInfo = 2,
  kAudioData = 3,
  kVideoData = 4
}

export enum DownloadProtocol {
  kWebsocket = 0,
  kHttp = 1
}

// Downloader request.
export enum DownloaderRequest {
  kGetFileInfoReq = 0,
  kDownloadFileReq = 1,
  kCloseDownloaderReq = 2
}

// Downloader response.
export enum DownloaderResponse {
 kGetFileInfoRsp = 0,
 kFileData = 1
}

// Decoder request.
export enum DecoderRequest {
  kInitDecoderReq = 0,
  kUninitDecoderReq = 1,
  kOpenDecoderReq = 2,
  kCloseDecoderReq = 3,
  kFeedDataReq = 4,
  kStartDecodingReq = 5,
  kPauseDecodingReq = 6,
  kSeekToReq = 7
}

// Decoder response.
export enum DecoderResponse {
  kInitDecoderRsp = 0,
  kUninitDecoderRsp = 1,
  kOpenDecoderRsp = 2,
  kCloseDecoderRsp = 3,
  kVideoFrame = 4,
  kAudioFrame = 5,
  kStartDecodingRsp = 6,
  kPauseDecodingRsp = 7,
  kDecodeFinishedEvt = 8,
  kRequestDataEvt = 9,
  kSeekToRsp = 10
}

export class WorkerMessage {
  type: number;
  data: any;

  constructor(type: number, data: any) {
    this.type = type;
    this.data = data;
  }

  public static getInstance(value: any): WorkerMessage {
    const { type, data } = value;
    return new WorkerMessage(type, data);
  }
}
