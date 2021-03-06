/*
 * @FilePath     : \xjzer\type.h
 * @Description  :
 * @Author       : Xjzer
 * @Date         : 2022-07-16 17:42:45
 * @Email        : xjzer2020@163.com
 * @Others       : empty
 * @LastEditTime : 2022-07-17 00:40:40
 */
#ifndef TYPE_H
#define TYPE_H
#include <QtGlobal>

typedef struct {
    struct {
        quint8 mVersion;
        quint8 mInverseVersion;
        quint16 mType;
        quint32 mLength;
    } mHeader;
    union {
        struct {
            quint16 mSource;
            quint8 mType;
            quint32 mISO;
            quint32 mOEM;
        } mRoutingReq;
        struct {
            quint16 mSource;
            quint16 mTarget;
        } mUdsReq;
    };
} DoIPProtocol;

#endif // TYPE_H
