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

static int8_t once = DND_ENV_INIT;

int32_t dmInit() {
  dInfo("start to init env");
  if (atomic_val_compare_exchange_8(&once, DND_ENV_INIT, DND_ENV_READY) != DND_ENV_INIT) {
    dError("env is already initialized");
    terrno = TSDB_CODE_REPEAT_INIT;
    return -1;
  }

  taosIgnSIGPIPE();
  taosBlockSIGPIPE();
  taosResolveCRC();

  dInfo("env is initialized");
  return 0;
}

void dmCleanup() {
  dDebug("start to cleanup env");
  if (atomic_val_compare_exchange_8(&once, DND_ENV_READY, DND_ENV_CLEANUP) != DND_ENV_READY) {
    dError("env is already cleaned up");
    return;
  }

  monCleanup();
  syncCleanUp();
  walCleanUp();
  udfcClose();
  taosStopCacheRefreshWorker();
  dInfo("env is cleaned up");
}