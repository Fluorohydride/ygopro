#ifndef DBOBJECT_H
#define DBOBJECT_H

#include "../common/common.h"
#include "../mongo/mongo.h"
#include <unordered_map>

namespace ygopro
{

	class _BsonUnit {
	protected:
		bson stbson;

	public:
		_BsonUnit() {
			bson_init(&stbson);
		}
		virtual ~_BsonUnit() {
			bson_destroy(&stbson);
		}
		void append_data(const char* name, int val) {
			bson_append_int(&stbson, name, val);
		}
		void append_data(const char* name, unsigned int val) {
			bson_append_int(&stbson, name, val);
		}
		void append_data(const char* name, long long val) {
			bson_append_long(&stbson, name, val);
		}
		void append_data(const char* name, unsigned long long val) {
			bson_append_long(&stbson, name, val);
		}
		void append_data(const char* name, double val) {
			bson_append_double(&stbson, name, val);
		}
		void append_data(const char* name, const char* val) {
			bson_append_string(&stbson, name, val);
		}
		void append_data(const char* name, const char* data, int size) {
			bson_append_binary(&stbson, name, BSON_BIN_BINARY, data, size);
		}
		void begin_object(const char* obj) {
			bson_append_start_object(&stbson, obj);
		}
		void finish_object() {
			bson_append_finish_object(&stbson);
		}
		void finish() {
			bson_finish(&stbson);
		}
		bson* get_bson() {
			return &stbson;
		}
	};

	class DBAdapter {

		friend class MongoExecutor;

	protected:
		mongo mongo_con[1];

	public:

		DBAdapter() {
			mongo_init(mongo_con);
		}

		~DBAdapter() {
			mongo_destroy(mongo_con);
		}

		bool Connect(const char* host, int port = 27017) {
			if(mongo_is_connected(mongo_con))
				return false;
			mongo_set_op_timeout(mongo_con, 1000);
			if(mongo_client(mongo_con, host, port) == MONGO_OK)
				return true;
			return false;
		}

		void Disconnect() {
			if(!mongo_is_connected(mongo_con))
				return;
			mongo_disconnect(mongo_con);
		}

	};

	class MongoQuery {

		friend class MongoExecutor;

	private:
		mongo_cursor mcursor[1];
		bson* data;

	public:
		MongoQuery() {
			data = nullptr;
		}
		MongoQuery& operator () (const char* name, int& val, int defval = 0) {
			bson_iterator iter[1];
			if(bson_find(iter, data, name) != BSON_EOO)
				val = bson_iterator_int(iter);
			else val = defval;
			return *this;
		}
		MongoQuery& operator () (const char* name, unsigned int& val, unsigned int defval = 0) {
			bson_iterator iter[1];
			if(bson_find(iter, data, name) != BSON_EOO)
				val = bson_iterator_int(iter);
			else val = defval;
			return *this;
		}
		MongoQuery& operator () (const char* name, long long& val, long long defval = 0) {
			bson_iterator iter[1];
			if(bson_find(iter, data, name) != BSON_EOO)
				val = bson_iterator_long(iter);
			else val = defval;
			return *this;
		}
		MongoQuery& operator () (const char* name, unsigned long long& val, unsigned long long defval = 0) {
			bson_iterator iter[1];
			if(bson_find(iter, data, name) != BSON_EOO)
				val = bson_iterator_long(iter);
			else val = defval;
			return *this;
		}
		MongoQuery& operator () (const char* name, double& val, double defval = 0.0) {
			bson_iterator iter[1];
			if(bson_find(iter, data, name) != BSON_EOO)
				val = bson_iterator_double(iter);
			else val = defval;
			return *this;
		}
		MongoQuery& operator () (const char* name, std::string& val, const char* defval = "") {
			bson_iterator iter[1];
			if(bson_find(iter, data, name) != BSON_EOO)
				val = bson_iterator_string(iter);
			else val = defval;
			return *this;
		}
		MongoQuery& operator () (const char* name, unsigned char*& ptr, int& size) {
			bson_iterator iter[1];
			if(bson_find(iter, data, name) != BSON_EOO) {
				size = bson_iterator_bin_len(iter);
				if(size == 0)
					ptr = nullptr;
				else {
					ptr = new unsigned char[size];
					memcpy(ptr, bson_iterator_bin_data(iter), size);
				}
			} else {
				ptr = nullptr;
				size = 0;
			}
			return *this;
		}
		bool operator ++ (int) {
			int res = mongo_cursor_next(mcursor);
			data = (bson*)mongo_cursor_bson(mcursor);
			if (res == MONGO_OK) {
				return true;
			} else {
				mongo_cursor_destroy(mcursor);
				return false;
			}
		}

	};

	//visual studio doesn't support Variadic Templates
	class MongoExecutor {
	private:
		mongo* pmongo;
		_BsonUnit condition;
		_BsonUnit operation;
		_BsonUnit* pbson;

	public:
		MongoExecutor(DBAdapter& pconn, bool op = false) {
			pmongo = pconn.mongo_con;
			if(op)
				pbson = &operation;
			else
				pbson = &condition;
		}
		MongoExecutor& Con() {
			pbson = &condition;
			return *this;
		}
		MongoExecutor& Op() {
			pbson = &operation;
			return *this;
		}
		MongoExecutor& operator () (const char* op) {
			pbson->begin_object(op);
			return *this;
		}
		template<typename T>
		MongoExecutor& operator () (const char* name, T val) {
			pbson->append_data(name, val);
			return *this;
		}
		MongoExecutor& operator () (const char* name, const std::string& val) {
			pbson->append_data(name, val.c_str());
			return *this;
		}
		MongoExecutor& operator () (const char* name, int size, void* data) {
			pbson->append_data(name, (const char*)data, size);
			return *this;
		}
		MongoExecutor& operator () () {
			pbson->finish_object();
			return *this;
		}
		template<typename T>
		MongoExecutor& operator () (const char* op, const char* name, T val) {
			pbson->begin_object(op);
			pbson->append_data(name, val);
			pbson->finish_object();
			return *this;
		}
		MongoExecutor& operator () (const char* op, const char* name, std::string& val) {
			pbson->begin_object(op);
			pbson->append_data(name, val.c_str());
			pbson->finish_object();
			return *this;
		}
		// Insert
		void operator + (const char* ns) {
			operation.finish();
			mongo_insert(pmongo, ns, operation.get_bson(), nullptr);
		}
		// Remove
		void operator - (const char* ns) {
			condition.finish();
			mongo_remove(pmongo, ns, condition.get_bson(), nullptr);
		}
		// Update
		void operator >> (const char* ns) {
			condition.finish();
			operation.finish();
			mongo_update(pmongo, ns, condition.get_bson(), operation.get_bson(), MONGO_UPDATE_MULTI | MONGO_UPDATE_UPSERT, nullptr);
		}
		// Query
		MongoQuery operator << (const char* ns) {
			condition.finish();
			MongoQuery mq;
			mongo_cursor_init(mq.mcursor, pmongo, ns);
			mongo_cursor_set_query(mq.mcursor, condition.get_bson());
			return mq;
		}

	};

	extern DBAdapter dbAdapter;

}
#endif
