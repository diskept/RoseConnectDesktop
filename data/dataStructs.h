#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include <QString>



/// 연관 컨텐츠, 재생목록(Queue)
struct DataTrack {

    /// audio의 PK
    int audio_id = 0;

    /// 노래 제목
    QString title = "";

    /// 가수명
    QString artist = "";

    /// 앨범이미지 경로
    QString album_art = "";
};

/// 로즈튜브 연관 컨텐츠
struct DataRosetubeTrack {

    /// 채널명
    QString channel = "";

    /// 업로드 경과일
    QString days = "";

    /// 조회수
    QString hit = "";

    /// 커버 이미지 경로
    QString img = "";

    /// 재생 시간
    QString time = "";

    /// 타이틀
    QString title = "";

    /// 재생 경로
    QString url = "";
};

#endif // DATASTRUCTS_H
