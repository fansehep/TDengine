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

#define _DEFAULT_SOURCE
#include "dmInt.h"


const char *dmStatStr(EDndRunStatus stype) {
  switch (stype) {
    case DND_STAT_INIT:
      return "init";
    case DND_STAT_RUNNING:
      return "running";
    case DND_STAT_STOPPED:
      return "stopped";
    default:
      return "UNKNOWN";
  }
}

const char *dmNodeLogName(EDndNodeType ntype) {
  switch (ntype) {
    case VNODE:
      return "vnode";
    case QNODE:
      return "qnode";
    case SNODE:
      return "snode";
    case MNODE:
      return "mnode";
    case BNODE:
      return "bnode";
    default:
      return "taosd";
  }
}

const char *dmNodeProcName(EDndNodeType ntype) {
  switch (ntype) {
    case VNODE:
      return "taosv";
    case QNODE:
      return "taosq";
    case SNODE:
      return "taoss";
    case MNODE:
      return "taosm";
    case BNODE:
      return "taosb";
    default:
      return "taosd";
  }
}

const char *dmEventStr(EDndEvent ev) {
  switch (ev) {
    case DND_EVENT_START:
      return "start";
    case DND_EVENT_STOP:
      return "stop";
    case DND_EVENT_CHILD:
      return "child";
    default:
      return "UNKNOWN";
  }
}

const char *dmProcStr(EDndProcType etype) {
  switch (etype) {
    case DND_PROC_SINGLE:
      return "start";
    case DND_PROC_CHILD:
      return "stop";
    case DND_PROC_PARENT:
      return "child";
    case DND_PROC_TEST:
      return "test";
    default:
      return "UNKNOWN";
  }
}

void *dmSetMgmtHandle(SArray *pArray, tmsg_t msgType, NodeMsgFp nodeMsgFp, bool needCheckVgId) {
  SMgmtHandle handle = {
      .msgType = msgType,
      .msgFp = nodeMsgFp,
      .needCheckVgId = needCheckVgId,
  };

  return taosArrayPush(pArray, &handle);
}

void dmGetSystemInfo(SMonSysInfo *pInfo) {
  taosGetCpuUsage(&pInfo->cpu_engine, &pInfo->cpu_system);
  taosGetCpuCores(&pInfo->cpu_cores);
  taosGetProcMemory(&pInfo->mem_engine);
  taosGetSysMemory(&pInfo->mem_system);
  pInfo->mem_total = tsTotalMemoryKB;
  pInfo->disk_engine = 0;
  pInfo->disk_used = tsDataSpace.size.used;
  pInfo->disk_total = tsDataSpace.size.total;
  taosGetCardInfoDelta(&pInfo->net_in, &pInfo->net_out);
  taosGetProcIODelta(&pInfo->io_read, &pInfo->io_write, &pInfo->io_read_disk, &pInfo->io_write_disk);
}