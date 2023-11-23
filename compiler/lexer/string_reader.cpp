//
// Created by gomil on 2023/11/21.
//

#include "include/string_reader.h"
#include "include/token.h"

Token StringReader::Forward() {
    currentToken = nextToken;
    nextToken = cursor.AdvanceToken();
    return nextToken;
}

