#ifndef SETTING_STRUCT_H
#define SETTING_STRUCT_H

#include <QJsonObject>
#include <QString>
#include <list>

namespace setting {
    /**
     * @brief 설정 내 프로필 편집 정보
     */
    struct ProfileInfo {
        QString email;              ///< 이메일 (아이디)
        QString username;           ///< 사용자 닉네임
        QString bio;                ///< 소개글
    };

}
#endif // SETTING_STRUCT_H
