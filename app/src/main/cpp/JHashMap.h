#pragma once
//
// Created by d.beqiraj on 10/3/2017.
//

#include <map>
#include <string>
#include "jni.h"

class JHashMap {

public:
    void insert( std::string key, std::string value );
    jobject toJObject(JNIEnv *, jclass);

private:

    std::map< std::string, std::string > map;
};
