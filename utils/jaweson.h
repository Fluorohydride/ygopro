#ifndef _JAWESON_H_
#define _JAWESON_H_

// version 0.5
// sample code:
//    jaweson::JsonRoot<> root;
//    if(!root.parse(buffer, len)) {
//        auto info = root.get_error_info();
//        auto& err_line = std::get<0>(info);
//        auto& err_col = std::get<1>(info);
//        auto& msg = std::get<2>(info);
//        std::cout << msg << " " << err_line << "," << err_col << std::endl;
//    } else {
//        root["nodename"].set_value<jaweson::JsonString<>>("teststring");
//        std::cout << root << std::endl;
//        // this is equivalent to
//        // jaweson::JsonStreamWriter<> writer(std::cout);
//        // root.write_to(writer);
//        // or
//        // std::string str;
//        // jaweson::JsonStringWriter<> writer(str);
//        // root.write_to(writer);
//        // std::cout << str;
//    }
// the parser uses a simple allocator in default which can be replaced with a custom allocator easily.

#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <limits>
#include <stdint.h>

namespace jaweson
{
    
#define TOKEN_UNKNOWN   0
#define TOKEN_LBRACE    1
#define TOKEN_RBRACE    2
#define TOKEN_LBRACKET  3
#define TOKEN_RBRACKET  4
#define TOKEN_NUMBER    5
#define TOKEN_STRING    6
#define TOKEN_TRUE      7
#define TOKEN_FALSE     8
#define TOKEN_NULL      9
#define TOKEN_COMMA     10
#define TOKEN_COLON     11
#define TOKEN_EOF       12
    
#define TOKEN_UNKNOWN_FLAG   0x1
#define TOKEN_LBRACE_FLAG    0x2
#define TOKEN_RBRACE_FLAG    0x4
#define TOKEN_LBRACKET_FLAG  0x8
#define TOKEN_RBRACKET_FLAG  0x10
#define TOKEN_NUMBER_FLAG    0x20
#define TOKEN_STRING_FLAG    0x40
#define TOKEN_TRUE_FLAG      0x80
#define TOKEN_FALSE_FLAG     0x100
#define TOKEN_NULL_FLAG      0x200
#define TOKEN_COMMA_FLAG     0x400
#define TOKEN_COLON_FLAG     0x800
#define TOKEN_EOF_FLAG       0x1000
#define TOKEN_ALLVALUE_FLAG  0x3ea
    
    class DefaultAllocator {
    public:
        template<typename T, typename... TR >
        static inline T* alloc(TR&&... params) { return new T(std::forward<TR>(params)...); }
        
        template<typename T>
        static inline void recycle(T* obj) { delete obj; }
    };
    
    class JsonWriter {
    public:
        virtual JsonWriter& operator << (const char* buf) = 0;
        virtual JsonWriter& operator << (const char ch) = 0;
        virtual JsonWriter& push_repeat(char ch, int32_t times) = 0;
        // >0 : increase indent;
        // =0 : no change
        // <0 : decrease indent;
        virtual JsonWriter& push_newline(int32_t indent_dif) = 0;
        virtual bool need_escape_slash() = 0;
    };
    
    class JsonUtil {
    public:
        static std::string raw_string_to_text(const std::string& raw, bool escape_slash) {
            std::string result;
            const char* p = raw.c_str();
            while(*p != 0) {
                uint8_t ch = *p++;
                switch(ch) {
                    case '"': result.append("\\\""); break;
                    case '\\': result.append("\\\\"); break;
                    case '/': result.append(escape_slash ? "\\/" : "/"); break;
                    case '\b': result.append("\\b"); break;
                    case '\f': result.append("\\f"); break;
                    case '\n': result.append("\\n"); break;
                    case '\r': result.append("\\r"); break;
                    case '\t': result.append("\\t"); break;
                    default: {
                        if(ch < ' ') {
                            char buffer[8];
                            sprintf(buffer, "\\u%4x", ch);
                            result.append(buffer);
                        } else
                            result.push_back(ch);
                        break;
                    }
                }
            }
            return std::move(result);
        }
        
        static inline uint8_t get_hex_value(uint8_t ch) {
            static const uint8_t hex_value[256] = {
                16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16, 16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
                16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16, 0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,16,16,16,16,16,16,
                16,10,11,12,13,14,15,16,16,16,16,16,16,16,16,16, 16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
                16,10,11,12,13,14,15,16,16,16,16,16,16,16,16,16, 16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
                16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16, 16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
                16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16, 16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
                16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16, 16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
                16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16, 16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
            };
            return hex_value[ch];
        }
        
        static inline uint8_t get_escape_value(uint8_t ch) {
            static const uint8_t escape_value[256] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, '"', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '/',  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '\\', 0, 0, 0,
                0, 0, '\b', 0, 0, 0, '\f', 0, 0, 0, 0, 0, 0, 0, '\n', 0,  0, 0, '\r', 0, '\t', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            };
            return escape_value[ch];
        }
        
        static inline uint32_t get_token_type(uint8_t ch) {
            static const uint8_t token_type[256] = {
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10,  5,  0,  0,
                5,  5,  5,  5,  5,  5,  5,  5,  5,  5, 11,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  0,  4,  0,  0,
                0,  0,  0,  0,  0,  0,  8,  0,  0,  0,  0,  0,  0,  0,  9,  0,
                0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,  1,  0,  2,  0,  0,
            };
            return token_type[ch];
        }
        
        static inline double pow10(int32_t e) {
            static const double value[] = {
                1e-308,1e-307,1e-306,1e-305,1e-304,1e-303,1e-302,1e-301,1e-300,
                1e-299,1e-298,1e-297,1e-296,1e-295,1e-294,1e-293,1e-292,1e-291,1e-290,1e-289,1e-288,1e-287,1e-286,1e-285,1e-284,1e-283,1e-282,1e-281,1e-280,
                1e-279,1e-278,1e-277,1e-276,1e-275,1e-274,1e-273,1e-272,1e-271,1e-270,1e-269,1e-268,1e-267,1e-266,1e-265,1e-264,1e-263,1e-262,1e-261,1e-260,
                1e-259,1e-258,1e-257,1e-256,1e-255,1e-254,1e-253,1e-252,1e-251,1e-250,1e-249,1e-248,1e-247,1e-246,1e-245,1e-244,1e-243,1e-242,1e-241,1e-240,
                1e-239,1e-238,1e-237,1e-236,1e-235,1e-234,1e-233,1e-232,1e-231,1e-230,1e-229,1e-228,1e-227,1e-226,1e-225,1e-224,1e-223,1e-222,1e-221,1e-220,
                1e-219,1e-218,1e-217,1e-216,1e-215,1e-214,1e-213,1e-212,1e-211,1e-210,1e-209,1e-208,1e-207,1e-206,1e-205,1e-204,1e-203,1e-202,1e-201,1e-200,
                1e-199,1e-198,1e-197,1e-196,1e-195,1e-194,1e-193,1e-192,1e-191,1e-190,1e-189,1e-188,1e-187,1e-186,1e-185,1e-184,1e-183,1e-182,1e-181,1e-180,
                1e-179,1e-178,1e-177,1e-176,1e-175,1e-174,1e-173,1e-172,1e-171,1e-170,1e-169,1e-168,1e-167,1e-166,1e-165,1e-164,1e-163,1e-162,1e-161,1e-160,
                1e-159,1e-158,1e-157,1e-156,1e-155,1e-154,1e-153,1e-152,1e-151,1e-150,1e-149,1e-148,1e-147,1e-146,1e-145,1e-144,1e-143,1e-142,1e-141,1e-140,
                1e-139,1e-138,1e-137,1e-136,1e-135,1e-134,1e-133,1e-132,1e-131,1e-130,1e-129,1e-128,1e-127,1e-126,1e-125,1e-124,1e-123,1e-122,1e-121,1e-120,
                1e-119,1e-118,1e-117,1e-116,1e-115,1e-114,1e-113,1e-112,1e-111,1e-110,1e-109,1e-108,1e-107,1e-106,1e-105,1e-104,1e-103,1e-102,1e-101,1e-100,
                1e-99, 1e-98, 1e-97, 1e-96, 1e-95, 1e-94, 1e-93, 1e-92, 1e-91, 1e-90, 1e-89, 1e-88, 1e-87, 1e-86, 1e-85, 1e-84, 1e-83, 1e-82, 1e-81, 1e-80,
                1e-79, 1e-78, 1e-77, 1e-76, 1e-75, 1e-74, 1e-73, 1e-72, 1e-71, 1e-70, 1e-69, 1e-68, 1e-67, 1e-66, 1e-65, 1e-64, 1e-63, 1e-62, 1e-61, 1e-60,
                1e-59, 1e-58, 1e-57, 1e-56, 1e-55, 1e-54, 1e-53, 1e-52, 1e-51, 1e-50, 1e-49, 1e-48, 1e-47, 1e-46, 1e-45, 1e-44, 1e-43, 1e-42, 1e-41, 1e-40,
                1e-39, 1e-38, 1e-37, 1e-36, 1e-35, 1e-34, 1e-33, 1e-32, 1e-31, 1e-30, 1e-29, 1e-28, 1e-27, 1e-26, 1e-25, 1e-24, 1e-23, 1e-22, 1e-21, 1e-20,
                1e-19, 1e-18, 1e-17, 1e-16, 1e-15, 1e-14, 1e-13, 1e-12, 1e-11, 1e-10, 1e-9,  1e-8,  1e-7,  1e-6,  1e-5,  1e-4,  1e-3,  1e-2,  1e-1,  1e+0,
                1e+1,  1e+2,  1e+3,  1e+4,  1e+5,  1e+6,  1e+7,  1e+8,  1e+9,  1e+10, 1e+11, 1e+12, 1e+13, 1e+14, 1e+15, 1e+16, 1e+17, 1e+18, 1e+19, 1e+20,
                1e+21, 1e+22, 1e+23, 1e+24, 1e+25, 1e+26, 1e+27, 1e+28, 1e+29, 1e+30, 1e+31, 1e+32, 1e+33, 1e+34, 1e+35, 1e+36, 1e+37, 1e+38, 1e+39, 1e+40,
                1e+41, 1e+42, 1e+43, 1e+44, 1e+45, 1e+46, 1e+47, 1e+48, 1e+49, 1e+50, 1e+51, 1e+52, 1e+53, 1e+54, 1e+55, 1e+56, 1e+57, 1e+58, 1e+59, 1e+60,
                1e+61, 1e+62, 1e+63, 1e+64, 1e+65, 1e+66, 1e+67, 1e+68, 1e+69, 1e+70, 1e+71, 1e+72, 1e+73, 1e+74, 1e+75, 1e+76, 1e+77, 1e+78, 1e+79, 1e+80,
                1e+81, 1e+82, 1e+83, 1e+84, 1e+85, 1e+86, 1e+87, 1e+88, 1e+89, 1e+90, 1e+91, 1e+92, 1e+93, 1e+94, 1e+95, 1e+96, 1e+97, 1e+98, 1e+99, 1e+100,
                1e+101,1e+102,1e+103,1e+104,1e+105,1e+106,1e+107,1e+108,1e+109,1e+110,1e+111,1e+112,1e+113,1e+114,1e+115,1e+116,1e+117,1e+118,1e+119,1e+120,
                1e+121,1e+122,1e+123,1e+124,1e+125,1e+126,1e+127,1e+128,1e+129,1e+130,1e+131,1e+132,1e+133,1e+134,1e+135,1e+136,1e+137,1e+138,1e+139,1e+140,
                1e+141,1e+142,1e+143,1e+144,1e+145,1e+146,1e+147,1e+148,1e+149,1e+150,1e+151,1e+152,1e+153,1e+154,1e+155,1e+156,1e+157,1e+158,1e+159,1e+160,
                1e+161,1e+162,1e+163,1e+164,1e+165,1e+166,1e+167,1e+168,1e+169,1e+170,1e+171,1e+172,1e+173,1e+174,1e+175,1e+176,1e+177,1e+178,1e+179,1e+180,
                1e+181,1e+182,1e+183,1e+184,1e+185,1e+186,1e+187,1e+188,1e+189,1e+190,1e+191,1e+192,1e+193,1e+194,1e+195,1e+196,1e+197,1e+198,1e+199,1e+200,
                1e+201,1e+202,1e+203,1e+204,1e+205,1e+206,1e+207,1e+208,1e+209,1e+210,1e+211,1e+212,1e+213,1e+214,1e+215,1e+216,1e+217,1e+218,1e+219,1e+220,
                1e+221,1e+222,1e+223,1e+224,1e+225,1e+226,1e+227,1e+228,1e+229,1e+230,1e+231,1e+232,1e+233,1e+234,1e+235,1e+236,1e+237,1e+238,1e+239,1e+240,
                1e+241,1e+242,1e+243,1e+244,1e+245,1e+246,1e+247,1e+248,1e+249,1e+250,1e+251,1e+252,1e+253,1e+254,1e+255,1e+256,1e+257,1e+258,1e+259,1e+260,
                1e+261,1e+262,1e+263,1e+264,1e+265,1e+266,1e+267,1e+268,1e+269,1e+270,1e+271,1e+272,1e+273,1e+274,1e+275,1e+276,1e+277,1e+278,1e+279,1e+280,
                1e+281,1e+282,1e+283,1e+284,1e+285,1e+286,1e+287,1e+288,1e+289,1e+290,1e+291,1e+292,1e+293,1e+294,1e+295,1e+296,1e+297,1e+298,1e+299,1e+300,
                1e+301,1e+302,1e+303,1e+304,1e+305,1e+306,1e+307,1e+308
            };
            return (e < -308) ? 0.0 : (e > 308) ? std::numeric_limits<double>::infinity() : value[e + 308];
        }
    };
    
    class JsonValue;
    
    class JsonType {
    public:
        JsonType() {};
        virtual ~JsonType() {};
        virtual bool is_number() { return false; }
        virtual bool is_string() { return false; }
        virtual bool is_null() { return false; }
        virtual bool is_bool() { return false; }
        virtual bool is_object() { return false; }
        virtual bool is_array() { return false; }
        virtual bool is_empty() { return false; }
        virtual int64_t to_integer() { return 0; }
        virtual double to_double() { return 0.0; }
        virtual bool to_bool() { return false; }
        virtual const std::string& to_string() { static std::string empty_str; return empty_str; }
        virtual JsonValue& operator [] (const std::string& key);
        virtual JsonValue& operator [] (int32_t index);
        virtual bool insert(const std::string& key, JsonType* value) { return false; }
        virtual bool insert(JsonType* value) { return false; }
        virtual bool erase(const std::string& key) { return false; }
        virtual bool erase(int32_t index) { return false; }
        virtual void for_each(const std::function<void(const std::string&, JsonValue&)>& fun) {}
        virtual void for_each(const std::function<void(int32_t, JsonValue&)>& fun) {}
        virtual size_t size() { return 0; }
        virtual void write_to(JsonWriter& writer) = 0;
        virtual JsonType* clone() = 0;
        virtual void free() = 0;
        
        template<typename CONV_TYPE>
        inline CONV_TYPE to_value() { return static_cast<CONV_TYPE>(to_integer()); }
        
    protected:
        JsonType(const JsonType&){};
    };
    
    template<> inline bool JsonType::to_value<bool>() { return to_bool(); }
    template<> inline float JsonType::to_value<float>() { return (float)to_double(); }
    template<> inline double JsonType::to_value<double>() { return to_double(); }
    template<> inline std::string JsonType::to_value<std::string>() { return to_string(); }
    
    template<typename ALLOC_TYPE = DefaultAllocator>
    class JsonDouble : public JsonType {
    public:
        inline JsonDouble(double value) : ref_value(value) {}
        inline virtual bool is_number() { return true; }
        inline virtual int64_t to_integer() { return static_cast<int64_t>(ref_value); }
        inline virtual double to_double() { return ref_value; }
        virtual void write_to(JsonWriter& writer) {
            char buffer[32];
            sprintf(buffer, "%.9lg", ref_value);
            writer << buffer;
        }
        virtual JsonType* clone() { return ALLOC_TYPE::template alloc<JsonDouble>(ref_value); }
        virtual void free() { ALLOC_TYPE::template recycle(this); }
        static inline JsonDouble<ALLOC_TYPE>* create(double val) { return ALLOC_TYPE::template alloc<JsonDouble>(val); }
    protected:
        double ref_value = 0.0;
    };
    
    template<typename ALLOC_TYPE = DefaultAllocator>
    class JsonInteger : public JsonType {
    public:
        inline JsonInteger(int64_t value) : ref_value(value) {}
        inline virtual bool is_number() { return true; }
        inline virtual int64_t to_integer() { return ref_value; }
        inline virtual double to_double() { return static_cast<double>(ref_value); }
        virtual void write_to(JsonWriter& writer) {
            char buffer[32];
            sprintf(buffer, "%lld", ref_value);
            writer << buffer;
        }
        virtual JsonType* clone() { return ALLOC_TYPE::template alloc<JsonInteger>(ref_value); }
        virtual void free() { ALLOC_TYPE::template recycle(this); }
        static inline JsonInteger<ALLOC_TYPE>* create(int64_t val) { return ALLOC_TYPE::template alloc<JsonInteger>(val); }
    protected:
        int64_t ref_value = 0;
    };
    
    template<typename ALLOC_TYPE = DefaultAllocator>
    class JsonString : public JsonType {
    public:
        inline JsonString(const std::string& value) : ref_value(value) {}
        inline JsonString(std::string&& value) : ref_value(std::move(value)) {}
        inline virtual bool is_string() { return true; }
        inline virtual const std::string& to_string() { return ref_value; }
        virtual void write_to(JsonWriter& writer) {
            writer << '\"' << JsonUtil::raw_string_to_text(ref_value, writer.need_escape_slash()).c_str() << '\"';
        }
        virtual JsonType* clone() { return ALLOC_TYPE::template alloc<JsonString>(ref_value); }
        virtual void free() { ALLOC_TYPE::template recycle(this); }
        static inline JsonString<ALLOC_TYPE>* create(const std::string& val) { return ALLOC_TYPE::template alloc<JsonString>(val); }
        static inline JsonString<ALLOC_TYPE>* create(std::string&& val) { return ALLOC_TYPE::template alloc<JsonString>(std::move(val)); }
    protected:
        std::string ref_value;
    };
    
    template<typename ALLOC_TYPE = DefaultAllocator>
    class JsonBool : public JsonType {
    public:
        inline JsonBool(bool value) : ref_value(value) {}
        virtual bool is_bool() { return true; }
        virtual bool to_bool() { return ref_value; }
        virtual void write_to(JsonWriter& writer) {
            writer << (ref_value ? "true" : "false");
        }
        virtual JsonType* clone() { return create(ref_value); }
        virtual void free() {}
        static JsonBool<ALLOC_TYPE>* create(bool val) {
            static JsonBool<ALLOC_TYPE> stTrue(true);
            static JsonBool<ALLOC_TYPE> stFalse(false);
            return val ? &stTrue : &stFalse;
        }
    protected:
        bool ref_value = 0;
    };
    
    class JsonNull : public JsonType {
    public:
        inline JsonNull() {}
        virtual bool is_null() { return true; }
        virtual void write_to(JsonWriter& writer) {
            writer << "null";
        }
        virtual JsonType* clone() { return create(); }
        virtual void free() {}
        static JsonNull* create() {
            static JsonNull stNull;
            return &stNull;
        }
    };
    
    class JsonValue {
    public:
        JsonValue(const JsonValue&) = delete;
        inline JsonValue(JsonType* value) { ref_value = value; }
        virtual ~JsonValue() { if(ref_value) ref_value->free(); }
        virtual void free() = 0;
        
        inline bool is_number() { return ref_value->is_number(); }
        inline bool is_string() { return ref_value->is_string(); }
        inline bool is_bool() { return ref_value->is_bool(); }
        inline bool is_null() { return ref_value->is_null(); }
        inline bool is_object() { return ref_value->is_object(); }
        inline bool is_array() { return ref_value->is_array(); }
        inline bool is_empty() { return ref_value->is_empty(); }
        inline int64_t to_integer() { return ref_value->to_integer(); }
        inline double to_double() { return ref_value->to_double(); }
        virtual bool to_bool() { return ref_value->to_bool(); }
        inline const std::string& to_string() { return ref_value->to_string(); }
        inline JsonValue& operator [] (const std::string& key) { return (*ref_value)[key]; }
        inline JsonValue& operator [] (int32_t index) { return (*ref_value)[index]; }
        inline bool insert(const std::string& key, JsonType* value) { return ref_value->insert(key, value); }
        inline bool insert(JsonType* value) { return ref_value->insert(value); }
        inline bool erase(const std::string& key) { return ref_value->erase(key); }
        inline bool erase(int32_t index) { return ref_value->erase(index); }
        inline void for_each(const std::function<void(const std::string&, JsonValue&)>& fun) { ref_value->for_each(fun); }
        inline void for_each(const std::function<void(int32_t, JsonValue&)>& fun) { ref_value->for_each(fun); }
        inline size_t size() { return ref_value->size(); }
        inline void write_to(JsonWriter& writer) { ref_value->write_to(writer); }
        inline JsonType* clone() { return ref_value->clone(); }
        inline void attach(JsonValue& node) {
            if(ref_value->is_empty()|| !node.ref_value) return;
            if(ref_value)
                ref_value->free();
            ref_value = node.ref_value;
            node.ref_value = JsonNull::create();
        }
        inline void operator = (JsonValue& node) {
            if(ref_value->is_empty()) return;
            auto val = node.clone();
            if(val == nullptr)
                return;
            if(ref_value != nullptr)
                ref_value->free();
            ref_value = node.clone();
        }
        template<typename VALUE_TYPE, typename... TR>
        inline void set_value(TR&&... params) {
            static_assert(std::is_base_of<JsonType, VALUE_TYPE>::value, "VALUE_TYPE should be subclass of JsonType.");
            if(ref_value->is_empty()) return;
            if(ref_value != nullptr)
                ref_value->free();
            ref_value = VALUE_TYPE::create(std::forward<TR>(params)...);
        }
        template<typename CONV_TYPE>
        inline CONV_TYPE to_value() { return ref_value->template to_value<CONV_TYPE>(); }
        
    protected:
        JsonType* ref_value = nullptr;
    };
    
    class JsonEmptyValue : public JsonValue {
    public:
        virtual void free() {}
        static JsonEmptyValue& Get() {
            static JsonEmptyValue empty_value;
            return empty_value;
        }
    protected:
        class JsonEmptyType : public JsonType {
        public:
            virtual bool is_empty() { return true; }
            virtual void write_to(JsonWriter& writer) {}
            virtual JsonType* clone() { return nullptr; }
            virtual void free() {}
        };
        
        inline JsonEmptyValue() : JsonValue(nullptr) {
            static JsonEmptyType empty_val;
            JsonValue::ref_value = &empty_val;
        };
    };
    
    inline JsonValue& JsonType::operator [] (const std::string& key) {
        return JsonEmptyValue::Get();
    }
    
    inline JsonValue& JsonType::operator [] (int32_t index) {
        return JsonEmptyValue::Get();
    }
    
    template<typename ALLOC_TYPE = DefaultAllocator>
    class JsonObject : public JsonType {
    public:
        inline JsonObject() { values.reserve(16); }
        ~JsonObject() {
            for(auto& iter : values)
                if(iter)
                    iter->free();
        }
        virtual bool is_object() { return true; }
        virtual JsonValue& operator [] (const std::string& key) {
            if(!has_index)
                build_index();
            auto iter = indices.find(key);
            if(iter == indices.end()) {
                std::string inner_key = key;
                auto null_value = JsonNull::create();
                values.push_back(ALLOC_TYPE::template alloc<JsonValueObject>(inner_key, null_value));
                return *values.back();
            } else
                return *values[iter->second];
        }
        
        virtual bool insert(const std::string& key, JsonType* value) {
            if(has_index) {
                auto iter = indices.find(key);
                if(iter == indices.end()) {
                    std::string inner_key = key;
                    auto null_value = JsonNull::create();
                    values.push_back(ALLOC_TYPE::template alloc<JsonValueObject>(inner_key, null_value));
                    indices[key] = (int32_t)values.size() - 1;
                } else
                    return false;
            } else {
                std::string inner_key = key;
                values.push_back(ALLOC_TYPE::template alloc<JsonValueObject>(inner_key, value));
            }
            return true;
        }
        
        virtual bool erase(const std::string& key) {
            if(!has_index)
                build_index();
            auto iter = indices.find(key);
            if(iter == indices.end())
                return false;
            values[iter->second]->free();
            values[iter->second] = nullptr;
            indices.erase(iter);
            return true;
        }
        
        virtual void for_each(const std::function<void(const std::string&, JsonValue&)>& fun) {
            if(!fun)
                return;
            for(auto& iter : values)
                if(iter)
                    fun(iter->ref_key, *iter);
        }
        
        virtual size_t size() { return values.size(); }
        virtual void write_to(JsonWriter& writer) {
            writer << '{';
            if(values.empty()) {
                writer << '}';
            } else {
                writer.push_newline(1);
                for(size_t i = 0; i < values.size(); ++i) {
                    auto& value_node = values[i];
                    if(value_node) {
                        writer << '\"' << JsonUtil::raw_string_to_text(value_node->ref_key, writer.need_escape_slash()).c_str() << "\" : ";
                        value_node->write_to(writer);
                        if(i != values.size() - 1) {
                            writer << ',';
                            writer.push_newline(0);
                        } else
                            writer.push_newline(-1);
                    }
                }
                writer << '}';
            }
        }
        
        virtual JsonType* clone() {
            JsonObject<ALLOC_TYPE>* obj = ALLOC_TYPE::template alloc<JsonObject>();
            for(auto& iter : values)
                if(iter)
                    obj->insert(iter->ref_key, iter->clone());
            return obj;
        }
        virtual void free() { ALLOC_TYPE::template recycle(this); }
        static JsonObject<ALLOC_TYPE>* create() { return ALLOC_TYPE::template alloc<JsonObject>(); }
        
        inline void reserve_value(std::string& key) {
            values.push_back(ALLOC_TYPE::template alloc<JsonValueObject>(key, nullptr));
        }
        
        inline void fulfill_value(JsonType* value) {
            values.back()->set_value(value);
        }
        
        inline void init(std::string& key, JsonType* value) {
            values.push_back(ALLOC_TYPE::template alloc<JsonValueObject>(key, value));
        }
        
    protected:
        class JsonValueObject : public JsonValue {
        public:
            inline JsonValueObject(std::string& key, JsonType* val = nullptr) : JsonValue(val), ref_key(std::move(key)) {}
            virtual void free() { ALLOC_TYPE::template recycle(this); }
            
            inline void set_value(JsonType* value) {
                if(JsonValue::ref_value)
                    JsonValue::ref_value->free();
                JsonValue::ref_value = value;
            }
            std::string ref_key;
        };
        
        void build_index() {
            for(int32_t i = 0; i < values.size(); ++i) {
                if(values[i])
                    indices[values[i]->ref_key] = i;
            }
            has_index = true;
        }
        
        bool has_index = false;
        std::vector<typename JsonObject<ALLOC_TYPE>::JsonValueObject*> values;
        std::map<std::string, int32_t> indices;
        
    };
    
    template<typename ALLOC_TYPE = DefaultAllocator>
    class JsonArray : public JsonType {
    public:
        inline JsonArray() { values.reserve(16); }
        ~JsonArray() {
            for(auto& iter : values)
                iter->free();
        }
        virtual bool is_array() { return true; }
        virtual JsonValue& operator [] (int32_t index) {
            if(index < 0 || index >= values.size())
                return JsonEmptyValue::Get();
            return *values[index];
        }
        virtual bool insert(JsonType* value) {
            values.push_back(ALLOC_TYPE::template alloc<JsonValueArray>(value));
            return true;
        }
        
        virtual bool erase(int32_t index) {
            if(index >= values.size())
                return false;
            values.erase(values.begin() + index);
            return true;
        }
        
        virtual void for_each(const std::function<void(int32_t, JsonValue&)>& fun) {
            if(!fun)
                return;
            for(size_t i = 0; i < values.size(); ++i)
                fun((int32_t)i, *values[i]);
        }
        
        virtual size_t size() { return values.size(); }
        virtual void write_to(JsonWriter& writer) {
            writer << '[';
            for(size_t i = 0; i < values.size(); ++i) {
                auto& value_node = values[i];
                if(value_node)
                    value_node->write_to(writer);
                if(i != values.size() - 1)
                    writer << ',' << ' ';
            }
            writer << ']';
        }
        
        virtual JsonType* clone() {
            JsonArray<ALLOC_TYPE>* obj = ALLOC_TYPE::template alloc<JsonArray>();
            for(auto& iter : values)
                obj->values.push_back(ALLOC_TYPE::template alloc<JsonValueArray>(iter->clone()));
            return obj;
        }
        virtual void free() { ALLOC_TYPE::template recycle(this); }
        static JsonArray<ALLOC_TYPE>* create() { return ALLOC_TYPE::template alloc<JsonArray>(); }
        
    protected:
        class JsonValueArray : public JsonValue {
        public:
            JsonValueArray(JsonType* val = nullptr) : JsonValue(val) {}
            virtual void free() { ALLOC_TYPE::template recycle(this); }
        };
        std::vector<JsonValueArray*> values;
    };
    
    template<typename ALLOC_TYPE = DefaultAllocator>
    class JsonRoot : public JsonValue {
    public:
        JsonRoot() : JsonValue(nullptr) {}
        virtual void free() {}
        
        bool parse(const char* text, size_t len) {
            JsonParseStatus status;
            err_line = 0;
            err_col = 0;
            err_msg.clear();
            if(status.parse(text, (int32_t)len)) {
                JsonValue::ref_value = status.ret_value;
                return true;
            } else {
                uintptr_t error_pos = (uintptr_t)(status.cur_ptr - status.begin_ptr);
                err_line = status.err_line;
                err_col = (int32_t)(error_pos - status.err_col);
                err_msg = std::move(status.error_msg);
                return false;
            }
        }
        
        bool parse(const std::string& text) {
            return parse(text.c_str(), text.length());
        }
        
        std::tuple<int32_t, int32_t, std::string> get_error_info() {
            return std::make_tuple(err_line + 1, err_col + 1, err_msg);
        }
        
    protected:
        int32_t err_line = 0;
        int32_t err_col = 0;
        std::string err_msg;
        
        class JsonParseStatus {
        public:
            bool parse(const char* text, int32_t len) {
                begin_ptr = text;
                end_ptr = text + len;
                err_line = 1;
                err_col = 0;
                if((len >= 3) && (begin_ptr[0] == (char)0xef) && (begin_ptr[1] == (char)0xbb) && (begin_ptr[2] == (char)0xbf)) {
                    begin_ptr += 3;
                    err_col = 3;
                }
                cur_ptr = begin_ptr;
                error_msg.clear();
                if(begin_ptr >= end_ptr) {
                    error_msg = "invalid buffer.";
                    return false;
                }
                int32_t token_type = check_next_token();
                ret_value = nullptr;
                if(token_type == TOKEN_LBRACE)
                    ret_value = parse_object();
                else if(token_type == TOKEN_LBRACKET)
                    ret_value = parse_array();
                else {
                    log_expecting_error(TOKEN_LBRACE_FLAG | TOKEN_LBRACKET_FLAG);
                    return false;
                }
                return ret_value != nullptr;
            }
            
            void log_expecting_error(uint32_t expecting) {
                error_msg = "expecting ";
                if(expecting & TOKEN_LBRACE_FLAG)
                    error_msg.append(" \"{\",");
                if(expecting & TOKEN_RBRACE_FLAG)
                    error_msg.append(" \"}\",");
                if(expecting & TOKEN_LBRACKET_FLAG)
                    error_msg.append(" \"[\",");
                if(expecting & TOKEN_RBRACKET_FLAG)
                    error_msg.append(" \"]\",");
                if(expecting & TOKEN_NUMBER_FLAG)
                    error_msg.append(" number,");
                if(expecting & TOKEN_STRING_FLAG)
                    error_msg.append(" string,");
                if(expecting & TOKEN_TRUE_FLAG)
                    error_msg.append(" \"true\",");
                if(expecting & TOKEN_FALSE_FLAG)
                    error_msg.append(" \"false\",");
                if(expecting & TOKEN_NULL_FLAG)
                    error_msg.append(" \"null\",");
                if(expecting & TOKEN_COMMA_FLAG)
                    error_msg.append(" \",\",");
                if(expecting & TOKEN_COLON_FLAG)
                    error_msg.append(" \":\",");
                if(expecting & TOKEN_EOF_FLAG)
                    error_msg.append(" EOF,");
                error_msg.pop_back();
            }
            
            inline int32_t check_next_token() {
                while(cur_ptr < end_ptr && ((uint8_t)(*cur_ptr) <= ' ')) {
                    if(*cur_ptr == '\n') {
                        err_line++;
                        err_col = cur_ptr - begin_ptr;
                    }
                    cur_ptr++;
                }
                if(cur_ptr == end_ptr)
                    return TOKEN_EOF;
                return JsonUtil::get_token_type(*cur_ptr);
            }
            
            inline JsonType* parse_true() {
                if(cur_ptr + 4 < end_ptr && *(uint32_t*)cur_ptr == 0x65757274) {
                    cur_ptr += 4;
                    return JsonBool<ALLOC_TYPE>::create(true);
                }
                log_expecting_error(TOKEN_ALLVALUE_FLAG);
                return nullptr;
            }
            inline JsonType* parse_false() {
                if(cur_ptr + 5 < end_ptr && *(uint32_t*)(cur_ptr + 1) == 0x65736c61) {
                    cur_ptr += 5;
                    return JsonBool<ALLOC_TYPE>::create(false);
                }
                log_expecting_error(TOKEN_ALLVALUE_FLAG);
                return nullptr;
            }
            inline JsonType* parse_null() {
                if(cur_ptr + 4 < end_ptr && *(uint32_t*)cur_ptr == 0x6c6c756e) {
                    cur_ptr += 4;
                    return JsonNull::create();
                }
                log_expecting_error(TOKEN_ALLVALUE_FLAG);
                return nullptr;
            }
            
#define RETURN_ERR(x) {error_msg = x; return nullptr; }
            
            JsonType* parse_number() {
                bool is_integer = true;
                int64_t int_value = 0;
                double double_value = 0.0;
                bool digit = false, e = false, em = false, nd = true, end = false, neg = false, eneg = false;
                double dg = 0.1;
                int16_t epow = 0;
                if(*cur_ptr == '-') {
                    cur_ptr++;
                    neg = true;
                }
                while(!end && cur_ptr < end_ptr) {
                    uint8_t ch = *cur_ptr++;
                    switch(ch) {
                        case '.': {
                            if(digit || e || nd) RETURN_ERR("invalid number.")
                            digit = true;
                            nd = true;
                            if(is_integer) {
                                is_integer = false;
                                double_value = static_cast<double>(int_value);
                            }
                            break;
                        }
                        case 'e':
                        case 'E': {
                            if(e || nd) RETURN_ERR("invalid number.")
                            if(is_integer) {
                                is_integer = false;
                                double_value = static_cast<double>(int_value);
                            }
                            e = true;
                            em = true;
                            nd = true;
                            break;
                        }
                        case '+':
                        case '-': {
                            if(!em) RETURN_ERR("invalid number.")
                            em = false;
                            if(ch == '-')
                                eneg = true;
                            break;
                        }
                        default: {
                            if(ch < '0' || ch > '9') {
                                end = true;
                                cur_ptr--;
                                break;
                            }
                            if(e) {
                                epow = epow * 10 + ch - '0';
                            } else {
                                if(is_integer) {
                                    int_value = int_value * 10 + ch - '0';
                                } else {
                                    double_value += dg * (ch - '0');
                                    dg /= 10.0;
                                }
                            }
                            em = false;
                            nd = false;
                            break;
                        }
                    }
                }
                if(nd) RETURN_ERR("invalid number.")
                if(is_integer) {
                    if(neg)
                        int_value = -int_value;
                } else {
                    if(eneg)
                        epow = -epow;
                    double_value *= JsonUtil::pow10(epow);
                    if(neg)
                        double_value = -double_value;
                }
                if(is_integer)
                    return JsonInteger<ALLOC_TYPE>::create(int_value);
                else
                    return JsonDouble<ALLOC_TYPE>::create(double_value);
            }
            
            JsonType* parse_string() {
                cur_ptr++;
                std::string str;
                if(parse_string(str))
                    return JsonString<ALLOC_TYPE>::create(std::move(str));
                return nullptr;
            }
            
            bool parse_string(std::string& parse_str) {
                static std::vector<char> result_buffer;
                result_buffer.resize(0);
                while(cur_ptr < end_ptr) {
                    uint8_t ch = *cur_ptr++;
                    if(ch != '\\') {
                        if(ch == '"') {
                            parse_str.append(&result_buffer[0], result_buffer.size());
                            return true;
                        } else if(ch >= ' ') {
                            result_buffer.push_back(ch);
                        } else RETURN_ERR("unterminated string literal.")
                    } else {
                        if(cur_ptr >= end_ptr) RETURN_ERR("unterminated string literal.")
                        uint8_t ch2 = *cur_ptr++;
                        if(ch2 == 'u') {
                            if(cur_ptr + 4 > end_ptr) RETURN_ERR("unterminated string literal.")
                            uint16_t wchar = 0;
                            for(int32_t i = 0; i < 4; ++i) {
                                uint8_t h = JsonUtil::get_hex_value(cur_ptr[i]);
                                if(h > 15) RETURN_ERR("invalid hex value.")
                                wchar = (wchar << 4) | h;
                            }
                            if(wchar < 0x80) {
                                result_buffer.push_back(static_cast<char>(wchar));
                            } else if(wchar < 0x800) {
                                result_buffer.push_back(static_cast<char>(((wchar >> 6) & 0x1f) | 0xc0));
                                result_buffer.push_back(static_cast<char>((wchar & 0x3f) | 0x80));
                            } else {
                                result_buffer.push_back(static_cast<char>(((wchar >> 12) & 0xf) | 0xe0));
                                result_buffer.push_back(static_cast<char>(((wchar >> 6) & 0x3f) | 0x80));
                                result_buffer.push_back(static_cast<char>((wchar & 0x3f) | 0x80));
                            }
                            cur_ptr += 4;
                        } else {
                            uint8_t escape_char = JsonUtil::get_escape_value(ch2);
                            if(escape_char)
                                result_buffer.push_back(escape_char);
                            else RETURN_ERR("unknown escape charactor.")
                        }
                    }
                }
                return nullptr;
            }
            
            JsonType* parse_value() {
                uint32_t token_type = check_next_token();
                if((1 << token_type) & TOKEN_ALLVALUE_FLAG) {
                    switch (token_type) {
                        case TOKEN_LBRACE:
                            return parse_object();
                        case TOKEN_LBRACKET:
                            return parse_array();
                        case TOKEN_NUMBER:
                            return parse_number();
                        case TOKEN_STRING:
                            return parse_string();
                        case TOKEN_TRUE:
                            return parse_true();
                        case TOKEN_FALSE:
                            return parse_false();
                        case TOKEN_NULL:
                            return parse_null();
                        default:
                            return nullptr;
                    }
                } else {
                    log_expecting_error(TOKEN_ALLVALUE_FLAG);
                    return nullptr;
                }
            }
            
            JsonType* parse_object() {
                cur_ptr++;
                uint32_t token_type = check_next_token();
                JsonObject<ALLOC_TYPE>* obj = JsonObject<ALLOC_TYPE>::create();
                if(token_type == TOKEN_RBRACE) {
                    cur_ptr++;
                    return obj;
                }
                int32_t warn_line = 0;
                int32_t warn_col = 0;
                while(true) {
                    uint32_t token_type = check_next_token();
                    if(token_type != TOKEN_STRING) {
                        if(token_type == TOKEN_RBRACE) {
                            std::cerr << "Warning: redundant \",\" detected at line " << warn_line << ", column " << warn_col << std::endl;
                            cur_ptr++;
                            return obj;
                        } else {
                            log_expecting_error(TOKEN_STRING_FLAG);
                            obj->free();
                            return nullptr;
                        }
                    }
                    cur_ptr++;
                    std::string key;
                    if(!parse_string(key)) {
                        obj->free();
                        return nullptr;
                    }
                    token_type = check_next_token();
                    if(token_type != TOKEN_COLON) {
                        log_expecting_error(TOKEN_COLON_FLAG);
                        obj->free();
                        return nullptr;
                    }
                    cur_ptr++;
                    JsonType* val = parse_value();
                    if(!val) {
                        obj->free();
                        return nullptr;
                    }
                    obj->init(key, val);
                    token_type = check_next_token();
                    if(token_type == TOKEN_RBRACE) {
                        cur_ptr++;
                        return obj;
                    } else if(token_type == TOKEN_COMMA) {
                        warn_line = err_line;
                        warn_col = (int32_t)((cur_ptr - begin_ptr) - err_col);
                        cur_ptr++;
                    } else {
                        log_expecting_error(TOKEN_COMMA_FLAG | TOKEN_RBRACE_FLAG);
                        obj->free();
                        return nullptr;
                    }
                }
                return nullptr;
            }
            
            JsonType* parse_array() {
                cur_ptr++;
                int32_t token_type = check_next_token();
                JsonArray<ALLOC_TYPE>* obj = JsonArray<ALLOC_TYPE>::create();
                if(token_type == TOKEN_RBRACKET) {
                    cur_ptr++;
                    return obj;
                }
                int32_t warn_line = 0;
                int32_t warn_col = 0;
                while(true) {
                    JsonType* val = parse_value();
                    if(!val) {
                        token_type = check_next_token();
                        if(token_type == TOKEN_RBRACKET) {
                            std::cerr << "Warning: redundant \",\" detected at line " << warn_line << ", column " << warn_col << std::endl;
                            cur_ptr++;
                            return obj;
                        } else {
                            obj->free();
                            return nullptr;
                        }
                    }
                    obj->insert(val);
                    token_type = check_next_token();
                    if(token_type == TOKEN_RBRACKET) {
                        cur_ptr++;
                        return obj;
                    } else if(token_type == TOKEN_COMMA) {
                        warn_line = err_line;
                        warn_col = (int32_t)((cur_ptr - begin_ptr) - err_col);
                        cur_ptr++;
                    } else {
                        log_expecting_error(TOKEN_COMMA_FLAG | TOKEN_RBRACKET_FLAG);
                        obj->free();
                        return nullptr;
                    }
                }
                return nullptr;
            }
            
            const char* begin_ptr = nullptr;
            const char* end_ptr = nullptr;
            const char* cur_ptr = nullptr;
            int32_t err_line = 0;
            uintptr_t err_col = 0;
            std::string error_msg;
            JsonType* ret_value = nullptr;
        };
    };
    
    template<char INDENT_CHAR = ' ', int32_t INDENT_COUNT = 2, bool ESCAPE_SLASH = false>
    class JsonStreamWriter : public JsonWriter {
    public:
        JsonStreamWriter(std::ostream& stream) {
            streamptr = &stream;
        }
        virtual JsonWriter& operator << (const char* buf) {
            *streamptr << buf;
            return static_cast<JsonWriter&>(*this);
        }
        virtual JsonWriter& operator << (const char ch) {
            *streamptr << ch;
            return static_cast<JsonWriter&>(*this);
        }
        virtual JsonWriter& push_repeat(char ch, int32_t times) {
            for(int32_t i = 0; i < times; ++i)
                *streamptr << ch;
            return static_cast<JsonWriter&>(*this);
        }
        virtual JsonWriter& push_newline(int32_t indent_dif) {
            *streamptr << '\n';
            if(indent_dif > 0)
                indent += INDENT_COUNT;
            else if(indent_dif < 0 && indent > 0)
                indent -= INDENT_COUNT;
            push_repeat(INDENT_CHAR, indent);
            return static_cast<JsonWriter&>(*this);
        }
        virtual bool need_escape_slash() { return ESCAPE_SLASH; }
        
    protected:
        int32_t indent = 0;
        std::ostream* streamptr = nullptr;
    };
    
    template<char INDENT_CHAR = ' ', int32_t INDENT_COUNT = 2, bool ESCAPE_SLASH = false>
    class JsonStringWriter : public JsonWriter {
    public:
        JsonStringWriter(std::string& str) {
            stringptr = &str;
        }
        virtual JsonWriter& operator << (const char* buf) {
            stringptr->append(buf);
            return static_cast<JsonWriter&>(*this);
        }
        virtual JsonWriter& operator << (const char ch) {
            stringptr->append(1, ch);
            return static_cast<JsonWriter&>(*this);
        }
        virtual JsonWriter& push_repeat(char ch, int32_t times) {
            stringptr->append(times, ch);
            return static_cast<JsonWriter&>(*this);
        }
        virtual JsonWriter& push_newline(int32_t indent_dif) {
            stringptr->append(1, '\n');
            if(indent_dif > 0)
                indent += INDENT_COUNT;
            else if(indent_dif < 0 && indent > 0)
                indent -= INDENT_COUNT;
            stringptr->append(indent, INDENT_CHAR);
            return static_cast<JsonWriter&>(*this);
        }
        virtual bool need_escape_slash() { return ESCAPE_SLASH; }
        
    protected:
        int32_t indent = 0;
        std::string* stringptr = nullptr;
    };
    
    inline std::ostream& operator << (std::ostream& stream, JsonValue& node) {
        JsonStreamWriter<> writer(stream);
        node.write_to(writer);
        return stream;
    }
    
}

#endif
