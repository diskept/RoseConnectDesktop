#ifndef GSCOMMON_H
#define GSCOMMON_H

#include <QFont>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QString>
#include <QMutex>

const int MAINWINDOW_SIZE_BOUNDARY = 1200;      ///< 메인윈도우 사이즈를 small모드 big모드 세팅 기준 width값


const QString SIGNAL_CATEGORY_ROSE = "ROSE";
const QString SIGNAL_CATEGORY_MUSIC = "MUSIC";
const QString SIGNAL_CATEGORY_TIDAL = "TIDAL";
const QString SIGNAL_CATEGORY_BUGS = "BUGS";
const QString SIGNAL_CATEGORY_QOBUZ = "QOBUZ";
const QString SIGNAL_CATEGORY_APPLE = "APPLE_MUSIC";
const QString SIGNAL_CATEGORY_SPOTIFY = "SPOTIFY";


// 공통 KEY
const QString KEY_MAIN_CODE = "mainMenuCode";   ///< 메인메뉴 페이지 코드의 Key (value : integer)
const QString KEY_PAGE_CODE = "pageCode";       ///< 서브메뉴 페이지 코드 의 key (value : QString)
const QString KEY_CONTENT_STEP = "contentStep"; ///< 서브메뉴 페이지의 Step 코드 의 key (value : QString)
const QString KEY_VIEWALL_MODE = "viewAllMode"; ///< view All 의 코드 의 key (value : QString)
const QString KEY_DATA = "data";
const QString KEY_PAGE_CURR = "currentPage";

// 옵션팝업에서 사용하는 공통 KEY
const QString KEY_OP_TITLE_MAIN = "op_mainTitle";   ///< 옵션팝업 공통필수 KEY : 메인 타이틀
const QString KEY_OP_TITLE_SUB = "op_subTitle";     ///< 옵션팝업 공통필수 KEY : 서브 타이틀
const QString KEY_OP_TYPE = "op_type";              ///< 옵션팝업 공통필수 KEY : FrameWidget의 Type값
const QString KEY_OP_albumImg = "op_albumImg";      ///< 옵션팝업 공통필수 KEY : 썸네일 경로
const QString KEY_OP_MIME = "op_mime";
const QString KEY_OP_ARTIST = "op_artist";
const QString KEY_OP_cntStar = "op_cntStar";        ///< 옵션팝업 공통필수 KEY : 0이하로 세팅하면 옵션팝업에서 별정보를 GET 한다.
const QString KEY_OP_DATA = "op_data";              ///< 옵션팝업 공통필수 KEY : array 음원 정보
const QString KEY_OP_DATA_TOTAL = "op_data_total";  ///< 옵션팝업 공통필수 KEY : array 음원 정보 앨범 트랙 ALL
const QString KEY_OP_TRACK_INDEX = "op_track_index";
const QString KEY_OP_PLAYLISTNO = "playListNo";     ///< 옵션팝업 선택 KEY : 플레이리스트앨범의 No (PK)
const QString KEY_OP_GENRENO = "genreNo";           ///< 옵션팝업 선택 KEY : 장르의 No (PK)
const QString KEY_OP_ROSETUBE_PLAYURL = "playurl";  ///< for 로즈튜브 옵션팝업 별정보 GET 할때 필요한 값
const QString KEY_OP_MUSIC_PLAYLIST_ADD_DATA = "music_plylist_add"; ///< for 음악 옵션팝업 플레이리스트 담기에 필요한 값



// Page Code 상수
//------------------------------------------------------------------------------------------
// MARK : 로즈홈
//------------------------------------------------------------------------------------------
const QString PAGECODE_RH_HOME = "RH_HOME";                         ///< 로즈홈 > 홈
const QString PAGECODE_RH_FAVORITE = "RH_FAVORITE";
const QString PAGECODE_RH_USERPICK = "RH_USERPICK";
const QString PAGECODE_RH_FRIEND = "RH_FRIEND";
const QString PAGECODE_RH_USERPAGE = "RH_USERPAGE";//c0124

const QString PAGECODE_RH_ALBUM_LIST_ALL_VIEW = "RH_ALBUM_LIST_LIST_ALL_VIEW";
const QString PAGECODE_RH_PLAYLIST_LIST_ALL_VIEW = "RH_PLAYLIST_LIST_ALL_VIEW";
const QString PAGECODE_RH_ROSETUBE_LIST_ALL_VIEW = "RH_ROSETUBE_LIST_ALL_VIEW";
const QString PAGECODE_RH_TRACK_LIST_ALL_VIEW = "RH_TRACK_LIST_ALL_VIEW";
const QString PAGECODE_RH_TRACK_SHARE_LIST_ALL_VIEW = "RH_SHARE_TRACK_LIST_ALL_VIEW";
const QString PAGECODE_RH_ALBUM_DETAIL = "RH_ALBUM_DETAIL";
const QString PAGECODE_RH_PLAYLIST_DETAIL = "RH_PLAYLIST_DETAIL";
const QString PAGECODE_RH_PLAYLIST_ROSE_DETAIL = "RH_PLAYLIST_ROSE_DETAIL";
const QString PAGECODE_RH_ARTIST_DETAIL = "RH_ARTIST_DETAIL";
const QString PAGECODE_RH_ADD_PLAYLIST = "RH_ADD_PLAYLIST";



//------------------------------------------------------------------------------------------
// MARK : 음악 2 (to test by sunnyfish)
//------------------------------------------------------------------------------------------
const QString PAGECODE_M2_HOME = "M2_HOME";                         ///< 음악 > 홈
const QString PAGECODE_M2_ALBUM_ALL_LIST = "M2_ALBUM_ALL_LIST";     ///< 음악 > 앨범 목록



//------------------------------------------------------------------------------------------
// MARK : 음악
//------------------------------------------------------------------------------------------
const QString PAGECODE_M_HOME = "home";                                 ///< 음악 > 홈
const QString PAGECODE_M_CATEGORY = "sort";                             ///< 음악 > 분류
const QString PAGECODE_M_ALBUM = "album";                               ///< 음악 > 앨범
const QString PAGECODE_M_FOLDER = "folder";                             ///< 음악 > 폴더
const QString PAGECODE_M_COLLECTION = "my";                             ///< 음악 > My Collection
const QString PAGECODE_M_SEARCH = "music_search";                       ///< 음악 > 검색
const QString PAGECODE_M_FAVORITE = "favorite";                         ///< 음악 > My Collection > Favorite

const QString PAGECODE_VA_PLAYALBUM = "viewAll_playAlbum";              ///< 음악 > 홈 > View All > 최근 재생 앨범
const QString PAGECODE_VA_PLAYALBUMALL = "viewAll_playAlbumAll";        ///< 음악 > 홈 > View All > 최근 재생 앨범
const QString PAGECODE_VA_PLAYPLAYLIST = "viewAll_playPlaylist";        ///< 음악 > 홈 > View All > 최근 재생 플레이리스트
const QString PAGECODE_VA_PLAYPLAYLISTALL = "viewAll_playPlaylistAll";  ///< 음악 > 홈 > View All > 최근 재생 플레이리스트
const QString PAGECODE_VA_PLAYTRACKALL = "viewAll_playTrackAll";        ///< 음악 > 홈 > View All > 최근 재생 트랙
const QString PAGECODE_VA_ADDEDALBUM = "viewAll_addedAlbum";            ///< 음악 > 홈 > View All > 최근 추가 된 앨범
const QString PAGECODE_VA_RECOMMANDARTIST = "viewAll_recommandArtist";  ///< 음악 > 홈 > View All > 추천 아티스트
const QString PAGECODE_VA_MYPLAYLIST = "viewAll_myPlayList";            ///< 음악 > 홈 > View All > My 플레이리스트
const QString PAGECODE_VA_USERPLAYLIST = "viewAll_userPlayList";        ///< 음악 > 홈 > View All > My 플레이리스트
const QString PAGECODE_VA_FRIENDPLAYLIST = "viewAll_friendPlayList";    ///< 음악 > 홈 > View All > 친구 플레이리스트
const QString PAGECODE_VA_RECOMMENDARTIST = "viewAll_recommendArtist";  ///< 음악 > 홈 > View All > 추천 아티스트
const QString PAGECODE_VA_COLLECTION_TRACK = "viewAll_collectionTrack"; ///< 음악 > 홈 > View All > 마이컬렉션 트랙
const QString PAGECODE_VA_SEARCH_TRACK = "viewAll_searchTrack";         ///< 음악 > 검색 > View All > 트랙
const QString PAGECODE_VA_SEARCH_ALBUM = "viewAll_searchAlbum";         ///< 음악 > 검색 > View All > 앨범
const QString PAGECODE_VA_SEARCH_ARTIST = "viewAll_searchArtist";       ///< 음악 > 검색 > View All > 아티스트
const QString PAGECODE_VA_ARTIST_TRACKLIST = "viewAll_artistTrackList"; ///< 음악 > 아티스트 > view All > 트랙
const QString PAGECODE_VA_ARTIST_ALBUMLIST = "viewAll_artsitAlbumList"; ///< 음악 > 아티스트 > view All > 앨범
const QString PAGECODE_VA_GENRE_ALBUMLIST = "viewAll_genreAlbumList";   ///< 음악 > 장르 > view All > 앨범


const QString PAGECODE_ETC_EDIT_FAVORITE = "edit_favorite";             ///< 음악 > 폴더 > 즐겨찾기 편집
const QString PAGECODE_ETC_PLAYFULLSCREEN = "playfullscreen";           ///< 음악 > 폴더 > 재생화면



//------------------------------------------------------------------------------------------
// MARK : 비디오
//------------------------------------------------------------------------------------------
const QString PAGECODE_V_HOME = "home";             ///< 비디오 > 홈
const QString PAGECODE_V_FOLDER = "folder";         ///< 비디오 > 폴더
const QString PAGECODE_V_SEARCH = "search";         ///< 비디오 > 검색



//------------------------------------------------------------------------------------------
// MARK : 라디오
//------------------------------------------------------------------------------------------
const QString PAGECODE_R_FAVORITE ="favorite";      ///< 라디오 > 즐겨찾기
const QString PAGECODE_R_COUNTRY = "country";       ///< 라디오 > KOREA
const QString PAGECODE_R_USER = "user";             ///< 라디오 > 사용자채널
const QString PAGECODE_R_EDIT = "edit";             ///< 라디오 > 사용자채널 편집
const QString PAGECODE_R_SEARCH = "search";         ///< 라디오 > 검색



//------------------------------------------------------------------------------------------
// MARK : 라디오
//------------------------------------------------------------------------------------------
const QString PAGECODE_RR_HOME ="RR_HOME";                          ///< 로즈라디오 > 홈
const QString PAGECODE_RR_EXPLORE = "RR_EXPLORE";                   ///< 로즈라디오 > 탐색
const QString PAGECODE_RR_MYRADIO = "RR_MYRADIO";                   ///< 로즈라디오 > 나의라디오
const QString PAGECODE_RR_RESERVATION = "RR_RESERVATION";           ///< 로즈라디오 > 방송 예약
const QString PAGECODE_RR_SETTINGS = "RR_SETTINGS";                 ///< 로즈라디오 > 설정

const QString PAGECODE_RR_LISTVIEWALL = "RR_LISTVIEWALL";           ///< 로즈라디오 > List View All
const QString PAGECODE_RR_GENREVIEWALL = "RR_GENREVIEWALL";         ///< 로즈라디오 > Genre View All
const QString PAGECODE_RR_COUNTRYVIEWALL = "RR_COUNTRYVIEWALL";     ///< 로즈라디오 > Country View All
const QString PAGECODE_RR_REGIONVIEWALL = "RR_REGIONVIEWALL";       ///< 로즈라디오 > Region View All
const QString PAGECODE_RR_LANGUAGEVIEWALL = "RR_LANGUAGEVIEWALL";   ///< 로즈라디오 > Language View All
const QString PAGECODE_RR_COUNTRYDETAIL = "RR_COUNTRYDETAIL";       ///< 로즈라디오 > Country Detail
const QString PAGECODE_RR_LANGUAGEDETAIL = "RR_LANGUAGEDETAIL";     ///< 로즈라디오 > Language Detail

const QString PAGECODE_RR_SEARCHMAIN = "RR_SEARCHMAIN";             ///< 로즈라디오 > Search Main



//------------------------------------------------------------------------------------------
// MARK : 비디오
//------------------------------------------------------------------------------------------
const QString PAGECODE_RF_HOME = "home";             ///< RoseFM > 홈



//------------------------------------------------------------------------------------------
// MARK : 로즈튜브
//------------------------------------------------------------------------------------------
const QString PAGECODE_RT_HOME = "home";                                            ///< 로즈튜브 > 홈
const QString PAGECODE_RT_EXPLORE = "explore";                                      ///< 로즈튜브 > 탐색
const QString PAGECODE_RT_SUB = "sub";                                              ///< 로즈튜브 > 구독
const QString PAGECODE_RT_COLLECTION = "myCollection";                              ///< 로즈튜브 > 마이컬렉션
const QString PAGECODE_RT_SEARCH = "search";                                        ///< 로즈튜브 > 검색

const QString PAGECODE_VA_HOME = "viewAll_home_playList";                           ///< 로즈튜브 > 홈 > View All 공용
const QString PAGECODE_VA_HOME_ROSE = "viewAll_home_rosePlayiist";                  ///< 로즈튜브 > 홈 > Rose SVR View All
const QString PAGECODE_VA_HOME_MYROSE = "viewAll_home_myrosePlaylist";              ///< 로즈튜브 > 홈 > Rose SVR View All
const QString PAGECODE_VA_HOME_USERROSE = "viewAll_home_userrosePlaylist";          ///< 로즈튜브 > 홈 > Rose SVR View All
const QString PAGECODE_VA_HOME_MYDETAIL = "viewAll_home_rosePlaylistDetail";        ///< 로즈튜브 > 홈 > Rose SVR View All

const QString PAGECODE_VA_EXPLORE_CATEGORY = "viewAll_explore_category";            ///< 로즈튜브 > 탐색 > View All > 장르별
const QString PAGECODE_VA_EXPLORE_PLAYLIST = "viewAll_explore_playList";            ///< 로즈튜브 > 탐색 > View All > 상황별
const QString PAGECODE_VA_CATEGORY_DETAILS = "viewAll_category_detials";            ///< 로즈튜브 > 탐색 > View All > 장르별 > Detail

const QString PAGECODE_VA_COLLECTION_FAV = "viewAll_collectionFavorite";            ///< 로즈튜브 > 마이컬렉션 > Rose SVR View All
const QString PAGECODE_VA_COLLECTION_CACHE = "viewAll_collectionCache";             ///< 로즈튜브 > 마이컬렉션 > Rose CACHE View All


const QString PAGECODE_VA_ROSE_PLAY_LIST = "viewAll_rosetube_playList";             ///< 로즈튜브 > MY Playlist List All
const QString PAGECODE_VA_TRACK_SHARE_LIST = "viewAll_roetube_share";

const QString PAGECODE_SUB_CHANNEL = "sub_channel";                                 ///< 로즈튜브 > 구독 > 구독채널정보


//------------------------------------------------------------------------------------------
// MARK : 팟캐스트
//------------------------------------------------------------------------------------------
const QString PAGECODE_PC_HOME = "PC_HOME";                             ///< 팟캐스트 > 홈
const QString PAGECODE_PC_CATEGORY = "PC_CATEGORY";                     ///< 팟캐스트 > 카테고리
const QString PAGECODE_PC_SUBSCRIBE = "PC_SUBSCRIBE";                   ///< 팟캐스트 > 구독
const QString PAGECODE_PC_CATEGORY_DETAIL = "PC_CATEGORY_DETAIL";       ///< 팟캐스트 > 카테고리 > 채널목록
const QString PAGECODE_PC_SUB_CHANNEL = "PC_SUB_CHANNEL";               ///< 팟캐스트 > 구독 > 채널상세
const QString PAGECODE_PC_SEARCH = "PC_SEARCH";                         ///< 팟캐스트 > 검색



//------------------------------------------------------------------------------------------
// MARK : CD 플레이
//------------------------------------------------------------------------------------------
const QString PAGECODE_CD_HOME = "CD_HOME";                             ///< 로즈라디오 > 홈



//------------------------------------------------------------------------------------------
// MARK : TIDAL
//------------------------------------------------------------------------------------------
// TIDAL > 각 서브 페이지
const QString PAGECODE_T_HOME = "T_HOME";                                                           ///< TIDAL > 홈
const QString PAGECODE_T_EXPLORE = "T_EXPLORE";                                                     ///< TIDAL > 탐색
const QString PAGECODE_T_VIDEO = "T_VIDEO";
const QString PAGECODE_T_MASTER = "T_MASTER";
const QString PAGECODE_T_SETTINGS = "T_SETTINGS";

const QString PAGECODE_T_EXPLORE_GENRES_CHOOSE_ALL = "T_EXPLORE_GENRES_ALL";                        ///< TIDAL > 탐색 > Genre View as List
const QString PAGECODE_T_EXPLORE_GENRES_PLAYLIST_VIEWALL = "T_EXPLORE_GENRES_PLAYLIST_VIEWALL";     ///< TIDAL > 탐색 > Genre 선택한 메인 > Playlist 전체
const QString PAGECODE_T_EXPLORE_GENRES_MAIN = "T_EXPLORE_GENRES_MAIN";                             ///< TIDAL > 탐색 > Genre 선택한 메인
const QString PAGECODE_T_EXPLORE_MOODS_MAIN = "T_EXPLORE_MOODS_MAIN";                               ///< TIDAL > 탐색 > Moods 선택한 메인
const QString PAGECODE_T_EXPLORE_NEW_MAIN = "T_EXPLORE_NEW_MAIN";                                   ///< TIDAL > 탐색 > New 메인
const QString PAGECODE_T_EXPLORE_TOP_MAIN = "T_EXPLORE_TOP_MAIN";                                   ///< TIDAL > 탐색 > Top 메인
const QString PAGECODE_T_EXPLORE_RISING_MAIN = "T_EXPLORE_RISING_MAIN";                             ///< TIDAL > 탐색 > Tidal Rising 메인

// TIDAL 공용사용
const QString PAGECODE_T_PLAYLIST_ALL_LIST_VIEW = "T_PLAYLIST_ALL_LIST_VIEW";
const QString PAGECODE_T_PLAYLIST_DETAIL = "T_PLAYLIST_DETAIL";
const QString PAGECODE_T_MIXES_DETAIL = "T_MIXES_DETAIL";
const QString PAGECODE_T_ALBUM_ALL_LIST_VIEW = "T_ALBUM_ALL_LIST_VIEW";
const QString PAGECODE_T_ALBUM_DETAIL = "T_ALBUM_DETAIL";
const QString PAGECODE_T_TRACK_ALL_LIST_VIEW = "T_TRACK_ALL_LIST_VIEW";
const QString PAGECODE_T_TRACK_SHARE_ALL_LIST_VIEW = "T_TRACK_SHARE_ALL_LIST_VIEW";
const QString PAGECODE_T_ARTIST_ALL_LIST_VIEW = "T_ARTIST_ALL_LIST_VIEW";
const QString PAGECODE_T_ARTIST_DETAIL = "T_ARTIST_DETAIL";
const QString PAGECODE_T_VIDEO_ALL_LIST_VIEW = "T_VIDEO_ALL_LIST_VIEW";
const QString PAGECODE_T_ADD_PLAYLIST = "T_ADD_PLAYLIST";

// TIDAL MyCollection Part
const QString PAGECODE_T_MY_COLLECTION = "T_MY_COLLECTION";
const QString PAGECODE_T_MY_COLLECTION_TIDALPALYLIST_ALL_LIST = "T_MY_COLLECTION_PLAYLSIT_ALL_TIDAL";
const QString PAGECODE_T_MY_COLLECTION_PLAYLIST_ALL_LIST = "T_MY_COLLECTION_PLAYLIST_ALL_LIST";
const QString PAGECODE_T_MY_COLLECTION_PLAYLIST_DETAIL = "T_MY_COLLECTION_PLAYLIST_DETAIL";
const QString PAGECODE_T_MY_COLLECTION_PLAYLIST_EDIT = "T_MY_COLLECTION_PLAYLIST_EDIT";
const QString PAGECODE_T_MY_COLLECTION_ALBUM_ALL_LIST = "T_MY_COLLECTION_ALBUM_ALL_LIST";
const QString PAGECODE_T_MY_COLLECTION_TRACK_ALL_LIST = "T_MY_COLLECTION_TRACK_ALL_LIST";
const QString PAGECODE_T_MY_COLLECTION_ARTIST_ALL_LIST = "T_MY_COLLECTION_ARTIST_ALL_LIST";
const QString PAGECODE_T_MY_COLLECTION_VIDEO_ALL_LIST = "T_MY_COLLECTION_VIDEO_ALL_LIST";

const QString PAGECODE_T_MY_RECENTLY_ALBUM_ALL_LIST = "T_MY_RECENTLY_ALBUM_ALL_LIST";
const QString PAGECODE_T_MY_RECENTLY_PLAYLIST_ALL_LIST = "T_MY_RECENTLY_PLAYLIST_ALL_LIST";
const QString PAGECODE_T_MY_RECENTLY_TRACK_ALL_LIST = "T_MY_RECENTLY_TRACK_ALL_LIST";
const QString PAGECODE_T_MY_ROSE_PLAYLIST_DETAIL = "T_MY_ROSE_PLAYLIST_DETAIL";
const QString PAGECODE_T_MY_ROSE_PLAYLIST_ALL_LIST = "T_MY_ROSE_PLAYLIST_ALL_LIST";
const QString PAGECODE_T_USER_ROSE_PLAYLIST_ALL_LIST = "T_USER_ROSE_PLAYLIST_ALL_LIST";

// TIDAL Search
const QString PAGECODE_T_SEARCH_MAIN = "T_SEARCH_MAIN";
const QString PAGECODE_T_SEARCH_TRACK_ALL_VIEW = "T_SEARCH_TRACK_ALL_VIEW";
const QString PAGECODE_T_SEARCH_ARTIST_ALL_VIEW = "T_SEARCH_ARTIST_ALL_VIEW";
const QString PAGECODE_T_SEARCH_ALBUM_ALL_VIEW = "T_SEARCH_ALBUM_ALL_VIEW";
const QString PAGECODE_T_SEARCH_PLAYLIST_ALL_VIEW = "T_SEARCH_PLAYLIST_ALL_VIEW";
const QString PAGECODE_T_SEARCH_VIDEO_ALL_VIEW = "T_SEARCH_VIDEO_ALL_VIEW";



//------------------------------------------------------------------------------------------
// MARK : Bugs
//------------------------------------------------------------------------------------------
// Bugs 각 서브 페이지
const QString PAGECODE_BUGS___HOME = "BUGS___HOME";                                                 ///< 벅스 > 홈
const QString PAGECODE_BUGS___EXPLORE = "BUGS___EXPLORE";                                           ///< 벅스 > 탐색
const QString PAGECODE_BUGS___EXPLORE___CHOOSE_THEME = "BUGS___EXPLORE___CHOOSE_THEME";             ///< 벅스 > 탐색 > 장르/테마 선택
const QString PAGECODE_BUGS___MY_COLLECTION = "BUGS___MY_COLLECTION";   ///< 벅스 > My Collection
const QString PAGECODE_BUGS___MY_COLLECTION___LIKE_MUSIC = "BUGS___MY_COLLECTION___LIKE_MUSIC";     ///< 벅스 > My Collection > 좋아한 음악
const QString PAGECODE_BUGS___SETTING = "BUGS___SETTING";                ///< 벅스 > 설정

const QString PAGECODE_BUGS___HOME_MUSIC_RECENTLY = "BUGS___HOME_MUSIC_RECENTLY";                   ///< 벅스 > 홈 > 최신음악
const QString PAGECODE_BUGS___HOME_MUSIC_CHART = "BUGS___HOME_MUSIC_CHART";                         ///< 벅스 > 홈 > 벅스차트
const QString PAGECODE_BUGS___HOME_TRACK_RECENTLY = "BUGS___HOME_TRACK_RECENTLY";
const QString PAGECODE_BUGS___HOME_ALBUM_RECENTLY = "BUGS___HOME_ALBUM_RECENTLY";
const QString PAGECODE_BUGS___HOME_VIDEO_RECENTLY = "BUGS___HOME_VIDEO_RECENTLY";
const QString PAGECODE_BUGS___HOME_TRACK_CHART = "BUGS___HOME_TRACK_CHART";
const QString PAGECODE_BUGS___HOME_ALBUM_CHART = "BUGS___HOME_ALBUM_CHART";
const QString PAGECODE_BUGS___HOME_VIDEO_CHART = "BUGS___HOME_VIDEO_CHART";

// Bugs 공용사용
const QString PAGECODE_BUGS___ALBUM_DETAIL = "BUGS___ALBUM_DETAIL";
const QString PAGECODE_BUGS___ARTIST_DETAIL = "BUGS___ARTIST_DETAIL";
const QString PAGECODE_BUGS___PD_ALBUM_DETAIL = "BUGS___PD_ALBUM_DETAIL";
const QString PAGECODE_BUGS___MY_ALBUM_DETAIL = "BUGS___MY_ALBUM_DETAIL";

const QString PAGECODE_BUGS___TRACK_ALL_VIEW = "BUGS___TRACK_ALL_VIEW";
const QString PAGECODE_BUGS___TRACK_SHARE_ALL_VIEW = "BUGS___TRACK_SHARE_ALL_VIEW";
const QString PAGECODE_BUGS___ALBUM_ALL_VIEW___OF_ARTIST = "BUGS___ALBUM_ALL_VIEW___OF_ARTIST";
const QString PAGECODE_BUGS___TRACK_ALL_VIEW___OF_ARTIST = "BUGS___TRACK_ALL_VIEW___OF_ARTIST";
const QString PAGECODE_BUGS___PD_ALBUM_ALL_VIEW = "BUGS___PD_ALBUM_ALL_VIEW";
const QString PAGECODE_BUGS___VIDEO_ALL_VIEW___OF_TRACK = "BUGS___VIDEO_ALL_VIEW___OF_TRACK";

const QString PAGECODE_BUGS___SEARCH_MAIN = "BUGS___SEARCH_MAIN";
const QString PAGECODE_BUGS___SEARCH_TRACK_ALL_VIEW = "BUGS___SEARCH_TRACK_ALL_VIEW";
const QString PAGECODE_BUGS___SEARCH_ARTIST_ALL_VIEW = "BUGS___SEARCH_ARTIST_ALL_VIEW";
const QString PAGECODE_BUGS___SEARCH_ALBUM_ALL_VIEW = "BUGS___SEARCH_ALBUM_ALL_VIEW";
const QString PAGECODE_BUGS___SEARCH_PD_ALBUM_ALL_VIEW = "BUGS___SEARCH_PD_ALBUM_ALL_VIEW";
const QString PAGECODE_BUGS___SEARCH_VIDEO_ALL_VIEW = "BUGS___SEARCH_VIDEO_ALL_VIEW";

const QString PAGECODE_BUGS__MY_RECENTLY_ALBUM_ALL_LIST = "BUGS__MY_RECENTLY_ALBUM_ALL_LIST";
const QString PAGECODE_BUGS__MY_RECENTLY_PLAYLIST_ALL_LIST = "BUGS__MY_RECENTLY_PLAYLIST_ALL_LIST";
const QString PAGECODE_BUGS__MY_RECENTLY_TRACK_ALL_LIST = "BUGS__MY_RECENTLY_TRACK_ALL_LIST";
const QString PAGECODE_BUGS__MY_ROSE_PLAYLIST_DETAIL = "BUGS__MY_ROSE_PLAYLIST_DETAIL";
const QString PAGECODE_BUGS__MY_ROSE_PLAYLIST_ALL_LIST = "BUGS__MY_ROSE_PLAYLIST_ALL_LIST";
const QString PAGECODE_BUGS__USER_ROSE_PLAYLIST_ALL_LIST = "BUGS__USER_ROSE_PLAYLIST_ALL_LIST";



//------------------------------------------------------------------------------------------
// MARK : QOBUZ
//------------------------------------------------------------------------------------------
// QOBUZ > 각 서브 페이지
const QString PAGECODE_Q_HOME = "Q_HOME";                                                           ///< QOBUZ > 홈
const QString PAGECODE_Q_EXPLORE = "Q_EXPLORE";                                                     ///< QOBUZ > 탐색
const QString PAGECODE_Q_VIDEO = "Q_VIDEO";
const QString PAGECODE_Q_MASTER = "Q_MASTER";
const QString PAGECODE_Q_SETTINGS = "Q_SETTINGS";

const QString PAGECODE_Q_EXPLORE_GENRES_CHOOSE_ALL = "Q_EXPLORE_GENRES_ALL";                        ///< QOBUZ > 탐색 > Genre View as List
const QString PAGECODE_Q_EXPLORE_GENRES_PLAYLIST_VIEWALL = "Q_EXPLORE_GENRES_PLAYLIST_VIEWALL";     ///< QOBUZ > 탐색 > Genre 선택한 메인 > Playlist 전체
const QString PAGECODE_Q_EXPLORE_GENRES_MAIN = "Q_EXPLORE_GENRES_MAIN";                             ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_POP = "Q_EXPLORE_GENRES_POP";                               ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_JAZZ = "Q_EXPLORE_GENRES_JAZZ";                             ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_CLASSIC = "Q_EXPLORE_GENRES_CLASSIC";                       ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_ELECTRO = "Q_EXPLORE_GENRES_ELECTRO";                       ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_SOUL = "Q_EXPLORE_GENRES_SOUL";                             ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_FOLK = "Q_EXPLORE_GENRES_FOLK";                             ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_HIPHOP = "Q_EXPLORE_GENRES_HIPHOP";                         ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_COUNTRY = "Q_EXPLORE_GENRES_COUNTRY";                       ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_METAL = "Q_EXPLORE_GENRES_METAL";                           ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_BLUES = "Q_EXPLORE_GENRES_BLUES";                           ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_LATIN = "Q_EXPLORE_GENRES_LATIN";                           ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_SOUNDTRACKS = "Q_EXPLORE_GENRES_SOUNDTRACKS";               ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_WORLD = "Q_EXPLORE_GENRES_WORLD";                           ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_GENRES_OTHER = "Q_EXPLORE_GENRES_OTHER";                           ///< QOBUZ > 탐색 > Genre 선택한 메인
const QString PAGECODE_Q_EXPLORE_MOODS_MAIN = "Q_EXPLORE_MOODS_MAIN";                               ///< QOBUZ > 탐색 > Moods 선택한 메인
const QString PAGECODE_Q_EXPLORE_NEW_MAIN = "Q_EXPLORE_NEW_MAIN";                                   ///< QOBUZ > 탐색 > New 메인
const QString PAGECODE_Q_EXPLORE_TOP_MAIN = "Q_EXPLORE_TOP_MAIN";                                   ///< QOBUZ > 탐색 > Top 메인
const QString PAGECODE_Q_EXPLORE_RISING_MAIN = "Q_EXPLORE_RISING_MAIN";                             ///< QOBUZ > 탐색 > Tidal Rising 메인

// QOBUZ 공용사용
const QString PAGECODE_Q_ALBUM_DETAIL = "Q_ALBUM_DETAIL";
const QString PAGECODE_Q_PLAYLIST_DETAIL = "Q_PLAYLIST_DETAIL";
const QString PAGECODE_Q_ARTIST_DETAIL = "Q_ARTIST_DETAIL";

const QString PAGECODE_Q_ALBUM_ALL_LIST_VIEW = "Q_ALBUM_ALL_LIST_VIEW";
const QString PAGECODE_Q_PLAYLIST_ALL_LIST_VIEW = "Q_PLAYLIST_ALL_LIST_VIEW";
const QString PAGECODE_Q_ARTIST_ALL_LIST_VIEW = "Q_ARTIST_ALL_LIST_VIEW";
const QString PAGECODE_Q_TRACK_ALL_LIST_VIEW = "Q_TRACK_ALL_LIST_VIEW";
const QString PAGECODE_Q_TRACK_SHARE_ALL_LIST_VIEW = "Q_TRACK_SHARE_ALL_LIST_VIEW";

const QString PAGECODE_Q_SIMILAR_PLAYLIST_VIEW = "Q_SIMILAR_PLAYLIST_VIEW";
const QString PAGECODE_Q_CLASSIC_ALL_LIST_VIEW = "Q_CLASSIC_ALL_LIST_VIEW";
const QString PAGECODE_Q_IDEAL_DISCOGRAPHY_ALL_LIST_VIEW = "Q_IDEAL_DISCOGRAPHY_ALL_LIST_VIEW";
const QString PAGECODE_Q_STILL_TRENDING_ALL_LIST_VIEW = "Q_STILL_TRENDING_ALL_LIST_VIEW";
const QString PAGECODE_Q_PRESS_AWARDS_ALL_LIST_VIEW = "Q_PRESS_AWARDS_ALL_LIST_VIEW";
const QString PAGECODE_Q_QOBUZ_ISSIME_ALL_LIST_VIEW = "Q_QOBUZ_ISSIME_ALL_LIST_VIEW";

const QString PAGECODE_Q_TOPRELEASE_ALL_LIST_VIEW = "Q_TOP_RELEASE_ALL_LIST_VIEW";

const QString PAGECODE_Q_APPEARSON_ALL_LIST_VIEW = "Q_APPEARSON_ALL_LIST_VIEW";
const QString PAGECODE_Q_LAST_RELEASE_LIST_VIEW = "Q_LAST_RELEASE_LIST_VIEW";
const QString PAGECODE_Q_SAME_ARTIST_LIST_VIEW = "Q_SAME_ARTIST_LIST_VIEW";

const QString PAGECODE_Q_VIDEO_ALL_LIST_VIEW = "Q_VIDEO_ALL_LIST_VIEW";
const QString PAGECODE_Q_ADD_PLAYLIST = "Q_ADD_PLAYLIST";

// QOBUZ MyCollection Part
const QString PAGECODE_Q_MY_COLLECTION = "Q_MY_COLLECTION";
const QString PAGECODE_Q_MY_COLLECTION_FAVORITES = "Q_MY_COLLECTION_FAVORITES";
const QString PAGECODE_Q_MY_COLLECTION_PURCHASES = "Q_MY_COLLECTION_PURCHASES";
const QString PAGECODE_Q_MY_COLLECTION_PLAYLIST_ALL_LIST = "Q_MY_COLLECTION_PLAYLIST_ALL_LIST";
const QString PAGECODE_Q_MY_COLLECTION_PLAYLIST_DETAIL = "Q_MY_COLLECTION_PLAYLIST_DETAIL";
const QString PAGECODE_Q_MY_COLLECTION_PLAYLIST_EDIT = "Q_MY_COLLECTION_PLAYLIST_EDIT";
const QString PAGECODE_Q_MY_COLLECTION_ALBUM_ALL_LIST = "Q_MY_COLLECTION_ALBUM_ALL_LIST";
const QString PAGECODE_Q_MY_COLLECTION_ARTIST_ALL_LIST = "Q_MY_COLLECTION_ARTIST_ALL_LIST";
const QString PAGECODE_Q_MY_COLLECTION_TRACK_ALL_LIST = "Q_MY_COLLECTION_TRACK_ALL_LIST";
const QString PAGECODE_Q_MY_COLLECTION_VIDEO_ALL_LIST = "Q_MY_COLLECTION_VIDEO_ALL_LIST";
const QString PAGECODE_Q_MY_COLLECTION_PURCHASES_ALBUM_LIST = "Q_MY_COLLECTION_PURCHASES_ALBUM_LIST";
const QString PAGECODE_Q_MY_COLLECTION_PURCHASES_TRACK_LIST = "Q_MY_COLLECTION_PURCHASES_TRACK_LIST";

const QString PAGECODE_Q_MY_RECENTLY_ALBUM_ALL_LIST = "Q_MY_RECENTLY_ALBUM_ALL_LIST";
const QString PAGECODE_Q_MY_RECENTLY_PLAYLIST_ALL_LIST = "Q_MY_RECENTLY_PLAYLIST_ALL_LIST";
const QString PAGECODE_Q_MY_RECENTLY_TRACK_ALL_LIST = "Q_MY_RECENTLY_TRACK_ALL_LIST";
const QString PAGECODE_Q_MY_ROSE_PLAYLIST_DETAIL = "Q_MY_ROSE_PLAYLIST_DETAIL";
const QString PAGECODE_Q_MY_ROSE_PLAYLIST_ALL_LIST = "Q_MY_ROSE_PLAYLIST_ALL_LIST";
const QString PAGECODE_Q_USER_ROSE_PLAYLIST_ALL_LIST = "Q_USER_ROSE_PLAYLIST_ALL_LIST";

// QOBUZ Search
const QString PAGECODE_Q_SEARCH_MAIN = "Q_SEARCH_MAIN";
const QString PAGECODE_Q_SEARCH_TRACK_ALL_VIEW = "Q_SEARCH_TRACK_ALL_VIEW";
const QString PAGECODE_Q_SEARCH_ARTIST_ALL_VIEW = "Q_SEARCH_ARTIST_ALL_VIEW";
const QString PAGECODE_Q_SEARCH_ALBUM_ALL_VIEW = "Q_SEARCH_ALBUM_ALL_VIEW";
const QString PAGECODE_Q_SEARCH_PLAYLIST_ALL_VIEW = "Q_SEARCH_PLAYLIST_ALL_VIEW";
const QString PAGECODE_Q_SEARCH_VIDEO_ALL_VIEW = "Q_SEARCH_VIDEO_ALL_VIEW";



//------------------------------------------------------------------------------------------
// MARK : APPLE MUSIC
//------------------------------------------------------------------------------------------
// APPLE MUSIC > 각 서브 페이지
const QString PAGECODE_APPLE_HOME = "APPLE_HOME";                                           ///< APPLE MUSIC > Home
const QString PAGECODE_APPLE_EXPLORE = "APPLE_EXPLORE";                                     ///< APPLE MUSIC > Explore
const QString PAGECODE_APPLE_SETTINGS = "APPLE_SETTINGS";                                   ///< APPLE MUSIC > Settings

// APPLE MUSIC > 각 서브 페이지 > View All
const QString PAGECODE_APPLE_HOME_RECENTLY_ALBUM = "APPLE_HOME_RECENTLY_ALBUM";             ///< APPLE MUSIC > Home > Recently Album ROSE
const QString PAGECODE_APPLE_HOME_RECENTLY_PLAYLIST = "APPLE_HOME_RECENTLY_PLAYLIST";       ///< APPLE MUSIC > Home > Recently Playlist ROSE
const QString PAGECODE_APPLE_HOME_RECENTLY_TRACK = "APPLE_HOME_RECENTLY_TRACK";             ///< APPLE MUSIC > Home > Recently Track ROSE
const QString PAGECODE_APPLE_HOME_MY_PLAYLIST = "APPLE_HOME_MY_PLAYLIST";                   ///< APPLE MUSIC > Home > My Playlist ROSE
const QString PAGECODE_APPLE_HOME_USER_PLAYLIST = "APPLE_HOME_USER_PLAYLIST";               ///< APPLE MUSIC > Home > User Playlist ROSE
const QString PAGECODE_APPLE_HOME_CHART_ALBUM = "APPLE_HOME_CHART_ALBUM";                   ///< APPLE MUSIC > Home > Chart Album
const QString PAGECODE_APPLE_HOME_CHART_PLAYLIST = "APPLE_HOME_CHART_PLAYLIST";             ///< APPLE MUSIC > Home > Chart Playlist

// APPLE MUSIC 공용사용
const QString PAGECODE_APPLE_ALBUM_DETAIL = "APPLE_ALBUM_DETAIL";
const QString PAGECODE_APPLE_PLAYLIST_DETAIL = "APPLE_PLAYLIST_DETAIL";
const QString PAGECODE_APPLE_PLAYLIST_ROSE_DETAIL = "APPLE_PLAYLIST_ROSE_DETAIL";
const QString PAGECODE_APPLE_ARTIST_DETAIL = "APPLE_ARTIST_DETAIL";

const QString PAGECODE_APPLE_ALBUM_ALL_LIST_VIEW = "APPLE_ALBUM_ALL_LIST_VIEW";
const QString PAGECODE_APPLE_PLAYLIST_ALL_LIST_VIEW = "APPLE_PLAYLIST_ALL_LIST_VIEW";
const QString PAGECODE_APPLE_ARTIST_ALL_LIST_VIEW = "APPLE_ARTIST_ALL_LIST_VIEW";
const QString PAGECODE_APPLE_TRACK_ALL_LIST_VIEW = "APPLE_TRACK_ALL_LIST_VIEW";
const QString PAGECODE_APPLE_TRACK_SHARE_ALL_LIST_VIEW = "APPLE_TRACK_SHARE_ALL_LIST_VIEW";



//------------------------------------------------------------------------------------------
// MARK : Spotify  --> 근데 뭐지? ^-^
//------------------------------------------------------------------------------------------
const QString PAGECODE_SP_HOME = "home";                ///< Spotify > 홈



//------------------------------------------------------------------------------------------
// MARK : 설정
//------------------------------------------------------------------------------------------
const QString PAGECODE_S_MYPAGE = "myPage";                     ///< 설정 > 홈

const QString PAGECODE_S_FRIEND = "friend";                     ///< 설정 > 친구관리

const QString PAGECODE_S_MEDIALIB = "mediaLib";                 ///< 설정 > 미디어 라이브러리
const QString PAGECODE_S_SUB_MEDIALIB = "mediaLib_sub";         ///< 설정 > 미디어 라이브러리 > 추가/삭제

const QString PAGECODE_S_DISPLAY = "display";                   ///< 설정 > 디스플레이
const QString PAGECODE_S_TRACK = "trackOption";                 ///< 설정 > 곡 옵션
const QString PAGECODE_S_COUNTRY = "countryOption";             ///< 설정 > 국가 채널 옵션
const QString PAGECODE_S_ROSETUBE = "rosetubeOption";           ///< 설정 > 로즈튜브 옵션
const QString PAGECODE_S_VIDEO = "videoOption";                 ///< 설정 > 비디오 옵션
const QString PAGECODE_S_VU = "vuOption";                       ///< 설정 > VU 옵션
const QString PAGECODE_S_INPUTOUTPUT = "inputOutput";           ///< 설정 > 입출력 설정
const QString PAGECODE_S_HDMI = "hdmiOption";                 ///< 설정 > HDMI 옵션 //cheon29_src
const QString PAGECODE_S_TIMER = "timerOption";                 ///< 설정 > TIMER 옵션 //cheon29_src
const QString PAGECODE_S_LANGUAGE = "languageOption";//cheon07_language
const QString PAGECODE_S_ROSEMENUEDIT = "roseMenuEdit";         ///< 설정 > 기기 메뉴 편집
const QString PAGECODE_S_APPMENUEDIT = "appMenuEdit";           ///< 설정 > 앱 메뉴 편집
const QString PAGECODE_S_APPINFO = "appInfo";                   ///< 설정 > 앱 정보



//------------------------------------------------------------------------------------------
// MARK : 옵션 팝업 ----> 사용하는 건지 확인 필요
//------------------------------------------------------------------------------------------
const QString PAGECODE_OP_ALBUMINFO = "albumInfo";              ///< 옵션팝업 > 앨범상세
const QString PAGECODE_OP_RECENTALBUMINFO = "recentAlbumInfo";  ///< 옵션팝업 > 앨범상세
const QString PAGECODE_OP_RECENTPLAYLISTINFO = "recentPlaylistInfo";  ///< 옵션팝업 > 앨범상세
const QString PAGECODE_OP_PLAYLISTINFO = "playListInfo";        ///< 옵션팝업 > 플레이리스트 상세
const QString PAGECODE_OP_ARTISTINFO = "artistInfo";            ///< 옵션팝업 > 아티스트 상세
const QString PAGECODE_OP_GENREINFO = "genreInfo";              ///< 옵션팝업 > 장르 상세
const QString PAGECODE_OP_COMPOSERINFO = "composerInfo";        ///< 옵션팝업 > 작곡가 상세
const QString PAGECODE_OP_VIDEOINFO = "videoInfo";              ///< 옵션팝업 > 비디오 상세
const QString PAGECODE_OP_FILEINFO = "fileInfo";                ///< 옵션팝업 > 파일정보
const QString PAGECODE_OP_METAINFO = "metaInfo";                ///< 옵션팝업 > 메타정보
const QString PAGECODE_OP_MULTI = "multi";                      ///< 옵션팝업 > 여러항목


#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";



// Hover Item code
const QString HOVER_CODE_FAV = "fav";
const QString HOVER_CODE_PLAY = "play";
const QString HOVER_CODE_MORE = "more";
const QString HOVER_CODE_DEL = "del";

/**
 * @brief The GSCommon class 전체 프로젝트에서 공용으로 쓰는 func/enum/const 등등.. 모음
 */
class GSCommon
{
    //Q_GADGET // Q_OBJECT
public:
    /// 메인메뉴 페이지 코드
    enum MainMenuCode {
        RoseHome,
        Music,
        Music_2,
        MusicNetwork,
        Video,
        Radio,
        RoseRadio,
        RoseFM,
        RoseTube,
        RoseTubeList,
        PodCast,
        CDplay,//c220721
        Tidal,
        Bugs,
        Qobuz,
        AppleMusic,
        NaverVibe,
        AmazonMusic,
        Spotify,
        Setting
    };
    //Q_ENUM(MainMenuCode); // Q_ENUMS is obsolete
    //Q_DECLARE_METATYPE(MainMenuCode);

    GSCommon();
    static QString getTextCutFromLabelWidth(QString p_text, int p_width, const QFont &p_font);
    static void clearLayout(QLayout *layout);
    static void clearLayout_album_onePage(QLayout *layout, int start_index, int end_index);
    static void clearLayout_album(QLayout *layout, int line_cnt, int album_cnt);//c22040301
    static void clearLayout_new(QLayout *layout);
    static QPixmap* getUIPixmapImg(QString p_url);
    static QPixmap* getUIPixmapImg_WH(QString p_url, int w, int h);//c220816
    static QLabel* getUILabelImg(QString p_url, QWidget *parent = nullptr);
    static QLabel* getUILabelImg_search(QString p_url, QWidget *parent = nullptr);

    static QPushButton* getUIBtnImg(QString p_objName, QString p_url, QWidget *parent = nullptr);
    static void applyBtnImage(QPushButton *p_btn, QString p_url);

    static bool checkFileExists(const QString &p_filePath);
    static QWidget* getUIControlTitleForSetting(QString p_title);

    static QWidget* getHorizontalBar(QString p_color, int p_height);

    static QString convertTimeFormat(const int &p_milliseconds, bool p_flagIsMSec=true);
private:

};


#endif // GSCOMMON_H
