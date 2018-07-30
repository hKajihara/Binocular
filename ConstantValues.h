#ifndef CONSTANTVALUES_H
#define CONSTANTVALUES_H

//#define VISION128
//#define VISION100
//#define VISION064
//#define QVGA
#define QQVGA

// 128x128の時
#ifdef VISION128
#define IMAGE_DATA_LENGTH		16384			// 画素数
#define USB_BUF_SIZE			16896			// 画像一面を受信するために利用するバッファバイト長
#define NUM_OF_PACKET			33				// １フレームのパケット数
#define IMAGE_WIDTH				128				// 画像の幅
#define IMAGE_HEIGHT			128				// 画像の高さ
#define ATTACHED_DATA_LENGTH	380				// 最終パケットの余分バイト長
#endif

// 100x100の時
#ifdef VISION100
#define IMAGE_DATA_LENGTH		10000			// 画素数
#define USB_BUF_SIZE			10240			// 画像一面を受信するために利用するバッファバイト長
#define NUM_OF_PACKET			20				// １フレームのパケット数
#define IMAGE_WIDTH				100				// 画像の幅
#define IMAGE_HEIGHT			100				// 画像の高さ
#define ATTACHED_DATA_LENGTH	160				// 最終パケットの余分バイト長
#endif

// 64x64の時
#ifdef VISION064
#define IMAGE_DATA_LENGTH		4096			// 画素数
#define USB_BUF_SIZE			4608			// 画像一面を受信するために利用するバッファバイト長
#define NUM_OF_PACKET			9				// １フレームのパケット数
#define IMAGE_WIDTH				64				// 画像の幅
#define IMAGE_HEIGHT			64				// 画像の高さ
#define ATTACHED_DATA_LENGTH	476				// 最終パケットの余分バイト長
#endif

// QVGAの時
#ifdef QVGA
#define IMAGE_DATA_LENGTH		76800			// 画素数
#define USB_BUF_SIZE			77824			// 画像一面を受信するために利用するバッファバイト長
#define NUM_OF_PACKET			152				// １フレームのパケット数
#define IMAGE_WIDTH				320				// 画像の幅
#define IMAGE_HEIGHT			240				// 画像の高さ
#define ATTACHED_DATA_LENGTH	416				// 最終パケットの余分バイト長
#endif

// QQVGAの時
#ifdef QQVGA
#define IMAGE_DATA_LENGTH		19200			// 画素数
#define USB_BUF_SIZE			19456			// 画像一面を受信するために利用するバッファバイト長
#define NUM_OF_PACKET			38				// １フレームのパケット数
#define IMAGE_WIDTH				160				// 画像の幅
#define IMAGE_HEIGHT			120				// 画像の高さ
#define ATTACHED_DATA_LENGTH	104				// 最終パケットの余分バイト長
#endif

// 通信関連の定数
#define MAX_NUM_OF_RECV_IMG		8				// 受信可能な最大イメージ数				
#define USB_PACKET_LENGTH		512				// USBパケットのバイト長
#define USB_DATA_LENGTH			508				// USBパケットのデータバイト長
#define USB_HEADER_LENGTH		4				// USBパケットのヘッダバイト長
#define NUM_OF_SEND_DATA		100				// 送信パケットのデータ数
#define	SEND_DATA_LIMIT			255				// 送信データの最大値(1 - 255)
#define	NUM_OF_TEMP_BUF			8				// イメージ一つあたりの一時バッファ数

// 表示、処理関連の定数
#define DISPLAY_RATE			1				// 描画レート
#define IDB_BITMAP				IDB_BITMAP8		// 1:100, 2:200, 3:300, 4:128, 5:256, 6:384, 7:QQVGA, 8:QVGA
#define PIXEL_SIZE				2				// １ピクセルの大きさ(1から5で指定)
#define IMAGE_INTERVAL			5				// 画像間のピクセル数
#define NUM_OF_ROW_DISPLAY		4				// 水平方向の画面数
#define NUM_OF_COL_DISPLAY		3				// 垂直方向の画面数
#define NUM_OF_DISPLAY			NUM_OF_ROW_DISPLAY * NUM_OF_COL_DISPLAY
#define NUM_OF_PROCESS_BUF		40				// 利用するcomputedImageの数
#define FRAMES_TO_PROC			8				// ImageProcで利用するフレーム数(1だと現時刻のみ)
#define LOAD_BUF_LENGTH			1800			// 録画再生時に利用するバッファ長
#define NUM_OF_GP_OUT			8				// 汎用出力変数用バッファ長
#define NUM_OF_GP_IN			7				// 汎用入力変数用バッファ長
#define DEFAULT_REPLAY_RATE		50				// 再生速度の初期値
#define SAVEDIR_NAME_BUF		256				// 記録フォルダ名を格納するバッファ長

// 付加情報に関する定数
#define NUM_OF_ATTACHED_DATA	20				// LAN経由で転送される付加データ数
#define NUM_OF_APPEND_DATA		24				// LAN経由転送データとImageProcでの計算データの和
#define APPEND_DATA_DIGIT		12				// 付加データの最大桁数

// 初期化ファイル読み込み時の定数
#define INIT_FILE_BUF_LENGTH	512				// 初期化ファイル読み込み用バッファ長
#define INIT_FILE_LINE_LENGTH	256				// 初期化ファイル１行読み込み用バッファ長

#endif