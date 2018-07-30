#ifndef CONSTANTVALUES_H
#define CONSTANTVALUES_H

//#define VISION128
//#define VISION100
//#define VISION064
//#define QVGA
#define QQVGA

// 128x128�̎�
#ifdef VISION128
#define IMAGE_DATA_LENGTH		16384			// ��f��
#define USB_BUF_SIZE			16896			// �摜��ʂ���M���邽�߂ɗ��p����o�b�t�@�o�C�g��
#define NUM_OF_PACKET			33				// �P�t���[���̃p�P�b�g��
#define IMAGE_WIDTH				128				// �摜�̕�
#define IMAGE_HEIGHT			128				// �摜�̍���
#define ATTACHED_DATA_LENGTH	380				// �ŏI�p�P�b�g�̗]���o�C�g��
#endif

// 100x100�̎�
#ifdef VISION100
#define IMAGE_DATA_LENGTH		10000			// ��f��
#define USB_BUF_SIZE			10240			// �摜��ʂ���M���邽�߂ɗ��p����o�b�t�@�o�C�g��
#define NUM_OF_PACKET			20				// �P�t���[���̃p�P�b�g��
#define IMAGE_WIDTH				100				// �摜�̕�
#define IMAGE_HEIGHT			100				// �摜�̍���
#define ATTACHED_DATA_LENGTH	160				// �ŏI�p�P�b�g�̗]���o�C�g��
#endif

// 64x64�̎�
#ifdef VISION064
#define IMAGE_DATA_LENGTH		4096			// ��f��
#define USB_BUF_SIZE			4608			// �摜��ʂ���M���邽�߂ɗ��p����o�b�t�@�o�C�g��
#define NUM_OF_PACKET			9				// �P�t���[���̃p�P�b�g��
#define IMAGE_WIDTH				64				// �摜�̕�
#define IMAGE_HEIGHT			64				// �摜�̍���
#define ATTACHED_DATA_LENGTH	476				// �ŏI�p�P�b�g�̗]���o�C�g��
#endif

// QVGA�̎�
#ifdef QVGA
#define IMAGE_DATA_LENGTH		76800			// ��f��
#define USB_BUF_SIZE			77824			// �摜��ʂ���M���邽�߂ɗ��p����o�b�t�@�o�C�g��
#define NUM_OF_PACKET			152				// �P�t���[���̃p�P�b�g��
#define IMAGE_WIDTH				320				// �摜�̕�
#define IMAGE_HEIGHT			240				// �摜�̍���
#define ATTACHED_DATA_LENGTH	416				// �ŏI�p�P�b�g�̗]���o�C�g��
#endif

// QQVGA�̎�
#ifdef QQVGA
#define IMAGE_DATA_LENGTH		19200			// ��f��
#define USB_BUF_SIZE			19456			// �摜��ʂ���M���邽�߂ɗ��p����o�b�t�@�o�C�g��
#define NUM_OF_PACKET			38				// �P�t���[���̃p�P�b�g��
#define IMAGE_WIDTH				160				// �摜�̕�
#define IMAGE_HEIGHT			120				// �摜�̍���
#define ATTACHED_DATA_LENGTH	104				// �ŏI�p�P�b�g�̗]���o�C�g��
#endif

// �ʐM�֘A�̒萔
#define MAX_NUM_OF_RECV_IMG		8				// ��M�\�ȍő�C���[�W��				
#define USB_PACKET_LENGTH		512				// USB�p�P�b�g�̃o�C�g��
#define USB_DATA_LENGTH			508				// USB�p�P�b�g�̃f�[�^�o�C�g��
#define USB_HEADER_LENGTH		4				// USB�p�P�b�g�̃w�b�_�o�C�g��
#define NUM_OF_SEND_DATA		100				// ���M�p�P�b�g�̃f�[�^��
#define	SEND_DATA_LIMIT			255				// ���M�f�[�^�̍ő�l(1 - 255)
#define	NUM_OF_TEMP_BUF			8				// �C���[�W�������̈ꎞ�o�b�t�@��

// �\���A�����֘A�̒萔
#define DISPLAY_RATE			1				// �`�惌�[�g
#define IDB_BITMAP				IDB_BITMAP8		// 1:100, 2:200, 3:300, 4:128, 5:256, 6:384, 7:QQVGA, 8:QVGA
#define PIXEL_SIZE				2				// �P�s�N�Z���̑傫��(1����5�Ŏw��)
#define IMAGE_INTERVAL			5				// �摜�Ԃ̃s�N�Z����
#define NUM_OF_ROW_DISPLAY		4				// ���������̉�ʐ�
#define NUM_OF_COL_DISPLAY		3				// ���������̉�ʐ�
#define NUM_OF_DISPLAY			NUM_OF_ROW_DISPLAY * NUM_OF_COL_DISPLAY
#define NUM_OF_PROCESS_BUF		40				// ���p����computedImage�̐�
#define FRAMES_TO_PROC			8				// ImageProc�ŗ��p����t���[����(1���ƌ������̂�)
#define LOAD_BUF_LENGTH			1800			// �^��Đ����ɗ��p����o�b�t�@��
#define NUM_OF_GP_OUT			8				// �ėp�o�͕ϐ��p�o�b�t�@��
#define NUM_OF_GP_IN			7				// �ėp���͕ϐ��p�o�b�t�@��
#define DEFAULT_REPLAY_RATE		50				// �Đ����x�̏����l
#define SAVEDIR_NAME_BUF		256				// �L�^�t�H���_�����i�[����o�b�t�@��

// �t�����Ɋւ���萔
#define NUM_OF_ATTACHED_DATA	20				// LAN�o�R�œ]�������t���f�[�^��
#define NUM_OF_APPEND_DATA		24				// LAN�o�R�]���f�[�^��ImageProc�ł̌v�Z�f�[�^�̘a
#define APPEND_DATA_DIGIT		12				// �t���f�[�^�̍ő包��

// �������t�@�C���ǂݍ��ݎ��̒萔
#define INIT_FILE_BUF_LENGTH	512				// �������t�@�C���ǂݍ��ݗp�o�b�t�@��
#define INIT_FILE_LINE_LENGTH	256				// �������t�@�C���P�s�ǂݍ��ݗp�o�b�t�@��

#endif