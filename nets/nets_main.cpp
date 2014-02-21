#include <iostream>
#include <thread>
#include "card_data_s.h"
#include <chrono>

int main(int argc, char** argv) {
    if(!ygopro::dataMgrs.LoadDatas("../ygo/cards.cdb"))
        return 0;
    return 0;
}
