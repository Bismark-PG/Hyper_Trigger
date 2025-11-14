/*==============================================================================

   システムタイマー [system_timer.h]
                                                         Author : Youhei Sato
                                                         Date   : 2018/06/17
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SYSTEM_TIMER_H_
#define SYSTEM_TIMER_H_


/*------------------------------------------------------------------------------
   関数のプロトタイプ宣言
------------------------------------------------------------------------------*/

// システムタイマーの初期化
void SystemTimer_Initialize(void);

// システムタイマーのリセット
void SystemTimer_Reset(void);

// システムタイマーのスタート
void SystemTimer_Start(void);

// システムタイマーのストップ
void SystemTimer_Stop(void);

// システムタイマーを0.1秒進める
void SystemTimer_Advance(void);

// 計測時間の取得
double SystemTimer_GetTime(void);

// 現在の時間を取得
double SystemTimer_GetAbsoluteTime(void);

// 経過時間の取得
double SystemTimer_GetElapsedTime(void);

// システムタイマーが止まっているか？
bool SystemTimer_IsStoped(void);

// 現在のスレッドを1つのプロセッサ（現在のスレッド）に制限
void LimitThreadAffinityToCurrentProc(void);

#endif // SYSTEM_TIMER_H_
