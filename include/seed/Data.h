//
//  Data.h
//  seed_base
//
//  Created by Yonemura Suguru on 11/10/25.
//  Copyright 2011年 sumstates.net. All rights reserved.
//

#ifndef seed_base_Data_h
#define seed_base_Data_h

#include <map>
#include <list>
#include <cstdint>

class Data{
    //typedef
    typedef std::map<int64_t,int64_t> Node;    
public:
    //setter
    /** dataMapに対してNodeを追加する */
    int setMap(int64_t key,Node& node);
    /** Nodeにメンバーをセットする。 */
    int setParams(Node &node,int64_t key,int64_t value);
    
    //getter
    /** dataMapからNodeを取得する */
    int getMap(int64_t key,Node& node);
    /** Nodeからメンバーをセットする。 */
    int getParams(Node &node,int64_t key,int64_t &value);
private:
    //key,nodeのmapをvalueに持つマップ
    std::map<int64_t,Node> dataMap;
};

#endif
