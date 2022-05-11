/*
 * Copyright (c) 2019 TAOS Data, Inc. <jhtao@taosdata.com>
 *
 * This program is free software: you can use, redistribute, and/or modify
 * it under the terms of the GNU Affero General Public License, version 3
 * or later ("AGPL"), as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TD_DM_INT_H_
#define _TD_DM_INT_H_

#include "cJSON.h"
#include "tcache.h"
#include "tcrc32c.h"
#include "tdatablock.h"
#include "tglobal.h"
#include "thash.h"
#include "tlockfree.h"
#include "tlog.h"
#include "tmsg.h"
#include "tmsgcb.h"
#include "tprocess.h"
#include "tqueue.h"
#include "trpc.h"
#include "tthread.h"
#include "ttime.h"
#include "tworker.h"

#include "dnode.h"
#include "mnode.h"
#include "monitor.h"
#include "sync.h"
#include "wal.h"

#include "libs/function/function.h"

#ifdef __cplusplus
extern "C" {
#endif

#define dFatal(...) { if (dDebugFlag & DEBUG_FATAL) { taosPrintLog("DND FATAL ", DEBUG_FATAL, 255, __VA_ARGS__); }}
#define dError(...) { if (dDebugFlag & DEBUG_ERROR) { taosPrintLog("DND ERROR ", DEBUG_ERROR, 255, __VA_ARGS__); }}
#define dWarn(...)  { if (dDebugFlag & DEBUG_WARN)  { taosPrintLog("DND WARN ", DEBUG_WARN, 255, __VA_ARGS__); }}
#define dInfo(...)  { if (dDebugFlag & DEBUG_INFO)  { taosPrintLog("DND ", DEBUG_INFO, 255, __VA_ARGS__); }}
#define dDebug(...) { if (dDebugFlag & DEBUG_DEBUG) { taosPrintLog("DND ", DEBUG_DEBUG, dDebugFlag, __VA_ARGS__); }}
#define dTrace(...) { if (dDebugFlag & DEBUG_TRACE) { taosPrintLog("DND ", DEBUG_TRACE, dDebugFlag, __VA_ARGS__); }}

typedef enum {
  DNODE = 0,
  VNODE = 1,
  QNODE = 2,
  SNODE = 3,
  MNODE = 4,
  BNODE = 5,
  NODE_END = 6,
} EDndNodeType;

typedef enum {
  DND_STAT_INIT,
  DND_STAT_RUNNING,
  DND_STAT_STOPPED,
} EDndRunStatus;

typedef enum {
  DND_ENV_INIT,
  DND_ENV_READY,
  DND_ENV_CLEANUP,
} EDndEnvStatus;

typedef enum {
  DND_PROC_SINGLE,
  DND_PROC_CHILD,
  DND_PROC_PARENT,
  DND_PROC_TEST,
} EDndProcType;

typedef struct {
  const char *path;
  const char *name;
  SMsgCb      msgCb;
  int32_t     dnodeId;
  int64_t     clusterId;
  const char *dataDir;
  const char *localEp;
  const char *firstEp;
  const char *localFqdn;
  uint16_t    serverPort;
  int32_t     supportVnodes;
} SMgmtInputOpt;

typedef struct {
  int32_t dnodeId;
  void   *pMgmt;
} SMgmtOutputOpt;

typedef int32_t (*NodeMsgFp)(void *pMgmt, SNodeMsg *pMsg);
typedef int32_t (*NodeOpenFp)(const SMgmtInputOpt *pInput, SMgmtOutputOpt *pOutput);
typedef void (*NodeCloseFp)(void *pMgmt);
typedef int32_t (*NodeStartFp)(void *pMgmt);
typedef void (*NodeStopFp)(void *pMgmt);
typedef int32_t (*NodeCreateFp)(void *pMgmt, SNodeMsg *pMsg);
typedef int32_t (*NodeDropFp)(void *pMgmt, SNodeMsg *pMsg);
typedef int32_t (*NodeRequireFp)(const SMgmtInputOpt *pInput, bool *required);
typedef SArray *(*NodeGetHandlesFp)();  // array of SMgmtHandle

typedef struct {
  NodeOpenFp       openFp;
  NodeCloseFp      closeFp;
  NodeStartFp      startFp;
  NodeStopFp       stopFp;
  NodeCreateFp     createFp;
  NodeDropFp       dropFp;
  NodeRequireFp    requiredFp;
  NodeGetHandlesFp getHandlesFp;
} SMgmtFunc;

typedef struct {
  tmsg_t    msgType;
  bool      needCheckVgId;
  NodeMsgFp msgFp;
} SMgmtHandle;

// dmUtil.c
const char *dmStatStr(EDndRunStatus stype);
const char *dmNodeLogName(EDndNodeType ntype);
const char *dmNodeProcName(EDndNodeType ntype);
const char *dmEventStr(EDndEvent etype);
const char *dmProcStr(EDndProcType ptype);
void       *dmSetMgmtHandle(SArray *pArray, tmsg_t msgType, NodeMsgFp nodeMsgFp, bool needCheckVgId);
void        dmGetSystemInfo(SMonSysInfo *pInfo);

// dmFile.c
int32_t   dmReadFile(const char *path, const char *name, bool *pDeployed);
int32_t   dmWriteFile(const char *path, const char *name, bool deployed);
TdFilePtr dmCheckRunning(const char *dataDir);
int32_t   dmReadShmFile(const char *path, const char *name, SShm *pShm);
int32_t   dmWriteShmFile(const char *path, const char *name, const SShm *pShm);

#ifdef __cplusplus
}
#endif

#endif /*_TD_DM_INT_H_*/