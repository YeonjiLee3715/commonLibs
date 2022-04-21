/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "CGstAudioPlayer.h"
#include "../utils/CStrManager.h"

#define GST_DEBUG_LEVEL "1,2,3" // print important gstreamer messages on stderr

//임시코드
#define CAPS "audio/x-raw, format=(string)S16LE, layout=(string)interleaved, rate=[ 1, MAX ], channels=(int)[ 1, MAX ]"

CStreamInfoDelegate::CStreamInfoDelegate(int &arg, char **argv)
    : m_parser(NULL), m_dec(NULL), m_sink(NULL), m_bitrate(0)
    , m_samplerate(0), m_channels(0), m_bIsComplete(false)
{
    if( gst_is_initialized() == false )
        gst_init( &arg, &argv );

    m_parser = gst_pipeline_new ("pipeline");
    m_dec = gst_element_factory_make ("uridecodebin", NULL);
    gst_bin_add (GST_BIN (m_parser), m_dec);

    m_sink = gst_element_factory_make ("fakesink", NULL);
    gst_bin_add (GST_BIN (m_parser), m_sink);
}

CStreamInfoDelegate::~CStreamInfoDelegate()
{
    gst_element_set_state(m_parser, GST_STATE_NULL);
    gst_object_unref(m_parser);
}

void CStreamInfoDelegate::setUri(const std::string &uri)
{
    clear();
    gst_element_set_state(m_dec, GST_STATE_NULL);
    g_object_set(G_OBJECT(m_dec), "uri", uri.c_str(), NULL);
}

void CStreamInfoDelegate::setFile(const std::string &file)
{
    clear();
    gst_element_set_state(m_dec, GST_STATE_NULL);
    g_object_set(G_OBJECT(m_dec), "local", file.c_str(), NULL);
}

bool CStreamInfoDelegate::setPadAddedCallback()
{
    g_signal_connect (m_dec, "pad-added", G_CALLBACK( CStreamInfoDelegate::on_new_pad ), this);
    gst_element_set_state (m_parser, GST_STATE_PAUSED);
}

bool CStreamInfoDelegate::setPadAddedCallback(GCallback callback, gpointer data)
{
    g_signal_connect (m_dec, "pad-added", callback, data);
    gst_element_set_state (m_parser, GST_STATE_PAUSED);
}


bool CStreamInfoDelegate::setBusCallback()
{
    GstBus* bus = gst_element_get_bus(m_parser);
    gst_bus_add_watch(bus, CStreamInfoDelegate::bus_callback, this);
    gst_object_unref(bus);
}

bool CStreamInfoDelegate::setBusCallback(GstBusFunc callback, gpointer data)
{
    GstBus* bus = gst_element_get_bus(m_parser);
    gst_bus_add_watch(bus, callback, data);
    gst_object_unref(bus);
}

void CStreamInfoDelegate::parseTags()
{
    gst_element_set_state(m_parser, GST_STATE_PLAYING);
}

void CStreamInfoDelegate::stop()
{
    gst_element_set_state(m_parser, GST_STATE_NULL);
}

void CStreamInfoDelegate::clear()
{
    stop();

    m_title.clear();
    m_album.clear();
    m_artist.clear();

    m_bitrate = 0;
    m_samplerate = 0;
    m_channels = 0;

    m_bIsComplete = false;
}

void CStreamInfoDelegate::on_new_pad(GstElement *dec, GstPad *pad,  gpointer user_data)
{
    CStreamInfoDelegate* pDelegate = reinterpret_cast<CStreamInfoDelegate*>(user_data);

    GstPad* sinkpad;
    GstCaps *caps;
#if GST_CHECK_VERSION(1,0,0)
   caps = gst_pad_get_current_caps(pad);
#else
   caps = gst_pad_get_caps(pad);
#endif

    GstStructure* gstStructure = gst_caps_get_structure(caps, 0);
    const gchar* strName = gst_structure_get_name(gstStructure);

    if (g_str_has_prefix(strName, "audio/"))
    {
        gint nValue;
        if( gst_structure_get_int( gstStructure, "rate", &nValue ) )
        {
            g_print ("\t%20s : %d\n", "rate", nValue);
            pDelegate->setSamplerate( nValue );
        }
        if( gst_structure_get_int( gstStructure, "channels", &nValue ) )
        {
            g_print ("\t%20s : %d\n", "channels", nValue);
            pDelegate->setChannels( nValue );
        }

#if GST_CHECK_VERSION(1,0,0)
        sinkpad = gst_element_get_static_pad(pDelegate->m_sink, "sink");
#else
        sinkpad = gst_element_get_pad(pDelegate->m_sink, "sink");
#endif

        if (!gst_pad_is_linked (sinkpad))
        {
          if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK)
            g_error ("Failed to link pads!");
        }
        gst_object_unref (sinkpad);
    }

    gst_caps_unref(caps);
}

void CStreamInfoDelegate::print_one_tag(const GstTagList *list, const gchar *tag, gpointer user_data)
{
    int total = gst_tag_list_get_tag_size (list, tag);

    for( int idx = 0; idx < total; ++idx)
    {
        const GValue *val;

        /* Note: when looking for specific tags, use the gst_tag_list_get_xyz() API,
         * we only use the GValue approach here because it is more generic */
        val = gst_tag_list_get_value_index (list, tag, idx);
        if (G_VALUE_HOLDS_STRING (val)) {
          g_print ("\t%20s : %s\n", tag, g_value_get_string (val));
        } else if (G_VALUE_HOLDS_UINT (val)) {
          g_print ("\t%20s : %u\n", tag, g_value_get_uint (val));
        } else if (G_VALUE_HOLDS_DOUBLE (val)) {
          g_print ("\t%20s : %g\n", tag, g_value_get_double (val));
        } else if (G_VALUE_HOLDS_BOOLEAN (val)) {
          g_print ("\t%20s : %s\n", tag,
              (g_value_get_boolean (val)) ? "true" : "false");
        } else if (GST_VALUE_HOLDS_BUFFER (val)) {
          GstBuffer *buf = gst_value_get_buffer (val);
          guint buffer_size = gst_buffer_get_size (buf);

          g_print ("\t%20s : buffer of size %u\n", tag, buffer_size);
        } else if (GST_VALUE_HOLDS_DATE_TIME (val)) {
          GstDateTime *dt = reinterpret_cast<GstDateTime*>( g_value_get_boxed (val));
          gchar *dt_str = gst_date_time_to_iso8601_string (dt);

          g_print ("\t%20s : %s\n", tag, dt_str);
          g_free (dt_str);
        } else {
          g_print ("\t%20s : tag of type '%s'\n", tag, G_VALUE_TYPE_NAME (val));
        }
    }
}

void CStreamInfoDelegate::track_from_tag(const GstTagList *list, const gchar *tag, gpointer user_data)
{
    CStreamInfoDelegate* pDelegate = reinterpret_cast<CStreamInfoDelegate*>(user_data);

    if( g_strcmp0(tag, GST_TAG_TITLE) == 0 )
    {
        gchar* strValue = 0;
        gst_tag_list_get_string(list, tag, &strValue);
        pDelegate->setTitle( std::string(strValue) );
        g_print ("\t%20s : %s\n", GST_TAG_TITLE, strValue);
        g_free(strValue);
    }
    else if( g_strcmp0(tag, GST_TAG_ARTIST) == 0 )
    {
        gchar* strValue = 0;
        gst_tag_list_get_string(list, tag, &strValue);
        pDelegate->setArtist( std::string(strValue) );
        g_print ("\t%20s : %s\n", GST_TAG_ARTIST, strValue);
        g_free(strValue);
    }
    else if( g_strcmp0(tag, GST_TAG_ALBUM) == 0 )
    {
        gchar* strValue = 0;
        gst_tag_list_get_string(list, tag, &strValue);
        pDelegate->setAlbum( std::string(strValue) );
        g_print ("\t%20s : %s\n", GST_TAG_ALBUM, strValue);
        g_free(strValue);
    }
    else if( g_strcmp0(tag, GST_TAG_BITRATE) == 0 )
    {
        guint nValue = 0;
        gst_tag_list_get_uint(list, tag, &nValue);
        pDelegate->setBitrate( nValue );
        g_print ("\t%20s : %d\n", GST_TAG_BITRATE, nValue);
    }
}

gboolean CStreamInfoDelegate::bus_callback(GstBus *sender, GstMessage *msg, void *data)
{
    CStreamInfoDelegate* pDelegate = reinterpret_cast<CStreamInfoDelegate*>(data);
    if( pDelegate == NULL )
        return true;

    switch (GST_MESSAGE_TYPE(msg))
    {
      case GST_MESSAGE_TAG:
      {
        GstTagList* tags;
        gchar* uri;
        g_object_get(pDelegate->getDecoder(), "uri", &uri, NULL);
        gst_message_parse_tag(msg, &tags);
        g_free(uri);
        gst_tag_list_foreach (tags, print_one_tag, pDelegate);
        gst_tag_list_free(tags);
      }
        break;

      case GST_MESSAGE_ASYNC_DONE:
        g_print("TagsDelegate: End of tagging. Stopping!");
        pDelegate->parseComplete();
        pDelegate->stop();
        break;
    }

    return true;
}

CGstAudioPlayer::CGstAudioPlayer(int &arg, char **argv)
    : m_parser(NULL), m_player(NULL), m_idNewSampleCallback(0), m_statusParser(GST_STATE_NULL), m_statusPlayer(GST_STATE_NULL)
{
#if defined(GST_DEBUG_LEVEL)
    setenv("GST_DEBUG", GST_DEBUG_LEVEL, 1);
#endif

    if( gst_is_initialized() == false )
        gst_init( &arg, &argv );

    setParser();
    setPlayer();
}

CGstAudioPlayer::~CGstAudioPlayer()
{
    stop(true);

    gst_object_unref(m_parser);
    gst_object_unref(m_player);
}

void CGstAudioPlayer::setUri(const std::string &uri)
{
    stop(true);

    GstElement* source = getSource();
    g_object_set( G_OBJECT( source ), "uri", uri.c_str(), NULL);
}

void CGstAudioPlayer::setFile(const std::string &file)
{
    stopParser();
    stopPlayer();

    GstElement* source = getSource();
    g_object_set( G_OBJECT( source ), "uri", file.c_str(), NULL);
}

bool CGstAudioPlayer::setParserBusCallback()
{
    GstBus* bus = gst_element_get_bus(m_parser);
    gst_bus_add_watch(bus, CGstAudioPlayer::parser_bus_callback, this);
    gst_object_unref(bus);
    return true;
}

bool CGstAudioPlayer::setParserBusCallback(GstBusFunc callback, gpointer data)
{
    GstBus* bus = gst_element_get_bus(m_parser);
    gst_bus_add_watch(bus, callback, data);
    gst_object_unref(bus);
    return true;
}

bool CGstAudioPlayer::setPlayerBusCallback()
{
    GstBus* bus = gst_element_get_bus(m_player);
    gst_bus_add_watch(bus, CGstAudioPlayer::player_bus_callback, this);
    gst_object_unref(bus);
}

bool CGstAudioPlayer::setPlayerBusCallback(GstBusFunc callback, gpointer data)
{
    GstBus* bus = gst_element_get_bus(m_player);
    gst_bus_add_watch(bus, callback, data);
    gst_object_unref(bus);
    return true;
}

/* we use appsink in push mode, it sends us a signal when data is available
 * and we pull out the data in the signal callback. We want the appsink to
 * push as fast as it can, hence the sync=false */
void CGstAudioPlayer::setEmitNewSampleSignal(bool isEmitSignal )
{
    GstElement* appSink = getAppSink();
    if( isEmitSignal )
    {
//        g_object_set( appSink, "emit-signals", TRUE, "sync", FALSE, NULL);

        //임시코드
        {
            GstCaps* caps;
            caps= gst_caps_from_string( CAPS );
            g_object_set( appSink, "emit-signals", TRUE, "sync", FALSE, "caps", caps, NULL);
            gst_caps_unref( caps );
        }
    }
    else
        g_object_set( appSink, "emit-signals", FALSE, "sync", FALSE, NULL);
}

bool CGstAudioPlayer::setNewSampleCallback()
{
    GstElement* appSink = getAppSink();
    m_idNewSampleCallback = g_signal_connect( appSink, "new-sample", G_CALLBACK(CGstAudioPlayer::new_audio_buffer), this );
    return true;
}

bool CGstAudioPlayer::setNewSampleCallback(GCallback callback, gpointer data)
{
    GstElement* appSink = getAppSink();
    m_idNewSampleCallback = g_signal_connect( appSink, "new-sample", callback, data );
    return true;
}

bool CGstAudioPlayer::setPadAddedCallback()
{
    GstElement* source = getSource();
    g_signal_connect (source, "pad-added", G_CALLBACK( CStreamInfoDelegate::on_new_pad ), this);

    gst_element_set_state (m_parser, GST_STATE_PAUSED);
    return true;
}

bool CGstAudioPlayer::setPadAddedCallback(GCallback callback, gpointer data)
{
    GstElement* source = getSource();
    g_signal_connect (source, "pad-added", callback, data);

    gst_element_set_state (m_parser, GST_STATE_PAUSED);
    return true;
}

bool CGstAudioPlayer::setCaps(GstCaps *caps)
{
    stopPlayer();

    GstElement* appSource = getAppSource();
    g_object_set ( appSource, "caps", caps, "format", GST_FORMAT_TIME, NULL);

    return playPlayer();
}

GstElement *CGstAudioPlayer::getAppSink()
{
    return gst_bin_get_by_name(GST_BIN(m_parser), PARSER_APPSINK);
}

GstElement *CGstAudioPlayer::getParserConvert()
{
    return gst_bin_get_by_name(GST_BIN(m_parser), PARSER_AUDIOCONVERT);
}

GstElement *CGstAudioPlayer::getSource()
{
    return gst_bin_get_by_name(GST_BIN(m_parser), PARSER_URIDECODEBIN );
}

GstElement *CGstAudioPlayer::getAppSource()
{
    return gst_bin_get_by_name(GST_BIN(m_player), PLAYER_APPSRC );
}

GstElement *CGstAudioPlayer::getAudioReSample()
{
    return gst_bin_get_by_name(GST_BIN(m_player), PLAYER_AUDIORESAMPLE );
}

GstElement *CGstAudioPlayer::getAudioSink()
{
    return gst_bin_get_by_name(GST_BIN(m_player), PLAYER_AUTOAUDIOSINK );
}

GstElement *CGstAudioPlayer::getPlayerConvert()
{
    return gst_bin_get_by_name(GST_BIN(m_player), PLAYER_AUDIOCONVERT );
}

GstElement *CGstAudioPlayer::getVolume()
{
    return gst_bin_get_by_name(GST_BIN(m_player), PLAYER_VOLUME );
}

void CGstAudioPlayer::unrefObject( GstElement* obj )
{
    if( obj )
        gst_object_unref(obj);
}

bool CGstAudioPlayer::play( bool isWithPlayer )
{
    if( playParser() == false )
        return false;

    if( isWithPlayer && playPlayer() == false )
        return false;

    return true;
}

bool CGstAudioPlayer::playParser()
{
    if( isParserPlaying() )
        return true;

    GstStateChangeReturn ret = gst_element_set_state(m_parser, GST_STATE_PLAYING);
    if(ret == GST_STATE_CHANGE_FAILURE)
    {
        g_print("Failed to start parser. error: %d\n", ret);
        return false;
    }

    m_statusParser = GST_STATE_PLAYING;
    return true;
}

bool CGstAudioPlayer::playPlayer()
{
    if( isPlayerPlaying() )
        return true;

    GstStateChangeReturn ret = gst_element_set_state(m_player, GST_STATE_PLAYING);
    if(ret == GST_STATE_CHANGE_FAILURE)
    {
        g_print("Failed to start player. error: %d\n",ret);
        return false;
    }

    m_statusPlayer = GST_STATE_PLAYING;
    return true;
}

bool CGstAudioPlayer::stop(bool isWithPlayer)
{
    if( stopParser() == false )
        return false;

    if( !isWithPlayer )
        return true;

    if( isWithPlayer && stopPlayer() == false )
        return false;

    return true;
}

bool CGstAudioPlayer::stopParser()
{
    if( parserState() == GST_STATE_NULL )
        return true;

    pauseParser();

    GstStateChangeReturn ret = gst_element_set_state(m_parser, GST_STATE_NULL);
    if(ret == GST_STATE_CHANGE_FAILURE)
    {
        g_print("Failed to stop parser. error: %d\n",ret);
        return false;
    }

    m_statusParser = GST_STATE_NULL;
    return true;
}

bool CGstAudioPlayer::stopPlayer()
{
    if( playerState() == GST_STATE_NULL )
        return true;

    pausePlayer();

    GstStateChangeReturn ret = gst_element_set_state(m_player, GST_STATE_NULL);
    if(ret == GST_STATE_CHANGE_FAILURE)
    {
        g_print("Failed to stop player. error: %d",ret);
        return false;
    }

    m_statusPlayer = GST_STATE_NULL;
    return true;
}

bool CGstAudioPlayer::pause(bool isWithPlayer)
{
    if( pauseParser() == false )
        return false;

    if( !isWithPlayer )
        return true;

    if( pausePlayer() == false )
        return false;

    return true;
}

bool CGstAudioPlayer::pauseParser()
{
    if( parserState() != GST_STATE_PLAYING )
        return true;

    GstStateChangeReturn ret = gst_element_set_state(m_parser, GST_STATE_PAUSED);
    if(ret == GST_STATE_CHANGE_FAILURE)
    {
        g_print("Failed to pause parser. error: %d\n",ret);
        return false;
    }

    m_statusParser = GST_STATE_PAUSED;
    return true;
}

bool CGstAudioPlayer::pausePlayer()
{
    if( playerState() != GST_STATE_PLAYING )
        return true;

    GstStateChangeReturn ret = gst_element_set_state(m_player, GST_STATE_PAUSED);
    if(ret == GST_STATE_CHANGE_FAILURE)
    {
        g_print("Failed to pause player. error: %d",ret);
        return false;
    }

    m_statusPlayer = GST_STATE_PAUSED;
    return true;
}

void CGstAudioPlayer::setParserState(GstState status)
{
    m_statusParser = status;
}

void CGstAudioPlayer::setPlayerState(GstState status)
{
    m_statusPlayer = status;
}

GstState CGstAudioPlayer::parserStateFromPipeline()
{
    gst_element_get_state(m_parser, &m_statusParser, NULL, -1);
    return m_statusParser;
}

GstState CGstAudioPlayer::playerStateFromPipeline()
{
    gst_element_get_state(m_player, &m_statusPlayer, NULL, -1);
    return m_statusPlayer;
}

GstState CGstAudioPlayer::parserState()
{
    return m_statusParser;
}

GstState CGstAudioPlayer::playerState()
{
    return m_statusPlayer;
}

void CGstAudioPlayer::setVolume(gdouble delta)
{
    gdouble vol;
    g_object_get(m_player, "volume", &vol, NULL);
    vol += delta;
    g_object_set(m_player, "volume", vol, NULL);
    g_print("volume set to %.0f%%\n", 100 * vol);
}

gint64 CGstAudioPlayer::getParserPosition()
{
    gint64 pos = -1;
    if( gst_element_query_position( getParser(), GST_FORMAT_TIME, &pos ) == false )
        g_print( "Failed to get current position" );

    return pos;
}

gint64 CGstAudioPlayer::getPlayerPosition()
{
    gint64 pos = -1;
    if( gst_element_query_position( getPlayer(), GST_FORMAT_TIME, &pos ) == false )
        g_print( "Failed to get current position" );

    return pos;
}

gint64 CGstAudioPlayer::getParserDuration()
{
    gint64 duration = -1;
    if( gst_element_query_duration( getParser(), GST_FORMAT_TIME, &duration ) == false )
        g_print( "Failed to get current duration" );
    else
        g_print ("Duration: %u:%02u:%02u.%09u", GST_TIME_ARGS (duration) );

    return duration;
}

gint64 CGstAudioPlayer::getPlayerDuration()
{
    gint64 duration = -1;
    if( gst_element_query_duration( getPlayer(), GST_FORMAT_TIME, &duration ) == false )
        g_print( "Failed to get current duration" );
    else
        g_print ("Duration: %u:%02u:%02u.%09u", GST_TIME_ARGS (duration) );

    return duration;
}

bool CGstAudioPlayer::seekParser(gint64 pos)
{
    gint64 nowPos;
    GstFormat format = GST_FORMAT_TIME;

    if( gst_element_query_position(m_parser, format, &nowPos))
    {
        if (format != GST_FORMAT_TIME)
        {
            g_warning("Avoiding seek... GST_FORMAT other than time!. This is a bug in gplay!\n");
            return false;
        }

        if( gst_element_seek_simple(m_parser,
                  GST_FORMAT_TIME,
                  GST_SEEK_FLAG_FLUSH,
                  pos) == false )
            g_error( "Failed to Seek.\n" );
    }
    else
    {
        g_print("Not a seekable stream!\n");
        return false;
    }

    return true;
}

bool CGstAudioPlayer::seekPlayer(gint64 pos)
{
    gint64 nowPos;
    GstFormat format = GST_FORMAT_TIME;

    if( gst_element_query_position(m_player, format, &nowPos))
    {
        if (format != GST_FORMAT_TIME)
        {
            g_warning("Avoiding seek... GST_FORMAT other than time!. This is a bug in gplay!\n");
            return false;
        }

        gst_element_seek_simple(m_player,
                  GST_FORMAT_TIME,
                  GST_SEEK_FLAG_FLUSH,
                  pos);
    }
    else
    {
        g_print("Not a seekable stream!\n");
        return false;
    }

    return true;
}

bool CGstAudioPlayer::isParserPlaying()
{
    return m_statusParser == GST_STATE_PLAYING;
}

bool CGstAudioPlayer::isPlayerPlaying()
{
    return m_statusPlayer == GST_STATE_PLAYING;
}

void CGstAudioPlayer::setParser()
{
    GstElement* appSink = gst_element_factory_make("appsink", PARSER_APPSINK);
    GstElement* parserConvert = gst_element_factory_make ("audioconvert", PARSER_AUDIOCONVERT);
    GstElement* source = gst_element_factory_make ("uridecodebin", PARSER_URIDECODEBIN);

    // create parser pipeline
    m_parser = gst_pipeline_new (PARSER_PIPELINE);

    if (!m_parser || !appSink || !parserConvert || !source )
    {
        g_printerr ("Not all elements could be created.\n");
        return;
    }

    gst_element_set_state(m_parser, GST_STATE_NULL);

    gst_bin_add_many (GST_BIN(m_parser), source, appSink, parserConvert, NULL);
    gst_element_link_many( parserConvert, appSink, NULL);

    stopParser();
}

void CGstAudioPlayer::setPlayer()
{
    GstElement* appSource = gst_element_factory_make ("appsrc", PLAYER_APPSRC);
    GstElement* audioresample = gst_element_factory_make ("audioresample", PLAYER_AUDIORESAMPLE);
    GstElement* audioSink = gst_element_factory_make ("autoaudiosink", PLAYER_AUTOAUDIOSINK);
    GstElement* playerConvert = gst_element_factory_make ("audioconvert", PLAYER_AUDIOCONVERT);
    GstElement* volume = gst_element_factory_make ("volume", PLAYER_VOLUME);

    // create player pipeline
    m_player = gst_pipeline_new(PLAYER_PIPELINE);

    if (!m_player || !appSource || !audioresample || !audioSink || !playerConvert || !volume )
    {
        g_printerr ("Not all elements could be created.\n");
        return;
    }

    gst_element_set_state(m_player, GST_STATE_NULL);

    gst_bin_add_many (GST_BIN (m_player), appSource, audioresample, audioSink, playerConvert, volume, NULL);
    gst_element_link_many(appSource, audioresample, audioSink, playerConvert, volume, NULL);
}

void CGstAudioPlayer::on_parser_state_changed(GstState newState)
{
    switch (newState){
      case GST_STATE_PAUSED:
        g_print("Parser PAUSED/n");
        break;
      case GST_STATE_PLAYING:
        g_print("Parser PLAYING/n");
        break;
      case GST_STATE_READY:
      case  GST_STATE_NULL:
        g_print("Parser STOPED/n");
        break;
      default:
        break;
    }

    m_statusParser = newState;
}

void CGstAudioPlayer::on_player_state_changed(GstState newState)
{
    switch (newState){
      case GST_STATE_PAUSED:
        g_print("Player PAUSED/n");
        break;
      case GST_STATE_PLAYING:
        g_print("Player PLAYING/n");
        break;
      case GST_STATE_READY:
      case  GST_STATE_NULL:
        g_print("Player STOPED/n");
        break;
      default:
        break;
    }

    m_statusPlayer = newState;
}

void CGstAudioPlayer::on_parser_eos()
{
    stopParser();
}

void CGstAudioPlayer::on_player_eos()
{
    stopPlayer();
}

void CGstAudioPlayer::on_new_pad(GstElement *dec, GstPad *pad, gpointer user_data)
{
    CGstAudioPlayer* pGstAudioPlayer = reinterpret_cast<CGstAudioPlayer*>( user_data );
    if( pGstAudioPlayer == NULL )
        return;

    GstPad* sinkpad;
    GstCaps *caps;
#if GST_CHECK_VERSION(1,0,0)
   caps = gst_pad_get_current_caps(pad);
#else
   caps = gst_pad_get_caps(pad);
#endif

    GstStructure* gstStructure = gst_caps_get_structure(caps, 0);
    const gchar* strName = gst_structure_get_name(gstStructure);

    if (g_str_has_prefix(strName, "audio/"))
    {
        gint nValue = 0;
        gst_structure_get_int( gstStructure, "width", &nValue );
            g_print ("\t%20s : %d\n", "width", nValue);
#if GST_CHECK_VERSION(1,0,0)
        sinkpad = gst_element_get_static_pad(pGstAudioPlayer->getParserConvert(), "sink");
#else
        sinkpad = gst_element_get_pad(pGstAudioPlayer->getParserConvert(), "sink");
#endif

        if (!gst_pad_is_linked (sinkpad))
        {
            if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK)
                g_error ("Failed to link pads!");
            else
            {
                GstElement* appSource = pGstAudioPlayer->getAppSource();
                g_object_set( appSource, "caps", caps, "format", GST_FORMAT_TIME, NULL);

                gst_element_set_state(pGstAudioPlayer->m_player, GST_STATE_PLAYING);
            }
        }
        gst_object_unref(sinkpad);
    }

    gst_caps_unref(caps);
}

GstFlowReturn CGstAudioPlayer::new_audio_buffer (GstElement* sink, gpointer user_data)
{
    GstFlowReturn ret = GST_FLOW_OK;

    CGstAudioPlayer* pGstAudioPlayer = reinterpret_cast<CGstAudioPlayer*>( user_data );
    if( pGstAudioPlayer == NULL )
        return ret;

    GstBuffer* buffer;
    GstMapInfo map;
    GstSample *sample;

    //gsize size;
    g_signal_emit_by_name( pGstAudioPlayer->getAppSink(), "pull-sample", &sample, NULL );

    if( sample )
    {
        buffer = gst_sample_get_buffer( sample );

        gst_buffer_map( buffer, &map, GST_MAP_READ );
        gst_buffer_unmap( buffer, &map );

        /* Push the buffer into the appsrc */
        g_signal_emit_by_name (pGstAudioPlayer->getAppSource(), "push-buffer", buffer, &ret);

        gst_sample_unref( sample );
    }

    return ret;
}

gboolean CGstAudioPlayer::parser_bus_callback(GstBus *sender, GstMessage *message, void *user_data)
{
    CGstAudioPlayer* pPlayer = reinterpret_cast<CGstAudioPlayer*>(user_data);
    if( pPlayer == NULL )
        return true;

    switch (GST_MESSAGE_TYPE (message))
    {
      case GST_MESSAGE_STATE_CHANGED:
      {
        GstState newState;
        gst_message_parse_state_changed(message, NULL, &newState, NULL);
        std::string message_name(GST_MESSAGE_SRC_NAME(message));

        if( message_name.compare(PARSER_PIPELINE) == 0 )
            pPlayer->on_parser_state_changed(newState);
      }
        break;

      case GST_MESSAGE_TAG:
        break;

      case GST_MESSAGE_EOS:
        pPlayer->on_parser_eos();
        break;
      case GST_MESSAGE_BUFFERING:
      {
        gint percent = 0;
        gst_message_parse_buffering (message, &percent);
        g_print ("Buffering (%3d%%)\r", percent);
        /* Wait until buffering is complete before start/resume playing */
        if (percent < 100)
            pPlayer->pauseParser();
        else
            pPlayer->playParser();
        break;
      }
      case GST_MESSAGE_CLOCK_LOST:
        /* Get a new clock */
        pPlayer->pauseParser();
        pPlayer->playParser();
        break;
      case GST_MESSAGE_STREAM_STATUS:
        GstStreamStatusType message_type;
        gst_message_parse_stream_status(message, &message_type, NULL);
        g_print("Parser Stream status: %d\n", message_type);
        break;
      default:
        g_print("Parser Message from %s: %s\n", GST_MESSAGE_SRC_NAME(message), gst_message_type_get_name(GST_MESSAGE_TYPE(message)));
        break;
    }

    return true;
}

gboolean CGstAudioPlayer::player_bus_callback(GstBus *sender, GstMessage *message, void *user_data)
{
    CGstAudioPlayer* pPlayer = reinterpret_cast<CGstAudioPlayer*>(user_data);
    if( pPlayer == NULL )
        return true;

    switch (GST_MESSAGE_TYPE (message))
    {
      case GST_MESSAGE_STATE_CHANGED:
      {
        GstState newState;
        gst_message_parse_state_changed(message, NULL, &newState, NULL);

        std::string message_name(GST_MESSAGE_SRC_NAME(message));//TODO: Avoid this copy using glib
        if( message_name.compare(PLAYER_PIPELINE) == 0 )
            pPlayer->on_player_state_changed(newState);
      }
        break;

      case GST_MESSAGE_ASYNC_DONE:
        break;
      case GST_MESSAGE_TAG:
        break;
    case GST_MESSAGE_BUFFERING:
    {
      break;
    }
      case GST_MESSAGE_CLOCK_LOST:
          /* Get a new clock */
          pPlayer->pausePlayer();
          pPlayer->playPlayer();
          break;
      case GST_MESSAGE_EOS:
          pPlayer->on_player_eos();
          break;

      case GST_MESSAGE_STREAM_STATUS:
        GstStreamStatusType message_type;
        gst_message_parse_stream_status(message, &message_type, NULL);
        g_print("Player Stream status: %d\n", message_type);
        break;

      default:
        g_print("Player Message from %s: %s\n", GST_MESSAGE_SRC_NAME(message), gst_message_type_get_name(GST_MESSAGE_TYPE(message)));
        break;
    }

    return true;
}
