#include "linker.h"

/**
 * @brief The Linker class 링커 @n
 * Object간 signal & slot 을 연결을 위한 링커 Object
 * @param QObject
 */
Linker::Linker(QObject *parent) : QObject(parent){
}

Linker::~Linker(){
}

