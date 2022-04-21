#ifndef CGSTAUDIOPLAYER_H
#define CGSTAUDIOPLAYER_H

// gstreamer includes
#include <gst/gst.h>
#include <glib.h>
#include <string>

#define PARSER_PIPELINE         "parser-pipeline"
#define PARSER_APPSINK          "sink"
#define PARSER_AUDIOCONVERT     "convert"
#define PARSER_URIDECODEBIN     "source"

#define PLAYER_PIPELINE         "player-pipeline"
#define PLAYER_APPSRC           "playerSource"
#define PLAYER_AUDIORESAMPLE    "playerResample"
#define PLAYER_AUTOAUDIOSINK    "sink"
#define PLAYER_AUDIOCONVERT     "convert"
#define PLAYER_VOLUME           "plyerVolume"

class CStreamInfoDelegate
{

public:
    CStreamInfoDelegate(int &arg, char **argv);
    virtual ~CStreamInfoDelegate();

    void setUri(const std::string &uri);
    void setFile(const std::string &file);

    bool setPadAddedCallback();
    bool setPadAddedCallback( GCallback callback, gpointer data );

    bool setBusCallback();
    bool setBusCallback( GstBusFunc callback, gpointer data );

    void setTitle( const std::string& title ){ m_title = title; }
    void setAlbum( const std::string& album ){ m_album = album; }
    void setArtist( const std::string& artist ){ m_artist = artist; }

    void setBitrate( int bitrate ){ m_bitrate = bitrate; }
    void setSamplerate( int samplerate ){ m_samplerate = samplerate; }
    void setChannels( int channels ){ m_channels = channels; }

    std::string getTitle(){ return m_title; }
    std::string getAlbum(){ return m_album; }
    std::string getArtist(){ return m_artist; }

    int getBitrate(){ return m_bitrate; }
    int getSamplerate(){ return m_samplerate; }
    int getChannels(){ return m_channels; }

    GstElement* getParser(){ return m_parser; }
    GstElement* getDecoder(){ return m_dec; }
    GstElement* getSink(){ return m_sink; }

    void parseTags();
    void stop();
    bool isParseComplete(){ return m_bIsComplete; }
    void parseComplete(){ m_bIsComplete = true; }

    void clear();

    static void on_new_pad (GstElement* dec, GstPad * pad, gpointer user_data);
    static void print_one_tag (const GstTagList* list, const gchar * tag, gpointer user_data);
    static void track_from_tag(const GstTagList* list, const gchar* tag, gpointer user_data );

private:
    static gboolean bus_callback(GstBus* sender, GstMessage* msg, void* data);

private:
    GstElement* m_parser;
    GstElement* m_dec;
    GstElement* m_sink;
    GstElement* m_appsink;

    std::string m_title;
    std::string m_album;
    std::string m_artist;

    int m_bitrate;
    int m_samplerate;
    int m_channels;

    bool m_bIsComplete;
};

class CGstAudioPlayer
{
public:
    CGstAudioPlayer( int &arg, char **argv );
    ~CGstAudioPlayer();

    void setUri(const std::string& uri);
    void setFile(const std::string& file);

    bool setParserBusCallback();
    bool setParserBusCallback( GstBusFunc callback, gpointer data );

    bool setPlayerBusCallback();
    bool setPlayerBusCallback( GstBusFunc callback, gpointer data );

    void setEmitNewSampleSignal(bool isEmitSignal);

    bool setNewSampleCallback();
    bool setNewSampleCallback(GCallback callback, gpointer data );

    bool setPadAddedCallback();
    bool setPadAddedCallback( GCallback callback, gpointer data );

    bool setCaps( GstCaps* caps );

    GstElement* getParser(){ return m_parser; }
    GstElement* getAppSink();
    GstElement* getParserConvert();
    GstElement* getSource();

    GstElement* getPlayer(){ return m_player; }
    GstElement* getAppSource();
    GstElement* getAudioReSample();
    GstElement* getAudioSink();
    GstElement* getPlayerConvert();
    GstElement* getVolume();

    void unrefObject( GstElement* obj );

    bool play(bool isWithPlayer = false);
    bool stop(bool isWithPlayer = false);
    bool pause(bool isWithPlayer = false);

    bool playParser();
    bool playPlayer();

    bool stopParser();
    bool stopPlayer();

    bool pauseParser();
    bool pausePlayer();

    void setParserState( GstState status );
    void setPlayerState( GstState status );

    GstState parserStateFromPipeline();
    GstState playerStateFromPipeline();

    GstState parserState();
    GstState playerState();

    void setVolume(gdouble delta);

    gint64 getParserPosition();
    gint64 getPlayerPosition();

    gint64 getParserDuration();
    gint64 getPlayerDuration();

    bool seekParser(gint64 pos);
    bool seekPlayer(gint64 pos);

    bool isParserPlaying();
    bool isPlayerPlaying();

private:
    void setParser();
    void setPlayer();

    void on_parser_state_changed(GstState newState);
    void on_player_state_changed(GstState newState);

    void on_parser_eos();
    void on_player_eos();

    static void on_new_pad (GstElement* dec, GstPad * pad, gpointer user_data);
    static GstFlowReturn new_audio_buffer (GstElement* sink, gpointer user_data);
    static gboolean parser_bus_callback(GstBus *sender, GstMessage *message, void *user_data);
    static gboolean player_bus_callback(GstBus *sender, GstMessage *message, void *user_data);

private:
    GstElement* m_parser;
    GstElement* m_player;

    gulong m_idNewSampleCallback;

    GstState m_statusParser;
    GstState m_statusPlayer;
};

#endif // CGSTAUDIOPLAYER_H
