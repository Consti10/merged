/// <reference lib="webworker" />

import Logger from "src/app/logger";
import { DownloaderResponse, DownloaderRequest, DownloadProtocol, WorkerMessage } from './worker-message.model';

class Downloader {
  logger: Logger;
  ws: WebSocket;

  constructor() {
    this.logger = new Logger("Downloader");
    this.ws = null;
  }

  appendBuffer(buffer1: any, buffer2: any): ArrayBuffer {
    let tmp: Uint8Array = new Uint8Array(buffer1.byteLength + buffer2.byteLength);
    tmp.set(new Uint8Array(buffer1), 0);
    tmp.set(new Uint8Array(buffer2), buffer1.byteLength);
    return tmp.buffer;
  }

  reportFileSize(sz: number, st: number) {
    let objData: WorkerMessage = {
      type: DownloaderResponse.kGetFileInfoRsp,
      data: {
        i: {
          sz: sz,
          st: st
        }
      }
    };

    this.logger.info("File size " + sz + " bytes.");
    postMessage(objData);
  }

  reportData(start: number, end: number, seq: number, data: any) {
    let objData: WorkerMessage = {
      type: DownloaderResponse.kFileData,
      data: {
        s: start,
        e: end,
        d: data,
        q: seq
      }
    };
    postMessage(objData, [data]);
  }

  getFileInfoByHttp(url: string) {
    this.logger.info("Getting file size " + url + ".");
    let size: number = 0;
    let status: number = 0;
    let reported: boolean = false;

    let xhr = new XMLHttpRequest();
    xhr.open('get', url, true);
    let self = this;
    xhr.onreadystatechange = () => {
      let len = parseInt(xhr.getResponseHeader("Content-Length"), 10);
      if (len) {
        size = len;
      }
      if (xhr.status) {
        status = xhr.status;
      }
      //Completed.
      if (!reported && ((size > 0 && status > 0) || xhr.readyState == 4)) {
        self.reportFileSize(size, status);
        reported = true;
        xhr.abort();
      }
    };
    xhr.send();
  }

  downloadFileByHttp(url: string, start: number, end: number, seq: number) {
    //this.logger.info("Downloading file " + url + ", bytes=" + start + "-" + end + ".");
    let xhr = new XMLHttpRequest();
    xhr.open('get', url, true);
    xhr.responseType = 'arraybuffer';
    xhr.setRequestHeader("Range", "bytes=" + start + "-" + end);
    let self = this;
    xhr.onload = () => self.reportData(start, end, seq, xhr.response);
    xhr.send();
  }

  requestWebsocket(url: string, msg: string, cb: { onmessage: (ev: MessageEvent) => void; }) {
    if (this.ws == null) {
      this.ws = new WebSocket(url);
      this.ws.binaryType = 'arraybuffer';

      const self = this;
      this.ws.onopen = (evt: MessageEvent) => {
        self.logger.info("Ws connected.");
        self.ws.send(msg);
      };

      this.ws.onerror = (evt: MessageEvent) => {
        self.logger.error("Ws connect error " + evt.type);
      };

      this.ws.onmessage = cb.onmessage;
    } else {
      this.ws.onmessage = cb.onmessage;
      this.ws.send(msg);
    }
  }

  getFileInfoByWebsocket(url: string) {
    this.logger.info("Getting file size " + url + ".");

    // TBD, consider tcp sticky package.
    let data = null;
    let expectLength = 4;
    let self = this;
    let cmd = {
      url: url,
      cmd: "size",
    };
    this.requestWebsocket(url, JSON.stringify(cmd), {
      onmessage: function (evt: MessageEvent) {
        if (data != null) {
          data = self.appendBuffer(data, evt.data);
        } else if (evt.data.byteLength < expectLength) {
          data = evt.data.slice(0);
        } else {
          data = evt.data;
        }

        // Assume 4 bytes header as file size.
        if (data.byteLength == expectLength) {
          let int32array = new Int32Array(data, 0, 1);
          let size = int32array[0];
          self.reportFileSize(size, 200);
          // self.logger.logInfo("Got file size " + self.fileSize + ".");
        }
      }
    });
  }

  downloadFileByWebsocket(url: string, start: number, end: number, seq: number) {
    // this.logger.info("Downloading file " + url + ", bytes=" + start + "-" + end + ".");
    let data = null;
    let expectLength = end - start + 1;
    let self = this;
    let cmd = {
      url: url,
      cmd: "data",
      start: start,
      end: end
    };
    this.requestWebsocket(url, JSON.stringify(cmd), {
      onmessage: function (evt) {
        if (data != null) {
          data = self.appendBuffer(data, evt.data);
        } else if (evt.data.byteLength < expectLength) {
          data = evt.data.slice(0);
        } else {
          data = evt.data;
        }

        // Wait for expect data length.
        if (data.byteLength == expectLength) {
          self.reportData(start, end, seq, data);
        }
      }
    });
  }

  getFileInfo(protocol: DownloadProtocol, url: string) {
    switch (protocol) {
      case DownloadProtocol.kHttp:
        this.getFileInfoByHttp(url);
        break;
      case DownloadProtocol.kWebsocket:
        this.getFileInfoByWebsocket(url);
        break;
      default:
        this.logger.error("Invalid protocol " + protocol);
        break;
    }
  }

  downloadFile(protocol: DownloadProtocol, url: string, start: number, end: number, seq: number) {
    switch (protocol) {
      case DownloadProtocol.kHttp:
        this.downloadFileByHttp(url, start, end, seq);
        break;
      case DownloadProtocol.kWebsocket:
        this.downloadFileByWebsocket(url, start, end, seq);
        break;
      default:
        this.logger.error("Invalid protocol " + protocol);
        break;
    }
  }
}

const downloader = new Downloader();

addEventListener('message', ({ data }) => {
  if (!downloader) {
    // console.error("Downloader is not initialized");
  }
  let msg = data;
  switch (msg.type) {
    case DownloaderRequest.kGetFileInfoReq:
      downloader.getFileInfo(msg.data.p, msg.data.u);
      break;
    case DownloaderRequest.kDownloadFileReq:
      downloader.downloadFile(msg.data.p, msg.data.u, msg.data.s, msg.data.e, msg.data.q);
      break;
    case DownloaderRequest.kCloseDownloaderReq:
      //Nothing to do.
      break;
    default:
      downloader.logger.error("Unsupport messsage " + data.type);
  }
});
