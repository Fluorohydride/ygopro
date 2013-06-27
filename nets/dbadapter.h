#ifndef DBOBJECT_H
#define DBOBJECT_H

#include "../common/common.h"
#include "../mongo/mongo.h"

namespace ygopro
{
	//The compiler should support Variadic Templates

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
		inline void append_data(const char* name, int val) {
			bson_append_int(&stbson, name, val);
		}
		inline void append_data(const char* name, unsigned int val) {
			bson_append_int(&stbson, name, val);
		}
		inline void append_data(const char* name, long long val) {
			bson_append_long(&stbson, name, val);
		}
		inline void append_data(const char* name, unsigned long long val) {
			bson_append_long(&stbson, name, val);
		}
		inline void append_data(const char* name, double val) {
			bson_append_double(&stbson, name, val);
		}
		inline void append_data(const char* name, const char* val) {
			bson_append_string(&stbson, name, val);
		}
 		inline void append_data(const char* name, const std::string& val) {
 			bson_append_string(&stbson, name, val.c_str());
		}
		inline void append_data(const char* name, const char* data, int size) {
			bson_append_binary(&stbson, name, BSON_BIN_BINARY, data, size);
		}
		inline void append_data(const char* name, const std::pair<const char*, int>& val) {
			bson_append_start_object(&stbson, name);
			bson_append_int(&stbson, val.first, val.second);
			bson_append_finish_object(&stbson);
		}
		inline void begin_object(const char* obj) {
			bson_append_start_object(&stbson, obj);
		}
		inline void finish_object() {
			bson_append_finish_object(&stbson);
		}
		inline void finish() {
			bson_finish(&stbson);
		}

		inline void push() {}

		template<typename T, typename... TR>
		inline void push(const char* name, const T& val, const TR&... rest) {
			append_data(name, val);
			push(rest...);
		}

		inline bson* get_bson() {
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
		inline MongoQuery& operator () (const char* name, int& val, int defval = 0) {
			bson_iterator iter[1];
			if(bson_find(iter, data, name) != BSON_EOO)
				val = bson_iterator_int(iter);
			else val = defval;
			return *this;
		}
		inline MongoQuery& operator () (const char* name, unsigned int& val, unsigned int defval = 0) {
			bson_iterator iter[1];
			if(bson_find(iter, data, name) != BSON_EOO)
				val = bson_iterator_int(iter);
			else val = defval;
			return *this;
		}
		inline MongoQuery& operator () (const char* name, long long& val, long long defval = 0) {
			bson_iterator iter[1];
			if(bson_find(iter, data, name) != BSON_EOO)
				val = bson_iterator_long(iter);
			else val = defval;
			return *this;
		}
		inline MongoQuery& operator () (const char* name, unsigned long long& val, unsigned long long defval = 0) {
			bson_iterator iter[1];
			if(bson_find(iter, data, name) != BSON_EOO)
				val = bson_iterator_long(iter);
			else val = defval;
			return *this;
		}
		inline MongoQuery& operator () (const char* name, double& val, double defval = 0.0) {
			bson_iterator iter[1];
			if(bson_find(iter, data, name) != BSON_EOO)
				val = bson_iterator_double(iter);
			else val = defval;
			return *this;
		}
		inline MongoQuery& operator () (const char* name, std::string& val, const char* defval = "") {
			bson_iterator iter[1];
			if(bson_find(iter, data, name) != BSON_EOO)
				val = bson_iterator_string(iter);
			else val = defval;
			return *this;
		}
		inline MongoQuery& operator () (const char* name, unsigned char*& ptr, int& size) {
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
		void foreach(void (f)(const char*, int)) {
			bson_iterator iter[1];
			bson_iterator_init(iter, data);
			while(bson_iterator_next(iter)) {
				f(bson_iterator_key(iter), bson_iterator_int(iter));
			}
		}

		inline bool operator ++ (int) {
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

	class MongoExecutor {
	private:
		mongo* pmongo;
		_BsonUnit _condition;
		_BsonUnit _operation;
		bool _queried;

	public:
		MongoExecutor(DBAdapter& pconn): pmongo(pconn.mongo_con), _queried(false) {
		}

		template<typename... TR>
		inline MongoExecutor& condition(const TR&... rest) {
			_condition.push(rest...);
			return *this;
		}

		template<typename... TR>
		inline MongoExecutor& values(const TR&... rest) {
			_operation.push(rest...);
			return *this;
		}

		template<typename... TR>
		inline MongoExecutor& query(const TR&... rest) {
			_condition.begin_object("$query");
			_condition.push(rest...);
			_condition.finish_object();
			_queried = true;
			return *this;
		}

		template<typename... TR>
		inline MongoExecutor& orderby(const TR&... rest) {
			if(!_queried) {
				_condition.begin_object("$query");
				_condition.finish_object();
			}
			_condition.begin_object("$orderby");
			_condition.push(rest...);
			_condition.finish_object();
			return *this;
		}

		template<typename... TR>
		inline MongoExecutor& operation(const char* op, const TR&... rest) {
			_operation.begin_object(op);
			_operation.push(rest...);
			_operation.finish_object();
			return *this;
		}

		// Insert
		inline void insertTo(const char* ns) {
			_operation.finish();
			mongo_insert(pmongo, ns, _operation.get_bson(), nullptr);
		}
		// Remove
		inline void deleteFrom(const char* ns) {
			_condition.finish();
			mongo_remove(pmongo, ns, _condition.get_bson(), nullptr);
		}
		// Update
		inline void updateTo(const char* ns) {
			_condition.finish();
			_operation.finish();
			mongo_update(pmongo, ns, _condition.get_bson(), _operation.get_bson(), MONGO_UPDATE_MULTI | MONGO_UPDATE_UPSERT, nullptr);
		}
		// Query
		inline MongoQuery queryFrom(const char* ns) {
			_condition.finish();
			MongoQuery mq;
			mongo_cursor_init(mq.mcursor, pmongo, ns);
			mongo_cursor_set_query(mq.mcursor, _condition.get_bson());
			return mq;
		}

	};

	extern DBAdapter dbAdapter;

}
#endif
