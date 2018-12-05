//
//  RankingManager.cpp
//
//  Created by seongmin hwang on 2018. 6. 17..
//

#include "RankingManager.hpp"

#include "UserDefaultKey.h"

#include "json/document-wrapper.h"
#include "json/stringbuffer.h"
#include "json/prettywriter.h"

USING_NS_CC;
using namespace std;

static const vector<RankingRecord> DEFAULT_RECORDS({
    RankingRecord(1,  1004, "KSL"),
    RankingRecord(2,  999,  "AAA"),
    RankingRecord(3,  820,  "SRJ"),
    RankingRecord(4,  777,  "LUK"),
    RankingRecord(5,  611,  "GMA"),
    RankingRecord(6,  500,  "VTA"),
    RankingRecord(7,  430,  "3ES"),
    RankingRecord(8,  419,  "SOL"),
    RankingRecord(9,  309,  "KHK"),
    RankingRecord(10, 118,  "FRE"),
});

static RankingManager *instance = nullptr;
RankingManager* RankingManager::getInstance() {
    
    if( !instance ) {
        instance = new RankingManager();
    }
    
    return instance;
}

void RankingManager::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

RankingManager::RankingManager() {
}

RankingManager::~RankingManager() {
}

/**
 * 랭킹 초기화
 */
void RankingManager::init() {
    
    parse();
    
    CCLOG("%s", toString().c_str());
}

/**
 * json 파싱
 */
void RankingManager::parse() {
    
    records.clear();
    
    string json = UserDefault::getInstance()->getStringForKey(UserDefaultKey::RANKING, "");
    
    // 초기 값 설정
    if( json == "" ) {
        for( auto record : DEFAULT_RECORDS ) {
            records.push_back(record);
        }
        
        // 부족한 기록을 빈 값으로 채움
        size_t len = RANKING_COUNT - records.size();
        
        for( int i = 0; i < len; ++i ) {
            records.push_back(RankingRecord(records.size()+1, 0, ""));
        }
        
        save();
    }
    // json 파싱
    else {
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        auto list = doc.GetArray();
        
        for( int i = 0; i < list.Size(); ++i ) {
            const rapidjson::Value &recordValue = list[i];
            
            RankingRecord info;
            info.ranking = recordValue["ranking"].GetInt();
            info.score = recordValue["score"].GetInt();
            info.name = recordValue["name"].GetString();
            
            records.push_back(info);
        }
    }
    
    sort();
}

/**
 * json string 생성
 */
string RankingManager::toJson() {
    
    rapidjson::Document doc;
    doc.SetArray();
    
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    
    for( auto record : records ) {
        rapidjson::Value recordValue(rapidjson::kObjectType);
        recordValue.AddMember("ranking", record.ranking, allocator);
        recordValue.AddMember("score", record.score, allocator);
        recordValue.AddMember("name", rapidjson::Value(record.name.c_str(), allocator), allocator);
//        recordValue.AddMember("name", record.name, allocator);
//        recordValue.AddMember("position", (*iter)->Position().c_str(), allocator);
        
        doc.PushBack(recordValue, allocator);
//        myArray.PushBack(objValue, allocator);
        
    }
    
    rapidjson::StringBuffer strbuf;
    strbuf.Clear();
    
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);
    
    return strbuf.GetString();
}

/**
 * 기록 저장
 */
void RankingManager::save() {
    
    string json = instance->toJson();
    CCLOG("save: %s", instance->toJson().c_str());
    
    UserDefault::getInstance()->setStringForKey(UserDefaultKey::RANKING, json);
    UserDefault::getInstance()->flush();
}

/**
 * 랭킹 정렬
 */
void RankingManager::sort(RankingRecords &records) {
    
    // 스코어 기준 내림 차순
    std::sort(records.begin(), records.end(), [](RankingRecord r1, RankingRecord r2) {
        return r1.score > r2.score;
    });
    
    // 랭킹 설정
    for( int i = 0; i < records.size(); ++i ) {
        auto &record = records[i];
        record.ranking = i+1;
    }
}

void RankingManager::sort() {
 
    sort(instance->records);
}

/**
 * 베스트 스코어 설정
 */
bool RankingManager::setBestScore(int score) {
    
    if( score <= getBestScore() ) {
        return false;
    }
    
    UserDefault::getInstance()->setIntegerForKey(UserDefaultKey::BEST_SCORE, score);
    UserDefault::getInstance()->flush();
    
    return true;
}

/**
 * 베스트 스코어를 반환합니다
 */
int RankingManager::getBestScore() {
    
    return UserDefault::getInstance()->getIntegerForKey(UserDefaultKey::BEST_SCORE, 0);
}

/**
 * 기록 설정
 */
void RankingManager::setRecord(RankingRecord record, bool isSave) {
    
    CCASSERT(record.ranking != INVALID_RANKING, "RankingManager::setRecord ranking error.");
    CCASSERT(record.score > 0, "RankingManager::setRecord score error.");
    CCASSERT(record.name != "", "RankingManager::setRecord name error.");
    
    // 기록 추가
    auto &records = instance->records;
    int i = record.ranking-1;
    records.insert((records.begin() + i), record);
    
    // 밀린 기록 삭제
    records.erase((records.begin() + records.size()-1));
    
    sort();
    
    // 기록 저장
    if( isSave ) {
        save();
    }
}

/**
 * 신기록 설정
 */
void RankingManager::setNewRecord(int score, string name, bool isSave) {
 
    int ranking = getNewRanking(score);
    if( ranking == INVALID_RANKING ) {
        // 신기록 아님
        return;
    }
    
    // 신기록 추가
    RankingRecord record;
    record.ranking = ranking;
    record.score = score;
    record.name = name;
    
    setRecord(record, isSave);
}

/**
 * 신기록인지 체크
 */
bool RankingManager::isNewRecord(int score) {
    
    return getNewRanking(score) != INVALID_RANKING;
}

/**
 * 신기록 랭킹 반환
 * 신기록이 아닌 경우, INVALID_RANKING
 */
int RankingManager::getNewRanking(int score) {
    
    auto records = getRecords();
    
    for( auto record : records ) {
        if( score >= record.score ) {
            return record.ranking;
        }
    }
    
    return INVALID_RANKING;
}

/**
 * 모든 기록 반환
 */
RankingRecords RankingManager::getRecords() {
    
    return instance->records;
}

/**
 * 상위 기록 반환
 */
RankingRecords RankingManager::getTopRecords(int count) {
    
    CCASSERT(count <= instance->records.size(), "RankingManager::getTopRecords count error.");
    
    RankingRecords topRecords;
    
    for( int i = 0; i < count; ++i ) {
        topRecords.push_back(instance->records[i]);
    }
    
    return topRecords;
}

string RankingManager::toString() {
    
    string str = "RankingManager [\n";
    
    for( auto record : records ) {
        str += STR_FORMAT("ranking: %d score: %d name: %s\n", record.ranking, record.score, record.name.c_str());
    }
    
    str += "]";
    return str;
}

