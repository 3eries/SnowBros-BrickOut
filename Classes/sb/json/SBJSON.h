//
//  SBJSON.h
//
//  Created by sm on 15. 05. 13..
//
//

#ifndef __SBLib__SBJSON__
#define __SBLib__SBJSON__

#include <string>

#include "cocos2d.h"
#include "json/document-wrapper.h"

class SBJSON {
public:
    static rapidjson::Document parse(const std::string &json);
    static std::string         toJSON(cocos2d::Value v);
    
    static bool isJSON(const std::string &json);
    
/**
 * convert to json value
 */
public:
    static rapidjson::Value value(cocos2d::Value v,
                                  rapidjson::Document::AllocatorType &allocator);
    static rapidjson::Value value(const std::string &v,
                                  rapidjson::Document::AllocatorType &allocator);
};

#endif /* defined(__SBLib__SBJSON__) */
