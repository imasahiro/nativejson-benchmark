#include "../test.h"
#define KJSON_HEADER_ONLY 1
#include "../../thirdparty/kjson/kjson/kjson.c"

static void GenStat(Stat* s, const JSON v) {
    JSON Key;
    JSON Val;
    JSONObject_iterator Itr;
    JSONArray *a;
    JSON *I, *E;
    switch (JSON_type(v)) {
    case JSON_Double:
        s->numberCount++;
        break;
    case JSON_String:
        s->stringCount++;
        s->stringLength += JSONString_length(v);
        break;
    case JSON_Int32:
        s->numberCount++;
        break;
    case JSON_Object:
        s->objectCount++;
        s->memberCount += JSON_length(v);
        s->stringCount += JSON_length(v);
        JSON_OBJECT_EACH(v, Itr, Key, Val) {
            s->stringLength += JSONString_length(Key);
            GenStat(s, Val);
        }
        break;
    case JSON_Bool:
        if (JSONBool_get(v)) {
            s->trueCount++;
        }
        else {
            s->falseCount++;
        }
        break;
    case JSON_Array:
        JSON_ARRAY_EACH(v, a, I, E) {
            GenStat(s, *I);
        }
        s->arrayCount++;
        s->elementCount += JSON_length(v);
        break;
    case JSON_Null:
        s->nullCount++;
        break;
    case JSON_UString:
        s->stringCount++;
        s->stringLength += JSONString_length(v);
        break;
    case JSON_Int64:
        s->numberCount++;
        break;
    default:
        break;
    }
}

class KJsonParseResult : public ParseResultBase {
public:
    JSONMemoryPool jm;
    JSON root;
    KJsonParseResult() {
        JSONMemoryPool_Init(&jm);
    }
    ~KJsonParseResult() {
        JSONMemoryPool_Delete(&jm);
    }
};

class KjsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return str; }

    char *str;
};

class KJsonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "kjson"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        KJsonParseResult* pr = new KJsonParseResult;
        pr->root = parseJSON(&pr->jm, json, json + length);
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const KJsonParseResult* pr = static_cast<const KJsonParseResult*>(parseResult);
        KjsonStringResult* sr = new KjsonStringResult;
        size_t len;
        sr->str = JSON_toStringWithLength(pr->root, &len);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const KJsonParseResult* pr = static_cast<const KJsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->root);
        return true;
    }
#endif
};

REGISTER_TEST(KJsonTest);
