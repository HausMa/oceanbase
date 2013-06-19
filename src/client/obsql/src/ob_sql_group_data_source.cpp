#include "ob_sql_group_data_source.h"
#include "ob_sql_list.h"
#include "ob_sql_global.h"
#include "ob_sql_util.h"
#include "ob_sql_data_source.h"
#include "ob_sql_data_source_utility.h"
#include "ob_sql_conn_acquire.h"
#include "ob_sql_list.h"
#include "stddef.h"
#include "tblog.h"
#include "common/ob_malloc.h"

using namespace oceanbase::common;
/* 从集群池pool中移除一个集群spool */
static int delete_server_pool(ObClusterInfo *spool, ObGroupDataSource *gds)
{
  int ret = OB_SQL_SUCCESS;
  int32_t index = 0;
  int32_t start = static_cast<int32_t>(spool - gds->clusters_);
  start++;
  int32_t cidx = 0;
  //connectin all datasource to delete ms list
  for (; index < spool->csize_; ++index)
  {
    ObDataSource *ds = spool->dslist_ + index;
    ObSQLConnList *list = reinterpret_cast<ObSQLConnList*>(ob_malloc(sizeof(ObSQLConnList), ObModIds::LIB_OBSQL));
    if (NULL == list)
    {
      TBSYS_LOG(ERROR, "alloc mem for ObSQLConnList failed");
      ret = OB_SQL_ERROR;
    }
    else
    {
      //move connection
      move_conn_list(list, ds);
      //memcpy(list, &ds->conn_list_, sizeof(ObSQLConnList));
      TBSYS_LOG(DEBUG, "ds free list is %d", get_list_size(&ds->conn_list_.free_conn_list_));
      TBSYS_LOG(DEBUG, "ds used list is %d", get_list_size(&ds->conn_list_.used_conn_list_));
      TBSYS_LOG(DEBUG, "new ds free list is %d", get_list_size(&list->free_conn_list_));
      TBSYS_LOG(DEBUG, "new ds used list is %d", get_list_size(&list->used_conn_list_));
      //memcpy(list, &ds->conn_list_, sizeof(ObSQLConnList));
      ob_sql_list_add_tail(&list->delete_list_node_, &g_delete_ms_list);
      dump_delete_ms_conn();
    }
  }

  if (OB_SQL_SUCCESS == ret)
  {
    for (; start < gds->csize_; ++start)
    {
      gds->clusters_[start - 1] = gds->clusters_[start];
      move_conn(gds->clusters_ + start - 1, gds->clusters_ + start);
      TBSYS_LOG(DEBUG, "%d free conn is %d", start, get_list_size(&gds->clusters_[start-1].dslist_[0].conn_list_.free_conn_list_));
      cidx = 0;
      for (; cidx < gds->clusters_[start -1].csize_; ++cidx)
      {
        gds->clusters_[start - 1].dslist_[cidx].cluster_ = gds->clusters_ + start - 1;
      }
    }
    gds->csize_--;
    gds->size_--;
  }
  return ret;
}

/* 初始化cluster info */
static int init_cluster_info(ObSQLClusterConfig *config, ObGroupDataSource *gds, int32_t idx)
{
  int ret = OB_SQL_SUCCESS;
  int32_t index = 0;
  if (idx <= gds->csize_)
  {
    index = gds->csize_;
    for (; index >= idx; index--)
    {
      gds->clusters_[index] = gds->clusters_[index - 1];
    }
  }
  ObClusterInfo * cluster = gds->clusters_ + gds->csize_;
  cluster->size_ = config->server_num_;
  cluster->csize_ = 0;
  cluster->rs_ = config->server_;
  cluster->cluster_id_ = config->cluster_id_;
  cluster->is_master_ = static_cast<int32_t>(config->cluster_type_);
  cluster->read_strategy_ = config->read_strategy_;
  pthread_rwlock_init(&(cluster->rwlock_), NULL);

  for (; index < config->server_num_ && OB_SQL_SUCCESS == ret; ++index)
  {
    ret = init_data_source(g_config_using->max_conn_size_, config->merge_server_[index], cluster);
    if (OB_SQL_SUCCESS != ret)
    {
      TBSYS_LOG(ERROR, "init ObDataSource %s failed", get_server_str(config->merge_server_ + index));
      for (; idx < gds->csize_; ++idx)
      {
        gds->clusters_[idx-1] = gds->clusters_[idx];
      }
      break;
    }
    TBSYS_LOG(INFO, "cluster ds size is %d", cluster->csize_);
  }
  gds->csize_++;
  return ret;
}

static int update_server_pool(ObSQLClusterConfig config, ObClusterInfo *cluster)
{
  int ret = OB_SQL_SUCCESS;
  int32_t index = 0;
  int32_t sindex = 0;
  int32_t exist = 0;
  ObDataSource *sds = NULL;
  ObServerInfo server;
  cluster->read_strategy_ = config.read_strategy_;
  for (; sindex < cluster->csize_; ++sindex)
  {
    exist = 0;
    sds = cluster->dslist_ + sindex;
    index = 0;
    for (; index < config.server_num_ && OB_SQL_SUCCESS == ret; ++index)
    {
      server = config.merge_server_[index];
      if (sds->server_.ip_ == server.ip_
        &&sds->server_.port_ == server.port_)
      {
        TBSYS_LOG(DEBUG, "sds ip=%u port=%u, config ip=%u port=%u", sds->server_.ip_, sds->server_.port_, server.ip_, server.port_);
        exist = 1;
        break;
      }
    }
    if (0 == exist)
    {
      delete_data_source(sds);
    }
  }

  index = 0;
  sindex = 0;
  for (; index < config.server_num_ && OB_SQL_SUCCESS == ret; ++index)
  {
    exist = 0;
    server = config.merge_server_[index];
    for(; sindex < cluster->csize_; ++sindex)
    {
      sds = cluster->dslist_ + sindex;
      if (sds->server_.ip_ == server.ip_
        &&sds->server_.port_ == server.port_)
      {
        exist = 1;
        break;
      }
    }
    if (0 == exist)
    {
      ret = init_data_source(g_config_using->max_conn_size_, server, cluster);
    }
  }
  return ret;
}

/* 根据config的配置来更新全局连接池 */
int update_group_ds(ObSQLGlobalConfig *config, ObGroupDataSource *gds)
{
  int ret = OB_SQL_SUCCESS;
  int32_t cindex = 0;
  int32_t index = 0;
  int32_t exist = 0;
  ObClusterInfo * scluster = NULL;
  for (; index < gds->csize_; ++index)
  {
    scluster = gds->clusters_ + index;
    cindex = 0;
    exist = 0;
    for(; cindex < config->cluster_size_; ++cindex)
    {
      if (scluster->cluster_id_ == config->clusters_[cindex].cluster_id_)
      {
        exist = 1;
        break;
      }
    }
    if (0 == exist)
    {
      ret = delete_server_pool(scluster, gds);
      if (OB_SQL_SUCCESS != ret)
      {
        TBSYS_LOG(ERROR, "delete server pool failed");
        break;
      }
      else
      {
        index--;
      }
    }
    else
    {
      ret = update_server_pool(config->clusters_[cindex], scluster);
      if (OB_SQL_SUCCESS != ret)
      {
        TBSYS_LOG(ERROR, "update server pool failed");
        break;
      }
    }
  }

  for(; cindex < config->cluster_size_ && OB_SQL_SUCCESS == ret; ++cindex)
  {
    exist = 0;
    index = 0;
    for (; index < gds->csize_; ++index)
    {
      ObClusterInfo *scluster = gds->clusters_ + index;
      if (scluster->cluster_id_ == config->clusters_[cindex].cluster_id_)
      {
        exist = 1;
        break;
      }
    }

    if (0 == exist)
    {
      TBSYS_LOG(INFO, "debug us update for init cindex is %d", cindex);
      //add pool
      ret = init_cluster_info(config->clusters_ + cindex, gds, cindex+1);
      if (OB_SQL_SUCCESS != ret)
      {
        TBSYS_LOG(ERROR, "init server pool failed");
        break;
      }
    }
  }
  return ret;
}

int init_group_ds(ObSQLGlobalConfig *config, ObGroupDataSource *gds)
{
  int ret = OB_SQL_SUCCESS;
  gds->min_conn_ = config->min_conn_size_;
  gds->max_conn_ = config->max_conn_size_;
  gds->size_ = config->cluster_size_;
  gds->csize_ = 0;
  if (0 == gds->size_)
  {
    TBSYS_LOG(ERROR, "cluster config error, there are %d clusters", gds->size_);
    ret = OB_SQL_ERROR;
  }
  else
  {
    pthread_rwlock_init(&(gds->rwlock_), NULL);
    int32_t index = 1;
    for (;index <= gds->size_ && OB_SQL_SUCCESS == ret; ++index)
    {
      //init cluster info
      TBSYS_LOG(DEBUG, "gds csize is %d", gds->csize_);
      ret = init_cluster_info(config->clusters_ + index - 1, gds, index);
    }
  }
  return ret;
}

ObClusterInfo* get_master_cluster(ObGroupDataSource *gds)
{
  ObClusterInfo *cluster = NULL;
  int32_t index = 0;
  for (; index < gds->csize_; ++index)
  {
    if (1 == gds->clusters_[index].is_master_)
    {
      cluster = gds->clusters_ + index;
      break;
    }
  }
  return cluster;
}
