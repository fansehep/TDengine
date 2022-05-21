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

#include "executor.h"
#include "qndInt.h"
#include "query.h"
#include "qworker.h"
#include "libs/function/function.h"

SQnode *qndOpen(const SQnodeOpt *pOption) {
  SQnode *pQnode = taosMemoryCalloc(1, sizeof(SQnode));
  if (NULL == pQnode) {
    qError("calloc SQnode failed");
    return NULL;
  }

  if (qWorkerInit(NODE_TYPE_QNODE, pQnode->qndId, NULL, (void **)&pQnode->pQuery, &pOption->msgCb)) {
    taosMemoryFreeClear(pQnode);
    return NULL;
  }

  pQnode->msgCb = pOption->msgCb;
  return pQnode;
}

void qndClose(SQnode *pQnode) {
  qWorkerDestroy((void **)&pQnode->pQuery);
  taosMemoryFree(pQnode);
}

int32_t qndGetLoad(SQnode *pQnode, SQnodeLoad *pLoad) { return 0; }

int32_t qndProcessQueryMsg(SQnode *pQnode, SRpcMsg *pMsg) {
  int32_t     code = -1;
  SReadHandle handle = {.pMsgCb = &pQnode->msgCb};
  qTrace("message in qnode queue is processing");

  switch (pMsg->msgType) {
    case TDMT_VND_QUERY:
      code = qWorkerProcessQueryMsg(&handle, pQnode->pQuery, pMsg);
      break;
    case TDMT_VND_QUERY_CONTINUE:
      code = qWorkerProcessCQueryMsg(&handle, pQnode->pQuery, pMsg);
      break;
    case TDMT_VND_FETCH:
      code = qWorkerProcessFetchMsg(pQnode, pQnode->pQuery, pMsg);
      break;
    case TDMT_VND_FETCH_RSP:
      code = qWorkerProcessFetchRsp(pQnode, pQnode->pQuery, pMsg);
      break;
    case TDMT_VND_RES_READY:
      code = qWorkerProcessReadyMsg(pQnode, pQnode->pQuery, pMsg);
      break;
    case TDMT_VND_TASKS_STATUS:
      code = qWorkerProcessStatusMsg(pQnode, pQnode->pQuery, pMsg);
      break;
    case TDMT_VND_CANCEL_TASK:
      code = qWorkerProcessCancelMsg(pQnode, pQnode->pQuery, pMsg);
      break;
    case TDMT_VND_DROP_TASK:
      code = qWorkerProcessDropMsg(pQnode, pQnode->pQuery, pMsg);
      break;
    case TDMT_VND_TABLE_META:
      // code =  vnodeGetTableMeta(pQnode, pMsg);
      // break;
    case TDMT_VND_CONSUME:
      // code =  tqProcessConsumeReq(pQnode->pTq, pMsg);
      // break;
    case TDMT_VND_QUERY_HEARTBEAT:
      code = qWorkerProcessHbMsg(pQnode, pQnode->pQuery, pMsg);
      break;
    default:
      qError("unknown msg type:%d in qnode queue", pMsg->msgType);
      terrno = TSDB_CODE_VND_APP_ERROR;
  }

  if (code == 0) return TSDB_CODE_ACTION_IN_PROGRESS;
  return code;
}
