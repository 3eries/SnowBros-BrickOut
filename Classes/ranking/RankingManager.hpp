//
//  RankingManager.hpp
//
//  Created by seongmin hwang on 2018. 6. 17..
//

#ifndef RankingManager_hpp
#define RankingManager_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

static const int INVALID_RANKING = -1;
static const int RANKING_COUNT   = 10;

struct RankingRecord {
    int ranking;
    int score;
    std::string name;
    
    RankingRecord(int _ranking, int _score, const std::string &_name) :
    ranking(_ranking), score(_score), name(_name) {}

    RankingRecord(int _score, const std::string &_name) :
    RankingRecord(INVALID_RANKING, _score, _name) {}
//    ranking(INVALID_RANKING), score(_score), name(_name) {}
    
    RankingRecord() : RankingRecord(INVALID_RANKING, 0, "") {}
};

typedef std::vector<RankingRecord> RankingRecords;

class RankingManager {
public:
    static RankingManager* getInstance();
    static void destroyInstance();
    ~RankingManager();
    
private:
    RankingManager();
    
    void        parse();
    std::string toJson();
    
    static void save();
    
public:
    void init();

    static void sort(RankingRecords &records);
    static void sort();
    
    static bool setBestScore(int score);
    static int  getBestScore();
    
    static void setRecord(RankingRecord record, bool isSave = true);
    static void setNewRecord(int score, std::string name, bool isSave = true);
    
    static bool isNewRecord(int score);
    static int  getNewRanking(int score);
    
    static RankingRecords getRecords();
    static RankingRecords getTopRecords(int count);
    
    std::string toString();
    
public:
    RankingRecords records; // 기록 목록, 랭킹 기준 내림 차순 (1...10)
};

#endif /* RankingManager_hpp */
