#ifndef __FRAME__
#define __FRAME__

// define
// ���ݭn������T���ɭԴN��Ӯ���
#define TRACE_DEBUG

// �{���Ұʼƶq,�p�G�L�W���N���ndefine
#define STARTUP_LIMIT					1

// ���ЧR��
#define SAFE_DELETE(a)					if( (a) != NULL ) {delete (a);   (a) = NULL;}
#define SAFE_DELETE_ARRAY(a)			if( (a) != NULL ) {delete[] (a); (a) = NULL;}

// ���a���~��T��exception
#define FRAME_EXPCEPTION_1(param)		Frame::Exception(param, __FILE__, __LINE__);
#define FRAME_EXPCEPTION_2(no, msg)		Frame::Exception(no, msg, __FILE__, __LINE__);

// ������T
#ifdef TRACE_DEBUG
	// �i�X�禡���ɭԯd�U����
	#define TRACE_SHOW(x)				TracePrint _TracePrint_(__FUNCTION__,__FILE__,true,x)
	// �i�X�禡���ɭԤ��d�U����
	#define TRACE_SHOW_NOENTRYPOINT(x)	TracePrint _TracePrint_(__FUNCTION__,__FILE__,false,x)
	// ��ܤ��e
	#define TRACE_PRINT					_TracePrint_.SetLine(__LINE__); _TracePrint_.Print
	// ��ܸӦ��T
	#define TRACE_RECORD				_TracePrint_.Record(__LINE__)
#else
	// �H�U�Ҧ��������|������
	#define TRACE_SHOW(x)				//
	#define TRACE_SHOW_NOENTRYPOINT(x)	//
	#define TRACE_PRINT					//
	#define TRACE_RECORD				/* */
#endif


// STL
#include <assert.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <vector>
#include <stdlib.h>
#include <map>
#include <set>
#include <deque>
#include <sys/shm.h>
#include <signal.h>
#include <exception>
#include <queue>
#include <stdarg.h>
#include <string.h>
#include <memory>
#include <tr1/memory>
#include <list>
#include <libgen.h> 


// Other
#include "Frame/TracePrint.h"
#include "Frame/FrameException.h"
#include "Frame/SocketClass.h"
#include "Frame/TcpAcceptor.h"
#include "Frame/TcpSession.h"
#include "Frame/Thread.h"
#include "Frame/ThreadBind.h"
#include "Frame/ThreadPool.h"
#include "Frame/Condition.h"
#include "Frame/DateTime.h"
#include "Frame/DateTimeFormat.h"
#include "Frame/ShareMemory.h"
#include "Frame/Dictionary.h"
#include "Frame/Dictionary2.h"
#include "Frame/StringPlus.h"
#include "Frame/Application.h"
#include "Frame/Singleton.h"
#include "Frame/AutoBuffer.h"
#include "Frame/SignalMessage.h"
#include "Frame/LogHeader.h"
#include "Frame/LogWriter.h"
#include "Frame/FileInfo.h"
#include "Frame/Config.h"
#include "Frame/SimpleFormat.h"
#include "Frame/ColumnFormat.h"
#include "Frame/FileController.h"



// �����ܼ�(�ϥΥ����ܼ�����ɲ��ͪ����~�N�L�k�Q����)
Application		g_app;
SignalMessage	g_sigmsg;


#endif
