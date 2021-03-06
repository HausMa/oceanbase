/*
 * (C) 2007-2010 Taobao Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * ob_mod_define.h is for what ...
 *
 * Version: $id: ob_mod_define.h,v 0.1 11/23/2010 5:57p wushi Exp $
 *
 * Authors:
 *   wushi <wushi.ly@taobao.com>
 *     - some work details if you want
 *
 */
#include "ob_memory_pool.h"
#ifndef OCEANBASE_COMMON_OB_MOD_DEFINE_H_
#define OCEANBASE_COMMON_OB_MOD_DEFINE_H_
namespace oceanbase
{
  namespace common
  {
    struct ObModInfo
    {
      ObModInfo()
      {
        mod_id_  = 0;
        mod_name_ = NULL;
      }
      int32_t mod_id_;
      const char * mod_name_;
    };


    static const int32_t G_MAX_MOD_NUM = 2048;
    extern ObModInfo OB_MOD_SET[G_MAX_MOD_NUM];


#define DEFINE_MOD(mod_name) mod_name

#define ADD_MOD(mod_name)\
  do \
  {\
    if (ObModIds::mod_name <= ObModIds::OB_MOD_END\
    && ObModIds::mod_name >= ObModIds::OB_MOD_DEFAULT \
    && ObModIds::mod_name < G_MAX_MOD_NUM) \
    {\
      OB_MOD_SET[ObModIds::mod_name].mod_id_ = ObModIds::mod_name; \
      OB_MOD_SET[ObModIds::mod_name].mod_name_ = # mod_name;\
    }\
  }while (0)

    /// 使用方法：
    /// 1. 在ObModIds中定义自己的模块名称, e.x. OB_MS_CELL_ARRAY
    /// 2. 在init_ob_mod_set中添加之前定义的模块名称，e.x. ADD_MOD(OB_MS_CELL_ARRAY)
    /// 3. 在调用ob_malloc的时候，使用定义的模块名称作为第二个参数，e.x. ob_malloc(512, ObModIds::OB_MS_CELL_ARRAY)
    /// 4. 发现内存泄露，调用ob_print_memory_usage()打印每个模块的内存使用量，以发现内存泄露模块
    /// 5. 也可以通过调用ob_get_mod_memory_usage(ObModIds::OB_MS_CELL_ARRAY)获取单个模块的内存使用量
    class ObModIds
    {
    public:
      enum
      {
        OB_MOD_DEFAULT = 0,
        /// define modules here
        // common modules
        OB_COMMON_NETWORK,
        OB_THREAD_BUFFER,
        OB_VARCACHE,
        OB_KVSTORE_CACHE,
        OB_TSI_FACTORY,
        OB_THREAD_OBJPOOL,
        OB_ROW_COMPACTION,
        OB_COMMON_COMPACTOR,
        OB_SQL_RPC_SCAN,
        OB_SQL_RPC_GET,
        OB_COMMON_ARRAY,
        OB_SE_ARRAY,
        OB_LIGHTY_QUEUE,
        OB_SEQ_QUEUE,

        OB_FILE_DIRECTOR_UTIL,
        OB_FETCH_RUNABLE,
        OB_GET_PARAM,
        OB_SCAN_PARAM,
        OB_MUTATOR,
        OB_SCANNER,
        OB_NEW_SCANNER,
        OB_BUFFER,
        OB_THREAD_STORE,
        OB_LOG_WRITER,
        OB_SINGLE_LOG_READER,
        OB_REGEX,
        OB_SLAB,
        OB_SLAVE_MGR,
        OB_THREAD_MEM_POOL,
        OB_HASH_BUCKET,
        OB_HASH_NODE,
        OB_PAGE_ARENA,
        OB_NB_ACCESSOR,
        OB_STRING_BUF,
        OB_SCHEMA,
        OB_ROW_ITER_ADAPTOR,
        OB_WAITABLE_POOL,
        OB_ASYNC_FILE_APPENDER,
        OB_FIRST_TABLET_META,
        OB_FILE_CLIENT,
        OB_OLD_GROUPBY,
        OB_MEM_BUFFER,
        OB_USER_INFO_KEY,
        OB_PERM_INFO,
        OB_SKEY_INFO_KEY,
        OB_FIXED_QUEUE,
        OB_PACKET_QUEUE,
        OB_POOL,
        OB_BLOOM_FILTER,
        COMPACT_ROW,
        SPOP_QUEUE,
        BIT_LOCK,
        ID_MAP,
        LIB_OBSQL,
        BLOCK_ALLOC,
        VECTOR,
        TEST,

       // mergeserver modules
        OB_MS_CELL_ARRAY,
        OB_MS_LOCATION_CACHE,
        OB_MS_BTREE,
        OB_MS_RPC,
        OB_MS_JOIN_CACHE,
        OB_MS_REQUEST_EVENT,
        OB_MS_RPC_EVENT,
        OB_MS_GET_EVENT,
        OB_MS_SERVICE_FUNC,
        OB_MS_COMMON,
        OB_MS_SUB_SCAN_REQUEST,

        // updateserver modules
        OB_UPS_ENGINE,
        OB_UPS_MEMTABLE,
        OB_UPS_LOG,
        OB_UPS_SCHEMA,
        OB_UPS_RESOURCE_POOL_NODE,
        OB_UPS_RESOURCE_POOL_ARRAY,
        OB_UPS_PARAM_DATA,
        OB_UPS_SESSION_CTX,
        OB_UPS_LOG_REPLAY_WORKER,
        OB_UPS_TRANS_EXECUTOR_TASK,
        OB_UPS_RESULT,
        OB_UPS_PHYPLAN_ALLOCATOR,
        OB_UPS_DATA_BLOCK,
        OB_UPS_SSTABLE_MGR,
        OB_UPS_COMMON,

        // chunkserver modules
        OB_CS_SSTABLE_READER,
        OB_CS_TABLET_IMAGE,
        OB_CS_IMPORT_SSTABLE,
        OB_CS_SERVICE_FUNC,
        OB_CS_MERGER,
        OB_CS_COMMON,
        OB_CS_FILE_RECYCLE,

        // sstable modules
        OB_SSTABLE_AIO,
        OB_SSTABLE_GET_SCAN,
        OB_SSTABLE_WRITER,
        OB_SSTABLE_READER,
        OB_SSTABLE_INDEX,
        OB_SSTABLE_SCHEMA,

        // obmysql modules
        OB_MYSQL_PACKET,
        OB_COMPACTSSTABLE_WRITER,

        // rootserver modules
        OB_RS_ROOT_TABLE,
        OB_RS_TABLET_MANAGER,
        OB_RS_SERVER_MANAGER,
        OB_RS_SCHEMA_MANAGER,
        OB_RS_LOG_WORKER,

        // sql modules
        OB_SQL_PARSER,
        OB_SQL_TRANSFORMER,
        OB_SQL_SESSION_POOL,
        OB_SQL_SESSION,
        OB_SQL_ROW_STORE,
        OB_SQL_EXECUTER,
        OB_SQL_SCAN_PARAM,
        OB_SQL_GET_PARAM,
        OB_SQL_RPC_REQUEST,
        OB_SQL_PS_TRANS,
        OB_SQL_ARRAY,
        OB_SQL_EXPR,
        OB_SQL_AGGR_FUNC,
        OB_SQL_INSERT,
        OB_SQL_MERGE_JOIN,
        OB_SQL_RESULT_SET,
        OB_SQL_TABLET_CACHE_JOIN,
        OB_SQL_UPDATE,
        OB_SQL_READ_STRATEGY,
        OB_SQL_PRIVILEGE,
        OB_SQL_COMMON,
        OB_SQL_SCALAR_AGGR,
        OB_SQL_MERGE_GROUPBY,
        OB_SQL_PHY_PLAN,
        OB_SQL_RESULT_SET_DYN,
        OB_SQL_SESSION_HASHMAP,
        OB_SQL_SESSION_SBLOCK,

        OB_MOD_END
      };
    };

    inline void init_ob_mod_set()
    {
      ADD_MOD(OB_MOD_DEFAULT);
      /// add modules here, modules must be first defined
      ADD_MOD(OB_COMMON_NETWORK);
      ADD_MOD(OB_THREAD_BUFFER);
      ADD_MOD(OB_VARCACHE);
      ADD_MOD(OB_KVSTORE_CACHE);
      ADD_MOD(OB_TSI_FACTORY);
      ADD_MOD(OB_THREAD_OBJPOOL);
      ADD_MOD(OB_ROW_COMPACTION);
      ADD_MOD(OB_COMMON_COMPACTOR);
      ADD_MOD(OB_SQL_RPC_SCAN);
      ADD_MOD(OB_SQL_RPC_GET);
      ADD_MOD(OB_COMMON_ARRAY);
      ADD_MOD(OB_SE_ARRAY);
      ADD_MOD(OB_LIGHTY_QUEUE);
      ADD_MOD(OB_SEQ_QUEUE);
      ADD_MOD(OB_FILE_DIRECTOR_UTIL);
      ADD_MOD(OB_FETCH_RUNABLE);
      ADD_MOD(OB_GET_PARAM);
      ADD_MOD(OB_SCAN_PARAM);
      ADD_MOD(OB_MUTATOR);
      ADD_MOD(OB_SCANNER);
      ADD_MOD(OB_NEW_SCANNER);
      ADD_MOD(OB_BUFFER);
      ADD_MOD(OB_THREAD_STORE);
      ADD_MOD(OB_LOG_WRITER);
      ADD_MOD(OB_SINGLE_LOG_READER);
      ADD_MOD(OB_REGEX);
      ADD_MOD(OB_SLAB);
      ADD_MOD(OB_SLAVE_MGR);
      ADD_MOD(OB_THREAD_MEM_POOL);
      ADD_MOD(OB_HASH_BUCKET);
      ADD_MOD(OB_HASH_NODE);
      ADD_MOD(OB_PAGE_ARENA);
      ADD_MOD(OB_NB_ACCESSOR);
      ADD_MOD(OB_STRING_BUF);
      ADD_MOD(OB_SCHEMA);
      ADD_MOD(OB_ROW_ITER_ADAPTOR);
      ADD_MOD(OB_WAITABLE_POOL);
      ADD_MOD(OB_ASYNC_FILE_APPENDER);
      ADD_MOD(OB_FIRST_TABLET_META);
      ADD_MOD(OB_FILE_CLIENT);
      ADD_MOD(OB_OLD_GROUPBY);
      ADD_MOD(OB_MEM_BUFFER);
      ADD_MOD(OB_USER_INFO_KEY);
      ADD_MOD(OB_PERM_INFO);
      ADD_MOD(OB_SKEY_INFO_KEY);
      ADD_MOD(OB_FIXED_QUEUE);
      ADD_MOD(OB_PACKET_QUEUE);
      ADD_MOD(OB_POOL);
      ADD_MOD(OB_BLOOM_FILTER);
      ADD_MOD(COMPACT_ROW);
      ADD_MOD(SPOP_QUEUE);
      ADD_MOD(BIT_LOCK);
      ADD_MOD(ID_MAP);
      ADD_MOD(LIB_OBSQL);
      ADD_MOD(BLOCK_ALLOC);
      ADD_MOD(VECTOR);
      ADD_MOD(TEST);

      ADD_MOD(OB_MS_CELL_ARRAY);
      ADD_MOD(OB_MS_LOCATION_CACHE);
      ADD_MOD(OB_MS_BTREE);
      ADD_MOD(OB_MS_RPC);
      ADD_MOD(OB_MS_JOIN_CACHE);
      ADD_MOD(OB_MS_REQUEST_EVENT);
      ADD_MOD(OB_MS_RPC_EVENT);
      ADD_MOD(OB_MS_GET_EVENT);
      ADD_MOD(OB_MS_SERVICE_FUNC);
      ADD_MOD(OB_MS_COMMON);
      ADD_MOD(OB_MS_SUB_SCAN_REQUEST);

      ADD_MOD(OB_UPS_ENGINE);
      ADD_MOD(OB_UPS_MEMTABLE);
      ADD_MOD(OB_UPS_LOG);
      ADD_MOD(OB_UPS_SCHEMA);
      ADD_MOD(OB_UPS_RESOURCE_POOL_NODE);
      ADD_MOD(OB_UPS_RESOURCE_POOL_ARRAY);
      ADD_MOD(OB_UPS_PARAM_DATA);
      ADD_MOD(OB_UPS_SESSION_CTX);
      ADD_MOD(OB_UPS_LOG_REPLAY_WORKER);
      ADD_MOD(OB_UPS_TRANS_EXECUTOR_TASK);
      ADD_MOD(OB_UPS_RESULT);
      ADD_MOD(OB_UPS_PHYPLAN_ALLOCATOR);
      ADD_MOD(OB_UPS_DATA_BLOCK);
      ADD_MOD(OB_UPS_SSTABLE_MGR);
      ADD_MOD(OB_UPS_COMMON);

      ADD_MOD(OB_CS_SSTABLE_READER);
      ADD_MOD(OB_CS_TABLET_IMAGE);
      ADD_MOD(OB_CS_IMPORT_SSTABLE);
      ADD_MOD(OB_CS_SERVICE_FUNC);
      ADD_MOD(OB_CS_MERGER);
      ADD_MOD(OB_CS_COMMON);
      ADD_MOD(OB_CS_FILE_RECYCLE);

      ADD_MOD(OB_SSTABLE_AIO);
      ADD_MOD(OB_SSTABLE_GET_SCAN);
      ADD_MOD(OB_SSTABLE_WRITER);
      ADD_MOD(OB_SSTABLE_READER);
      ADD_MOD(OB_SSTABLE_INDEX);
      ADD_MOD(OB_SSTABLE_SCHEMA);

      ADD_MOD(OB_MYSQL_PACKET);
      ADD_MOD(OB_COMPACTSSTABLE_WRITER);

      ADD_MOD(OB_RS_ROOT_TABLE);
      ADD_MOD(OB_RS_TABLET_MANAGER);
      ADD_MOD(OB_RS_SERVER_MANAGER);
      ADD_MOD(OB_RS_SCHEMA_MANAGER);
      ADD_MOD(OB_RS_LOG_WORKER);

      ADD_MOD(OB_SQL_PARSER);
      ADD_MOD(OB_SQL_TRANSFORMER);
      ADD_MOD(OB_SQL_SESSION_POOL);
      ADD_MOD(OB_SQL_SESSION);
      ADD_MOD(OB_SQL_ROW_STORE);
      ADD_MOD(OB_SQL_EXECUTER);
      ADD_MOD(OB_SQL_SCAN_PARAM);
      ADD_MOD(OB_SQL_GET_PARAM);
      ADD_MOD(OB_SQL_RPC_REQUEST);
      ADD_MOD(OB_SQL_PS_TRANS);
      ADD_MOD(OB_SQL_ARRAY);
      ADD_MOD(OB_SQL_EXPR);
      ADD_MOD(OB_SQL_AGGR_FUNC);
      ADD_MOD(OB_SQL_INSERT);
      ADD_MOD(OB_SQL_MERGE_JOIN);
      ADD_MOD(OB_SQL_RESULT_SET);
      ADD_MOD(OB_SQL_TABLET_CACHE_JOIN);
      ADD_MOD(OB_SQL_UPDATE);
      ADD_MOD(OB_SQL_READ_STRATEGY);
      ADD_MOD(OB_SQL_PRIVILEGE);
      ADD_MOD(OB_SQL_COMMON);
      ADD_MOD(OB_SQL_SCALAR_AGGR);
      ADD_MOD(OB_SQL_MERGE_GROUPBY);
      ADD_MOD(OB_SQL_PHY_PLAN);
      ADD_MOD(OB_SQL_RESULT_SET_DYN);
      ADD_MOD(OB_SQL_SESSION_HASHMAP);
      ADD_MOD(OB_SQL_SESSION_SBLOCK);

      ADD_MOD(OB_MOD_END);
    }
    class ObModSet : public oceanbase::common::ObMemPoolModSet
    {
    public:
      ObModSet();
      virtual ~ObModSet()
      {
      }
      virtual int32_t get_mod_id(const char * mod_name)const;
      virtual const char * get_mod_name(const int32_t mod_id) const;
      virtual int32_t get_max_mod_num()const;
    private:
      int32_t mod_num_;
    };
  }
}

#endif /* OCEANBASE_COMMON_OB_MOD_DEFINE_H_ */
