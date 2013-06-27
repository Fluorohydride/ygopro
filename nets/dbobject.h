#ifndef _DBOBJECT_H
#define _DBOBJECT_H

#include "dbadapter.h"

namespace ygopro
{

	class DBObject {
	public:
		template<typename... TR>
		inline void DBSetField(TR&... rest) {
			MongoExecutor(dbAdapter).condition("_id", uid).operation("$set", rest...).updateTo(GetCollection());
		}

		template<typename... TR>
		inline void DBIncField(TR&... rest) {
			MongoExecutor(dbAdapter).condition("_id", uid).operation("$inc", rest...).updateTo(GetCollection());
		}

		template<typename... TR>
		inline void DBUnsetField(TR&... rest) {
			MongoExecutor(dbAdapter).condition("_id", uid).operation("$unset", rest...).updateTo(GetCollection());
		}

		template<typename... TR>
		inline void DBNewObject(TR&... rest) {
			MongoExecutor(dbAdapter).values("_id", uid, rest...).insertTo(GetCollection());
		}

		inline void DBDeleteObject() {
			MongoExecutor(dbAdapter).condition("_id", uid).deleteFrom(GetCollection());
		}

		virtual inline const char* GetCollection() = 0;

	public:
		int uid;

	};

}

#endif
