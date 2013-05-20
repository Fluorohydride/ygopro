#ifndef _DBOBJECT_H
#define _DBOBJECT_H

#include "dbadapter.h"

namespace ygopro
{

	class DBObject {
	public:
		template<typename T1>
		void SetField(const char* name1, T1 val1) {
			MongoExecutor(dbAdapter)("_id", uid).Op()("$set", name1, val1) >> GetCollection();
		}
		template<typename T1, typename T2>
		void SetField(const char* name1, T1 val1, const char* name2, T2 val2) {
			MongoExecutor(dbAdapter)("_id", uid).Op()("$set")(name1, val1)(name2, val2)() >> GetCollection();
		}
		template<typename T1, typename T2, typename T3>
		void SetField(const char* name1, T1 val1, const char* name2, T2 val2, const char* name3, T3 val3) {
			MongoExecutor(dbAdapter)("_id", uid).Op()("$set")(name1, val1)(name2, val2)(name3, val3)() >> GetCollection();
		}
		template<typename T1, typename T2, typename T3, typename T4>
		void SetField(const char* name1, T1 val1, const char* name2, T2 val2, const char* name3, T3 val3, const char* name4, T4 val4) {
			MongoExecutor(dbAdapter)("_id", uid).Op()("$set")(name1, val1)(name2, val2)(name3, val3)(name4, val4)() >> GetCollection();
		}
		template<typename T1, typename T2, typename T3, typename T4, typename T5>
		void SetField(const char* name1, T1 val1, const char* name2, T2 val2, const char* name3, T3 val3, const char* name4, T4 val4, const char* name5, T5 val5) {
			MongoExecutor(dbAdapter)("_id", uid).Op()("$set")(name1, val1)(name2, val2)(name3, val3)(name4, val4)(name5, val5)() >> GetCollection();
		}

		template<typename T1>
		void IncField(const char* name1, T1 val1) {
			MongoExecutor(dbAdapter)("_id", uid).Op()("$inc", name1, val1) >> GetCollection();
		}
		template<typename T1, typename T2>
		void IncField(const char* name1, T1 val1, const char* name2, T2 val2) {
			MongoExecutor(dbAdapter)("_id", uid).Op()("$inc")(name1, val1)(name2, val2)() >> GetCollection();
		}
		template<typename T1, typename T2, typename T3>
		void IncField(const char* name1, T1 val1, const char* name2, T2 val2, const char* name3, T3 val3) {
			MongoExecutor(dbAdapter)("_id", uid).Op()("$inc")(name1, val1)(name2, val2)(name3, val3)() >> GetCollection();
		}
		template<typename T1, typename T2, typename T3, typename T4>
		void IncField(const char* name1, T1 val1, const char* name2, T2 val2, const char* name3, T3 val3, const char* name4, T4 val4) {
			MongoExecutor(dbAdapter)("_id", uid).Op()("$inc")(name1, val1)(name2, val2)(name3, val3)(name4, val4)() >> GetCollection();
		}
		template<typename T1, typename T2, typename T3, typename T4, typename T5>
		void IncField(const char* name1, T1 val1, const char* name2, T2 val2, const char* name3, T3 val3, const char* name4, T4 val4, const char* name5, T5 val5) {
			MongoExecutor(dbAdapter)("_id", uid).Op()("$inc")(name1, val1)(name2, val2)(name3, val3)(name4, val4)(name5, val5)() >> GetCollection();
		}

		virtual const char* GetCollection() = 0;

	public:
		int uid;

	};

}

#endif
