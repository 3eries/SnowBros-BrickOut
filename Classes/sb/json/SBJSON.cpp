//
//  SBJSON.cpp
//
//  Created by sm on 15. 05. 13..
//
//

#include "SBJSON.h"

#include "json/stringbuffer.h"
#include "json/prettywriter.h"

#include "../utils/SBStringUtils.h"

USING_NS_CC;
using namespace std;

rapidjson::Document SBJSON::parse(const string &json) {
    
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
    if( doc.HasParseError() ) {
        CCLOG("SBJSON parse error.");
        // CCASSERT(false, "SBJSON parse error.");
    }
    
    return doc;
}

string SBJSON::toJSON(Value v) {
    
    rapidjson::Document doc;
    doc.CopyFrom(value(v, doc.GetAllocator()), doc.GetAllocator());
    
    rapidjson::StringBuffer strbuf;
    strbuf.Clear();

    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);

    return strbuf.GetString();
}

bool SBJSON::isJSON(const string &json) {
    
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
    return !doc.HasParseError();
}

rapidjson::Value SBJSON::value(Value v, rapidjson::Document::AllocatorType &allocator) {

    switch( v.getType() ) {
        case Value::Type::INTEGER:      return rapidjson::Value(v.asInt());
        case Value::Type::UNSIGNED:     return rapidjson::Value(v.asUnsignedInt());
        case Value::Type::FLOAT:        return rapidjson::Value(v.asFloat());
        case Value::Type::DOUBLE:       return rapidjson::Value(v.asDouble());
        case Value::Type::STRING:       return value(v.asString(), allocator);
        case Value::Type::BOOLEAN:      return rapidjson::Value(v.asBool());
            
        case Value::Type::VECTOR: {
            rapidjson::Value jsonValue(rapidjson::kArrayType);
            auto vector = v.asValueVector();
            
            for( int i = 0; i < vector.size(); ++i ) {
                jsonValue.PushBack(value(vector[i], allocator), allocator);
            }
            
            return jsonValue;

        } break;

        case Value::Type::MAP: {
            rapidjson::Value jsonValue(rapidjson::kObjectType);
            auto map = v.asValueMap();
            
            for( auto it = map.begin(); it != map.end(); ++it ) {
                jsonValue.AddMember(value(it->first, allocator), value(it->second, allocator), allocator);
            }
            
            return jsonValue;

        } break;

        default:
            CCASSERT(false, "SBJSON toJSONValue error: not supported type.");
            break;
    }
    
    return rapidjson::Value();
}

rapidjson::Value SBJSON::value(const string &v, rapidjson::Document::AllocatorType &allocator) {

    return rapidjson::Value(v.c_str(), allocator);
}

//Json::LargestUInt SBJSON::getLargestUInt(Value value) {
//    
//    if( value.getType() == Value::Type::STRING ) {
//        return SBStringUtils::toNumber<Json::LargestUInt>(value.asString());
//    } else {
//        return value.asInt();
//    }
//}
