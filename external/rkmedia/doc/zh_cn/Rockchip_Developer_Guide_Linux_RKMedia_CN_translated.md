# Rockchip RKMedia-Entwicklungsleitfaden

Datei-ID:

Release-Version: 0.0.1

Datum: 2020.8

Vertraulichkeit von Dokumenten: Öffentliche Informationen

---

Haftungsausschluss

Dieses Dokument wird "wie besehen" zur Verfügung gestellt, Rockchip Microelectronics Co., Ltd. ("das Unternehmen", das gleiche unten) macht keine Aussagen, Informationen und Inhalte dieses Dokuments zur Genauigkeit, Zuverlässigkeit, Vollständigkeit, Marktfähigkeit oder spezifischen Zweck und Nichtverletzung bieten ausdrückliche oder stillschweigende Erklärungen oder Garantien. Dieses Dokument dient nur als Referenz für Anweisungen.

Aufgrund von Produktversions-Upgrades oder aus anderen Gründen kann dieses Dokument von Zeit zu Zeit ohne vorherige Ankündigung aktualisiert oder geändert werden.

Markenerklärung

"Rockchip", "Rockchip" und "Rockchip" sind alle eingetragene Warenzeichen unseres Unternehmens und sind Eigentum unseres Unternehmens.

Alle anderen eingetragenen Warenzeichen oder Warenzeichen, die in diesem Dokument erwähnt werden können, sind Eigentum ihrer jeweiligen Inhaber.

Copyright © 2020 Rockchip Microelectronics Co., Ltd.

Außerhalb des Rahmens der fairen Nutzung und ohne schriftliche Genehmigung des Unternehmens darf keine Einheit oder Einzelperson den Inhalt dieses Dokuments ganz oder teilweise ohne Genehmigung extrahieren oder kopieren und in keiner Form verbreiten.

Rockchip Microelectronics Co., Ltd.

Rockchip Electronics Co., Ltd.

Adresse: Nr. 18, Area A, Software Park, Tongpan Road, Stadt Fuzhou, Provinz Fujian

Website: www.rock-chips.com

Kundenservice-Tel: +86-4007-700-590

Kundendienst-Fax: +86-591-83951833

Kundenservice-E-Mail: fae@rock-chips.com

---

##Vorwort

 Überblick

 Dieser Artikel beschreibt hauptsächlich die RKMedia-Medienentwicklungsreferenz

Produktversion

| Chipname | Kernelversion |
| ------------- | ------------ |
| RV1126/RV1109 | 4,19 |

Publikum

Dieses Dokument (dieses Handbuch) gilt hauptsächlich für die folgenden Ingenieure:

​ Technischer Support-Ingenieur

​ Softwareentwicklungsingenieur

 Revisionsdatensatz

| Datum | Version | Autor | Änderungsbeschreibung |
| ---------- | -------- | :------------ | ------------ |
| 28.08.2020 | V0.0.1 | Fan Lichuang / Yu Yongzhen | Erste Version |

---

## Inhalt

[Inhaltsverzeichnis]

---

## Systemübersicht

### Überblick

 RKMedia bietet eine Medienverarbeitungslösung, die die schnelle Entwicklung von Anwendungssoftware unterstützt. RKMedia kapselt außerdem die grundlegende API jedes Moduls, was die Schwierigkeit der Anwendungsentwicklung vereinfacht. Die Plattform unterstützt die folgenden Funktionen: VI (Eingangsvideoerfassung), VENC (H.265/H.264/JPEG/MJPEG-Kodierung), VDEC (H.265/H.264/JPEG, MJPEG-Dekodierung), VO (Videoausgabe Anzeige ), RGA-Videoverarbeitung (einschließlich Drehung, Skalierung, Zuschneiden) AI (Audioaufnahme), AO (Audioausgabe), AENC (Audiocodierung), ADEC (Audiodecodierung), MD (Bewegungserkennung), OD (Okklusionserkennung) .

### Systemstruktur

 ![](resource/system-1.png)

### Tabelle der Anzahl der Systemressourcen

| Modulname | Anzahl Kanäle |
| -------------- | -------- |
| VI | 4 |
| VENC | 16 |
| VDEC | 16 |
| AI | 1 |
| AO | 1 |
| AENC | 16 |
| ADEC | 16 |
| MD (Bewegungserkennung) | 4 |
| OD (Okklusionserkennung) | 4 |
| RGA | 16 |
| VO | 2 |

## Systemkontrolle

### Überblick

Das System steuert die Initialisierung des Basissystems und ist auch dafür verantwortlich, die Initialisierung und Deinitialisierung jedes Moduls abzuschließen, die Bindungsbeziehung jedes Geschäftsmoduls zu verwalten, die aktuelle Systemversion bereitzustellen und die Systemprotokollverwaltung.

### Bedienungsanleitung

#### Systembindung

RKMedia stellt eine Systembindungsschnittstelle (RK_MPI_SYS_Bind) bereit, die durch Bindung der Datenquelle an den Datenempfänger hergestellt wird
Die Beziehung zwischen den beiden (nur der Datenempfänger darf die Datenquelle binden). Nach dem Binden werden die von der Datenquelle generierten Daten
Wird automatisch an den Empfänger gesendet. Die derzeit unterstützten Bindungsbeziehungen sind in Tabelle 2-1 aufgeführt.

Tabelle 4-1 Von RKMedia unterstützte Bindungsbeziehung

| Datenquelle | Datenempfänger |
| ------ | ----------------- |
| VI | VO/RGA/VENC/MD/OD |
| VDEC | VO/RGA/VENC/MD/OD |
| RGA | VO/VENC/MD/OD |
| AI | AO/AENC |
| ADEC | AO |

### API-Referenz

#### RK_MPI_SYS_Init

​ [Beschreibung]

​ System initialisieren

​ [Syntax]

​ RK_S32 RK_MPI_SYS_Init();

#### RK_MPI_SYS_Bind

​ [Beschreibung]

​ Datenquelle-zu-Datenempfänger-Bindungsschnittstelle.

​ [Syntax]

​ RK_S32 RK_MPI_SYS_Bind(const MPP_CHN_S *pstSrcChn,const MPP_CHN_S *pstDestChn);

#### RK_MPI_SYS_UnBind

​ [Beschreibung]

​ Unbinding-Schnittstelle zwischen Datenquelle und Datenempfänger.

​ [Syntax]

​ RK_MPI_SYS_UnBind(const MPP_CHN_S *pstSrcChn,const MPP_CHN_S *pstDestChn);

#### RK_MPI_SYS_RegisterEventCb

​ [Beschreibung]

​ Ereignisrückrufe registrieren, z. B. Bewegungserkennungsereignisse

​ [Syntax]

​ RK_S32 RK_MPI_SYS_RegisterEventCb(const MPP_CHN_S *pstChn,EventCbFunc cb);

#### RK_MPI_SYS_RegisterOutCb

​ [Beschreibung]

​ Datenausgabe-Callback registrieren. Hinweis: Die Callback-Funktion kann keine zeitaufwendigen Operationen verarbeiten, da sonst der Datenstrom des entsprechenden Kanals blockiert wird.

​ [Syntax]

​ RK_S32 RK_MPI_SYS_RegisterOutCb(const MPP_CHN_S *pstChn, OutCbFunc cb);

#### RK_MPI_SYS_SendMediaBuffer

​ [Beschreibung]

​ Daten in den angegebenen Kanal eingeben, z. B. die lokale yuv-Datei zur Codierung an den Encoder senden

​ [Syntax]

​ RK_S32 RK_MPI_SYS_SendMediaBuffer(MOD_ID_E enModID, RK_S32 s32ChnID,

​ MEDIA_BUFFER-Puffer);

#### RK_MPI_SYS_GetMediaBuffer

​ [Beschreibung]

​ Daten vom angegebenen Kanal abrufen. Hinweis: Wenn diese Schnittstelle nicht rechtzeitig aufgerufen werden kann, kommt es zu Frameverlusten.

​ [Syntax]

​ RK_S32 RK_MPI_SYS_SendMediaBuffer(MOD_ID_E enModID, RK_S32 s32ChnID,
​ MEDIA_BUFFER-Puffer);

## Video Eingang

### Überblick

Die durch das Videoeingangsmodul (VI) realisierte Funktion: ISPP-Treiber realisiert Standard-V4L2-Geräte Durch die Kapselung der V4L2-API können ISPP-Mehrkanal-Videodaten gesammelt werden. Das VI speichert die empfangenen Daten im angegebenen Speicherbereich, um die Sammlung von Videodaten zu realisieren.

### Bedienungsanleitung

#### VI-Knotenname

Beim Erstellen des VIs muss der Name des Videoknotens angegeben werden, z. B. "/dev/video0". Auf der RV1126/RV1109-Plattform ist es spezieller und die entsprechenden Knotennamen lauten wie folgt

Tabelle 5-1 ISPP-Knotennamen

| ISPP-Knotenname |
| ------------- |
| ispp_bypass |
| ispp_scal0 |
| ispp_scal1 |
| ispp_scal2 |

#### VI-Arbeitsmodus

VI hat zwei Arbeitsmodi, wie in der folgenden Tabelle gezeigt

| Modusname | Makrodefinitionsname | Funktionsbeschreibung |
| -------- | ---------- | ----------------- ------------------------------------------------------- |
| Normalmodus | VI_WORK_MODE_NORMAL | Im Vergleich zum "Helligkeitsmodus" werden in diesem Modus die Kameradaten normalerweise gelesen und an die Folgestufe gesendet |
| Helligkeitsmodus | VI_WORK_MODE_LUMA_ONLY | Im Helligkeitsmodus wird VI nur für Helligkeitsstatistiken verwendet. Zu diesem Zeitpunkt kann das VI-Modul keine Daten über die Callback-Funktion oder RK_MPI_SYS_GetMediaBuffer abrufen. |

### API-Referenz

#### RK_MPI_VI_EnableChn

​ [Beschreibung]
VI-Kanal aktivieren

​ [Syntax]

​ RK_S32 RK_MPI_VI_EnableChn(VI_PIPE ViPipe, VI_CHN ViChn);

#### RK_MPI_VI_DisableChn

​ [Beschreibung]

​ VI-Kanal schließen

​ [Syntax]

​ RK_S32 RK_MPI_VI_DisableChn(VI_PIPE ViPipe, VI_CHN ViChn);

#### RK_MPI_VI_SetChnAttr

​ [Beschreibung]

​ VI-Kanaleigenschaften festlegen

​ [Syntax]

​ RK_MPI_VI_SetChnAttr(VI_PIPE ViPipe, VI_CHN ViChn,const VI_CHN_ATTR_S *pstChnAttr);

#### RK_MPI_VI_GetChnRegionLuma

​ [Beschreibung]

​ Informationen zur Helligkeit des Bereichs abrufen. Diese Schnittstelle unterstützt das Komprimierungsformat FBC0/FBC2 nicht.

​ [Syntax]

​ RK_S32 RK_MPI_VI_GetChnRegionLuma(VI_PIPE ViPipe, VI_CHN ViChn, const

​ VIDEO_REGION_INFO_S *pstRegionInfo, RK_U64 *pu64LumaData, RK_S32 s32MilliSec);

##Videokodierung

### Überblick

VENC-Modul, nämlich Videokodierungsmodul. Dieses Modul unterstützt Mehrkanal-Echtzeitkodierung, und jeder Kanal ist unabhängig von der Kodierung.
Das Protokoll und das Codierungsprofil können unterschiedlich sein. Wenn Sie die Videocodierung unterstützen, senden Sie das Region-Modul an das codierte Bild
Inhalte werden überlagert und verdeckt. Unterstützt H264/H1265/MJPEG/JPEG-Codierung.

### Bedienungsanleitung

#### Datenflussdiagramm

![](resource/venc-1.png)

Hinweis: Die im gestrichelten Kasten beschriebenen Funktionen sind optional und werden nur ausgelöst, wenn der Encoder entsprechend konfiguriert ist.

#### Ratenkontrolle

| Encodertyp | Unterstützungscode-Steuerungstyp |
| ---------- | ------------ |
| H265 | CBR / VBR |
| H264 | CBR / VBR |
| MJPEG | CBR / VBR |

#### GOP-Modus

Der GOP-Modus wird verwendet, um die Abhängigkeit des Referenzrahmens anzupassen und unterstützt derzeit die folgenden Modi. Hinweis: Es kann je nach Bedarf angepasst werden.

| Name | Makrodefinition | Beschreibung |
| ---------------- | -------------------- | ----------- -------------------------------------------------- |
| Normaler Modus | VENC_GOPMODE_NORMALP | Die häufigste Szene, eine, die ich für jede GopSize einrahme |
| Smart P-Frame-Modus | VENC_GOPMODE_SMARTP | Ein virtuelles I-Frame für jede GopSize und ein I-Frame für jedes BgInterval |
| Mehrschichtiger Zeitbereichsreferenzmodus | VENC_GOPMODE_TSVC | Die Codierungsabhängigkeit ist in mehrere Schichten unterteilt, und die Schichtinformationen können gemäß RK_MPI_MB_GetTsvcLevel abgerufen werden, um den Codestrom anzupassen. Wenn Sie beispielsweise nur den Stream der 0. Ebene abspielen, kann eine schnelle Vorschau realisiert werden. |

#### Interessengebiet (ROI)

Durch Konfigurieren des interessierenden Bereichs des Encoders kann der QP für den angegebenen Bereich angepasst werden. Zum Beispiel eine zum Korridor gerichtete Linse, die den Benutzer wirklich an der Mitte des Korridors interessiert. ROI kann so konfiguriert werden, dass die Kodierungsqualität in der Mitte des Korridors höher und das Bild klarer wird und die Bildqualität des nicht interessierenden Bereichs des Korridors (Wand, Decke usw.) niedriger ist. Auf diese Weise wird das Interessengebiet des Benutzers hervorgehoben, während die Bitrate im Wesentlichen unverändert bleibt.

Das System bietet 8 Regionen von Interesse, und die Priorität steigt von REGION_ID_0 auf REGION_ID_7. In Bereichen, in denen sich mehrere ROIs überschneiden, wird die QP-Strategie entsprechend den Bereichen mit hoher Priorität konfiguriert.

```
  REGION_ID_0
  REGION_ID_1
  REGION_ID_2
  REGION_ID_3
  REGION_ID_4
  REGION_ID_5
  REGION_ID_6
  REGION_ID_7
```

#### Drehung

Der Encoder unterstützt 4 Rotationsarten, 0°, 90°, 180°, 270°. Die Encoderrotation unterstützt derzeit das FBC-Format nicht und die FBC-Formatrotation muss durch die ISPP-Rotation erreicht werden.

### API-Referenz

#### RK_MPI_VENC_CreateChn

​ [Beschreibung]

​ Kodierungskanal erstellen

​ [Syntax]

​ RK_MPI_VENC_CreateChn(VENC_CHN VeChn,VENC_CHN_ATTR_S *stVencChnAttr);

#### RK_MPI_VENC_DestroyChn

​ [Beschreibung]

​ Zerstöre den Codierungskanal

​ [Syntax]

​ RK_S32 RK_MPI_VENC_DestroyChn(VENC_CHN VeChn);

#### RK_MPI_VENC_SetRcParam

​ [Beschreibung]

​ Parameter für die Geschwindigkeitsregelung einstellen

​ [Syntax]

​ RK_MPI_VENC_SetRcParam(VENC_CHN VeChn,const VENC_RC_PARAM_S *pstRcParam);

#### RK_MPI_VENC_SetRcMode

​ [Beschreibung]

​ Ratensteuerungsmodus einstellen

​ [Syntax]

​ RK_S32 RK_MPI_VENC_SetRcMode(VENC_CHN VeChn, VENC_RC_MODE_E RcMode);

#### RK_MPI_VENC_SetRcQuality

​ [Beschreibung]

​ Legen Sie die Kodierungsqualität fest. Wird für H264/H265-Encoder verwendet.

​ [Syntax]

​ RK_MPI_VENC_SetRcQuality(VENC_CHN VeChn,VENC_RC_QUALITY_E RcQuality);

#### RK_MPI_VENC_SetBitrate

​ [Beschreibung]

​ Bitrate einstellen

​ [Syntax]

​ RK_MPI_VENC_SetBitrate(VENC_CHN VeChn, RK_U32 u32BitRate, RK_U32 u32MinBitRate,

​ RK_U32 u32MaxBitRate);

#### RK_MPI_VENC_RequestIDR

​ [Beschreibung]

​ IDR-Rahmen anfordern. Nach Aufruf dieser Schnittstelle aktualisiert der Encoder sofort den IDR-Frame.

​ [Syntax]

​ RK_S32 RK_MPI_VENC_RequestIDR(VENC_CHN VeChn, RK_BOOL bInstant);

#### RK_MPI_VENC_SetFps

​ [Beschreibung]

​ Stellen Sie die Codierungsbildrate ein. Hinweis: Die Ausgabebildrate kann nicht höher sein als die Eingabebildrate.

​ [Syntax]

​ RK_S32 RK_MPI_VENC_SetFps(VENC_CHN VeChn, RK_U8 u8OutNum, RK_U8 u8OutDen,

​ RK_U8 u8InNum, RK_U8 u8InDen);

#### RK_MPI_VENC_SetGop

​ [Beschreibung]

​ GOP setzen. Wird für H264/H265-Encoder verwendet.

【Grammatik】

​ RK_S32 RK_MPI_VENC_SetGop(VENC_CHN VeChn, RK_U32 u32Gop);

#### RK_MPI_VENC_SetAvcProfile

​ [Beschreibung]

Profil einstellen. Wird für H264-Encoder verwendet.

​ [Syntax]

​ RK_MPI_VENC_SetAvcProfile(VENC_CHN VeChn, RK_U32 u32Profile,RK_U32 u32Level);

#### RK_MPI_VENC_InsertUserData

​ [Beschreibung]

​ Benutzerdaten einfügen, die eingefügten Daten werden im SEI-Paket des Code-Streams widergespiegelt. Wird für H264/H265-Encoder verwendet.

​ [Syntax]

​ RK_MPI_VENC_InsertUserData(VENC_CHN VeChn, RK_U8 *pu8Data,RK_U32 u32Len);

#### RK_MPI_VENC_SetRoiAttr

​ [Beschreibung]

​ Legen Sie den ROI-Codierungsbereich von Interesse fest. Wird für H264/H265-Encoder verwendet.

​ [Syntax]

​ RK_MPI_VENC_SetRoiAttr(VENC_CHN VeChn,const VENC_ROI_ATTR_S *pstRoiAttr);

#### RK_MPI_VENC_SetGopMode

​ [Beschreibung]

​ GopMode einstellen. Wird für H264/H265-Encoder verwendet.

​ [Syntax]

​ RK_S32 RK_MPI_VENC_SetGopMode(VENC_CHN VeChn, VENC_GOP_MODE_E GopMode);

#### RK_MPI_VENC_InitOsd

​ [Beschreibung]

​ OSD initialisieren. Bevor Sie RK_MPI_VENC_SetBitMap oder RK_MPI_VENC_RGN_SetCover aufrufen, müssen Sie

​ Rufen Sie diese Schnittstelle zuerst auf, und jeder Kodierungskanal kann nur einmal aufgerufen werden.

​ [Syntax]

​ RK_S32 RK_MPI_VENC_InitOsd(VENC_CHN VeChn);

#### RK_MPI_VENC_SetBitMap

​ [Beschreibung]

​ OSD-Bitmap einstellen

​ [Syntax]

​ RK_S32 RK_MPI_VENC_SetBitMap(VENC_CHN VeChn,const OSD_REGION_INFO_S *

​ pstRgnInfo, const BITMAP_S *pstBitmap);

#### RK_MPI_VENC_RGN_SetCover

​ [Beschreibung]

​ Privatsphärenmaske einstellen

​ [Syntax]

​ RK_S32 RK_MPI_VENC_RGN_SetCover(VENC_CHN VeChn, const OSD_REGION_INFO_S

​ *pstRgnInfo, const COVER_INFO_S *pstCoverInfo);

#### RK_MPI_VENC_SetJpegParam

​ [Beschreibung]

​ JPEG-Kodierungsparameter einstellen

​ [Syntax]

​ RK_S32 RK_MPI_VENC_SetJpegParam(VENC_CHN VeChn, const VENC_JPEG_PARAM_S

​ *pstJpegParam);

#### RK_MPI_VENC_StartRecvFrame

​ [Beschreibung]

​ Stellen Sie die Anzahl der vom Encoder empfangenen Frames ein. Der standardmäßig erstellte Encoder empfängt weiterhin VI-Daten über

​ RK_MPI_VENC_StartRecvFrame Schnittstelle kann die Anzahl der empfangenen Frames einstellen, nach Erreichen der angegebenen Anzahl stoppt der Encoder

​ Schlafen Sie bis zum nächsten Aufruf dieser Schnittstelle, um die Anzahl der empfangenen Frames zu ändern.

​ [Syntax]

​ RK_S32 RK_MPI_VENC_StartRecvFrame(VENC_CHN VeChn, const

​ VENC_RECV_PIC_PARAM_S *pstRecvParam);

## Bewegungserkennung

### Überblick

Das Bewegungserkennungsmodul (MD) realisiert die Bewegungsbereichserkennung und unterstützt maximal 4096 Bereiche.

### Bedienungsanleitung

Der MD-Algorithmus ist per Software implementiert, und die Eingangsauflösung ist nicht einfach zu groß.Die typische Auflösung beträgt 640 x 480. Je größer die Auflösung, desto höher die CPU-Last.

### API-Referenz

#### RK_MPI_ALGO_MD_CreateChn

​ [Beschreibung]

​ MD-Kanal erstellen

​ [Syntax]

​ RK_S32 RK_MPI_ALGO_MD_CreateChn(ALGO_MD_CHN MdChn);

#### RK_MPI_ALGO_MD_DestroyChn

​ [Beschreibung]

​ Zerstöre den MD-Kanal

​ [Syntax]

​ RK_S32 RK_MPI_ALGO_MD_DestroyChn(ALGO_MD_CHN MdChn);

#### RK_MPI_ALGO_MD_SetChnAttr

​ [Beschreibung]

​ MD-Attribute festlegen

​ [Syntax]

​ RK_S32 RK_MPI_ALGO_MD_SetChnAttr(ALGO_MD_CHN MdChn,

​ const ALGO_MD_ATTR_S *pstChnAttr);

## Bewegungserkennung

### Überblick

Das Okklusionserkennungsmodul implementiert Okklusionsalarme und unterstützt bis zu 10 Bereiche.

### Bedienungsanleitung

Der OD-Algorithmus wird per Software implementiert, und die Eingabeauflösung ist nicht einfach zu groß.Die typische Auflösung beträgt 640 x 480. Je größer die Auflösung, desto höher die CPU-Last.

### API-Referenz

#### RK_MPI_ALGO_OD_CreateChn

​ [Beschreibung]

​ OD-Kanal erstellen

​ [Syntax]

​ RK_S32 RK_MPI_ALGO_OD_CreateChn(ALGO_OD_CHN OdChn, konstant ALGO_OD_ATTR_S

​ *pstChnAttr);

#### RK_MPI_ALGO_OD_DestroyChn

​ [Beschreibung]

​ Zerstöre den OD-Kanal

​ [Syntax]

​ RK_S32 RK_MPI_ALGO_OD_DestroyChn(ALGO_OD_CHN OdChn);

## Audio

### Überblick

Das AUDIO-Modul umfasst vier Untermodule: Audioeingang, Audioausgang, Audiocodierung und Audiodecodierung. Audioeingang und
Das Ausgangsmodul ist mit Linux ALSA Audio verbunden
Die Kapselung des Ports realisiert Audio-Ein- und Ausgabefunktionen. Audiokodierung und -dekodierung
Das Modul wird durch Kapselung des ffmpeg-Audio-Encoders realisiert. Unterstützt G711A/G711U/G726/MP2.

### Bedienungsanleitung

#### Audioeingang und -ausgang

Audioeingang AI-Ausgang AO, wird zum Andocken mit Audio-Codec verwendet, um die Tonaufnahme und -wiedergabe abzuschließen. RKMedia AI/AO setzt auf Linux ALSA-Geräte.Verschiedene Soundkarten können die AI/AO-Schnittstelle nutzen, sofern sie den ALSA-Treiber unterstützen. In die KI sind Audioalgorithmen integriert, die durch Konfiguration eingeschaltet werden können. Nach dem Einschalten des Algorithmus gibt AI die vom Algorithmus verarbeiteten PCM-Daten aus.

#### Audio-Codec

Audio-Codec wird durch die Kapselung von ffmpeg realisiert und unterstützt derzeit G711A/G711U/G726/MP2.

#### Audioalgorithmus

Derzeit unterstützt es den AEC-Algorithmus für die Intercom-Szene und den ANR-Algorithmus für die Aufnahmeszene.

### API-Referenz

#### RK_MPI_AI_EnableChn

​ [Beschreibung]

​ KI-Kanal öffnen

​ [Syntax]

​ RK_S32 RK_MPI_AI_EnableChn(AI_CHN AiChn);

#### RK_MPI_AI_DisableChn

​ [Beschreibung]

​ KI-Kanal schließen

​ [Syntax]

​ RK_S32 RK_MPI_AI_DisableChn(AI_CHN AiChn);

#### RK_MPI_AI_SetChnAttr

​ [Beschreibung]

​ AO-Kanaleigenschaften einstellen

​ [Syntax]

​ RK_S32 RK_MPI_AI_SetChnAttr(AI_CHN AiChn, const AI_CHN_ATTR_S *pstAttr);

#### RK_MPI_AI_SetVolume

​ [Beschreibung]

​ Lautstärke einstellen

​ [Syntax]

​ RK_S32 RK_MPI_AI_SetVolume(AI_CHN AiChn, RK_S32 s32Volume);

#### RK_MPI_AI_GetVolume

​ [Beschreibung]

​ Volumen erhalten

​ [Syntax]

​ RK_S32 RK_MPI_AI_GetVolume(AI_CHN AiChn, RK_S32 *ps32Volume);

#### RK_MPI_AO_EnableChn

​ [Beschreibung]

​ AO-Kanal öffnen

​ [Syntax]

​ RK_S32 RK_MPI_AO_EnableChn(AO_CHN AoChn);

#### RK_MPI_AO_DisableChn

​ [Beschreibung]

​ Schließen Sie den AO-Kanal

​ [Syntax]

​ RK_S32 RK_MPI_AO_DisableChn(AO_CHN AoChn);

#### RK_MPI_AO_SetChnAttr

​ [Beschreibung]

​ AO-Kanaleigenschaften einstellen

​ [Syntax]

​ RK_S32 RK_MPI_AO_SetChnAttr(AO_CHN AoChn, const AO_CHN_ATTR_S *pstAttr);

#### RK_MPI_AO_SetVolume

​ [Beschreibung]

​ Lautstärke einstellen

​ [Syntax]

​ RK_S32 RK_MPI_AO_SetVolume(AO_CHN AoChn, RK_S32 s32Volume);

#### RK_MPI_AO_GetVolume

​ [Beschreibung]

​ Volumen erhalten

​ [Syntax]

​ RK_S32 RK_MPI_AO_GetVolume(AO_CHN AoChn, RK_S32 *ps32Volume);

#### RK_MPI_AENC_CreateChn

​ [Beschreibung]

​ Audiokodierungskanal erstellen

​ [Syntax]

​ RK_MPI_AENC_CreateChn(AENC_CHN AencChn,const AENC_CHN_ATTR_S *pstAttr);

#### RK_MPI_AENC_DestroyChn

​ [Beschreibung]

​ Zerstöre den Audiokodierungskanal

​ [Syntax]

​ RK_S32 RK_MPI_AENC_DestroyChn(AENC_CHN AencChn);

#### RK_MPI_ADEC_CreateChn

​ [Beschreibung]

​ Audio-Dekodierungskanal erstellen

​ [Syntax]

​ RK_S32 RK_MPI_ADEC_CreateChn(ADEC_CHN AdecChn, const ADEC_CHN_ATTR_S

​ *pstAttr);

#### RK_MPI_ADEC_DestroyChn

​ [Beschreibung]

​ Audio-Dekodierungskanal zerstören

​ [Syntax]

​ RK_S32 RK_MPI_ADEC_DestroyChn(ADEC_CHN AdecChn);

## RGA

### Überblick

Das RGA-Modul wird für 2D-Bildbeschneidung, Formatkonvertierung, Skalierung, Drehung, Bildüberlagerung usw. verwendet.

### Bedienungsanleitung

Der RGA-Kanal in rkmedia unterstützt nur Formatkonvertierung, Zoomen, Zuschneiden und Rotationsfunktionen Für Bildüberlagerung muss die Bibliothek librga.so separat aufgerufen werden, siehe "Rockchip_Developer_Guide_Linux_RGA_CN.pdf"

### API-Referenz

#### RK_MPI_RGA_CreateChn

​ [Beschreibung]

​ RGA-Kanal erstellen

​ [Syntax]

​ RK_S32 RK_MPI_RGA_CreateChn(RGA_CHN RgaChn, RGA_ATTR_S *pstRgaAttr);

#### RK_MPI_RGA_DestroyChn

​ [Beschreibung]

​ Zerstöre den RGA-Kanal

​ [Syntax]

​ RK_S32 RK_MPI_RGA_DestroyChn(RGA_CHN RgaChn);

## Videoanzeige

### Überblick

Das VO-Modul wird für die Verwaltung der Videoausgabe verwendet.

### Bedienungsanleitung

Das VO-Modul ist eine Kapselung von DRM/KMS und unterstützt Multi-VOP und Multi-Layer-Display.

### API-Referenz

#### RK_MPI_VO_CreateChn

​ [Beschreibung]

VO-Kanal erstellen

​ [Syntax]

​ RK_S32 RK_MPI_VO_CreateChn(VO_CHN VoChn, const VO_CHN_ATTR_S *pstAttr);

#### RK_MPI_VO_DestroyChn

​ [Beschreibung]

​ VO-Kanal zerstören

​ [Syntax]

​ RK_S32 RK_MPI_VO_DestroyChn(VO_CHN VoChn);