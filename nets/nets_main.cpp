#include "../common/common.h"

#include "card_data_s.h"

int main(int argc, char** argv) {
    if(!ygopro::DataMgrS::Get().LoadDatas("./conf/cards.cdb"))
        return 0;
    return 0;
}
