// client.cpp, mostly network related client game code

#include "cube.h"
#include "bot/bot.h"

VAR(connected, 1, 0, 0);

ENetHost *clienthost = NULL;
ENetPeer *curpeer = NULL, *connpeer = NULL;
int connmillis = 0, connattempts = 0, discmillis = 0;
SVAR(curdemofile, "n/a");
extern bool clfail, cllock;
extern int searchlan;

int getclientnum() { return player1 ? player1->clientnum : -1; }

bool multiplayer(bool msg)
{
    // check not correct on listen server?
    if(curpeer && msg) conoutf("operation not available in multiplayer");
    return curpeer!=NULL;
}

bool allowedittoggle()
{
    bool allow = !curpeer || gamemode==1;
    if(!allow) conoutf("editing in multiplayer requires coopedit mode (1)");
    return allow;
}

void throttle();

VARF(throttle_interval, 0, 5, 30, throttle());
VARF(throttle_accel,    0, 2, 32, throttle());
VARF(throttle_decel,    0, 2, 32, throttle());

void throttle()
{
    if(!curpeer) return;
    ASSERT(ENET_PEER_PACKET_THROTTLE_SCALE==32);
    enet_peer_throttle_configure(curpeer, throttle_interval*1000, throttle_accel, throttle_decel);
}

string clientpassword = "";
int connectrole = CR_DEFAULT;
bool modprotocol = false;

void abortconnect()
{
    if(!connpeer) return;
    clientpassword[0] = '\0';
    connectrole = CR_DEFAULT;
    if(connpeer->state!=ENET_PEER_STATE_DISCONNECTED) enet_peer_reset(connpeer);
    connpeer = NULL;
#if 0
    if(!curpeer)
    {
        enet_host_destroy(clienthost);
        clienthost = NULL;
    }
#endif
}

void connectserv_(const char *servername, int serverport = 0, const char *password = NULL, int role = CR_DEFAULT)
{
    if(serverport <= 0) serverport = CUBE_DEFAULT_SERVER_PORT;
    if(watchingdemo) enddemoplayback();
    if(!clfail && cllock && searchlan<2) return;

    if(connpeer)
    {
        conoutf("aborting connection attempt");
        abortconnect();
    }
    connectrole = role;
    copystring(clientpassword, password ? password : "");
    ENetAddress address;
    address.port = serverport;

    if(servername)
    {
        addserver(servername, serverport, 0);
        conoutf("\f2attempting to %sconnect to \f5%s\f4:%d\f2", role==CR_DEFAULT?"":"\f8admin\f2", servername, serverport);
        if(!resolverwait(servername, &address))
        {
            conoutf("\f2could \f3not resolve\f2 server \f5%s\f2", servername);
            clientpassword[0] = '\0';
            connectrole = CR_DEFAULT;
            return;
        }
    }
    else
    {
        conoutf("\f2attempting to connect over \f1LAN\f2");
        address.host = ENET_HOST_BROADCAST;
    }

    if(!clienthost)
        clienthost = enet_host_create(NULL, 2, 3, 0, 0);

    if(clienthost)
    {
        connpeer = enet_host_connect(clienthost, &address, 3, 0);
        enet_host_flush(clienthost);
        connmillis = totalmillis;
        connattempts = 0;
        if(!m_mp(gamemode)) gamemode = GMODE_TEAMDEATHMATCH;
    }
    else
    {
        conoutf("\f2could \f3not connect\f2 to server");
        clientpassword[0] = '\0';
        connectrole = CR_DEFAULT;
    }
}

void connectserv(char *servername, int *serverport, char *password)
{
    modprotocol = false;
    connectserv_(servername, *serverport, password);
}

void connectadmin(char *servername, int *serverport, char *password)
{
    modprotocol = false;
    if(!password[0]) return;
    connectserv_(servername, *serverport, password, CR_ADMIN);
}

void lanconnect()
{
    modprotocol = false;
    connectserv_(NULL);
}

void modconnectserv(char *servername, int *serverport, char *password)
{
    modprotocol = true;
    connectserv_(servername, *serverport, password);
}

void modconnectadmin(char *servername, int *serverport, char *password)
{
    modprotocol = true;
    if(!password[0]) return;
    connectserv_(servername, *serverport, password, CR_ADMIN);
}

void modlanconnect()
{
    modprotocol = true;
    connectserv_(NULL);
}

void whereami()
{
    conoutf("you are at (%.2f,%.2f)", player1->o.x, player1->o.y);
}

void go_to(float *x, float *y, char *showmsg)
{
    if(player1->state != CS_EDITING) return;
    player1->newpos.x = *x;
    player1->newpos.y = *y;
    if(!showmsg || !*showmsg || strcmp(showmsg, "0")!=0)
        conoutf("you are going to (%.2f; %.2f)", *x, *y);
}

void disconnect(int onlyclean, int async)
{
    bool cleanup = onlyclean!=0;
    if(curpeer)
    {
        if(!discmillis)
        {
            enet_peer_disconnect(curpeer, DISC_NONE);
            enet_host_flush(clienthost);
            discmillis = totalmillis;
        }
        if(curpeer->state!=ENET_PEER_STATE_DISCONNECTED)
        {
            if(async) return;
            enet_peer_reset(curpeer);
        }
        curpeer = NULL;
        discmillis = 0;
        connected = 0;
        conoutf("disconnected");
        cleanup = true;
    }

    if(cleanup)
    {
        player1->clientnum = -1;
        player1->lifesequence = 0;
        player1->clientrole = CR_DEFAULT;
        kickallbots();
        loopv(players) zapplayer(players[i]);
        clearvote();
        audiomgr.clearworldsounds(false);
        localdisconnect();
    }
#if 0
    if(!connpeer && clienthost)
    {
        enet_host_destroy(clienthost);
        clienthost = NULL;
    }
#endif
    if(!onlyclean) localconnect();
    exechook(HOOK_SP_MP, "onDisconnect", "%d", -1);
}

void trydisconnect()
{
    if(connpeer)
    {
        conoutf("aborting connection attempt");
        abortconnect();
        return;
    }
    if(!curpeer)
    {
        conoutf("not connected");
        return;
    }
    conoutf("attempting to disconnect...");
    disconnect(0, !discmillis);
}

void _toserver(char *text, int msg, int msgt)
{
    bool toteam = text && text[0] == '%' && (m_teammode || team_isspect(player1->team));
    if(!toteam && text[0] == '%' && strlen(text) > 1) text++; // convert team-text to normal-text if no team-mode is active
    if(toteam) text++;
    filtertext(text, text, FTXT__CHAT);
    trimtrailingwhitespace(text);
    if(m_match(servstate.mastermode) && servstate.matchteamsize && !team_isactive(player1->team) && !(player1->team == TEAM_SPECT && player1->clientrole == CR_ADMIN)) toteam = true; // spect chat
    if(*text)
    {
        if(msg == SV_TEXTME) conoutf("\f%d%s %s", toteam ? 1 : 0, colorname(player1), highlight(text));
        else conoutf("%s:\f%d %s", colorname(player1), toteam ? 1 : 0, highlight(text));
        addmsg(toteam ? msgt : msg, "rs", text);
    }
}

void toserver(char *text)
{
    _toserver(text, SV_TEXT, SV_TEAMTEXT);
}

void toserverme(char *text)
{
    _toserver(text, SV_TEXTME, SV_TEAMTEXTME);
}

void echo(char *text)
{
    const char *s = strtok(text, "\n");
    do
    {
        conoutf("%s", s ? s : "");
        s = strtok(NULL, "\n");
    }
    while(s);
}

VARP(allowhudechos, 0, 1, 1);
void hudecho(char *text)
{
    const char *s = strtok(text, "\n");
    void (*outf)(const char *s, ...) = allowhudechos ? hudoutf : conoutf;
    do
    {
        outf("%s", s ? s : "");
        s = strtok(NULL, "\n");
    }
    while(s);
}

void pm(char *text)
{
    if(!text || !text[0]) return;
    int cn = -1;
    char digit;
    while ((digit = *text++) != '\0')
    {
        if (digit < '0' || digit > '9') break;
        if(cn < 0) cn = 0;
        else cn *= 10;
        cn += digit - '0';
    }
    playerent *to = getclient(cn);
    if(!to)
    {
        conoutf("invalid client number specified");
        return;
    }

    if(!isspace(digit)) { --text; }

    // FIXME:
    /*if(!text || !text[0] || !isdigit(text[0])) return;
    int cn = -1;
    char *numend = strpbrk(text, " \t");
    if(!numend) return;
    string cnbuf;
    copystring(cnbuf, text, min(numend-text+1, MAXSTRLEN));
    cn = atoi(cnbuf);
    playerent *to = getclient(cn);
    if(!to)
    {
        conoutf("invalid client number specified");
        return;
    }

    if(*numend) numend++;*/
    // :FIXME

    filtertext(text, text, FTXT__CHAT);
    trimtrailingwhitespace(text);

    addmsg(SV_TEXTPRIVATE, "ris", cn, text);
    conoutf("to %s:\f9 %s", colorname(to), highlight(text));
}
COMMAND(pm, "c");

COMMAND(echo, "c");
COMMAND(hudecho, "c");
COMMANDN(say, toserver, "c");
COMMANDN(me, toserverme, "c");
COMMANDN(connect, connectserv, "sis");
COMMAND(connectadmin, "sis");
COMMAND(lanconnect, "");
COMMANDN(modconnect, modconnectserv, "sis");
COMMAND(modconnectadmin, "sis");
COMMAND(modlanconnect, "");
COMMANDN(disconnect, trydisconnect, "");
COMMAND(whereami, "");
COMMAND(go_to, "ffs");

void current_version(char *text)
{
    int version = atoi(text);
    if (version && AF_VERSION<version)
    {
        conoutf("\f3YOUR VERSION OF ACTIONFPS IS OUTDATED. PLEASE DOWNLOAD ACTIONFPS %d", version);
        execute("sleep 500 [ echo \"game outdated!\"; showmenu download ] 1");
    }
}
COMMAND(current_version, "s");

void open_web_browser(const char *url)
{
#if defined(WIN32)
    ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
#elif defined(__linux__)
    defformatstring(cmd)("x-www-browser %s &", url);
    system(cmd);
#elif defined(__APPLE__)
    defformatstring(cmd)("open %s &", url);
    system(cmd);
#else

#endif
}

void downloadgame()
{
#if defined(WIN32)
    open_web_browser(AF_DOWNLOAD "/?os=windows&ref=game");
#elif defined(__linux__)
    open_web_browser(AF_DOWNLOAD "/?os=linux&ref=game");
#elif defined(__APPLE__)
    open_web_browser(AF_DOWNLOAD "/?os=mac&ref=game");
#else

#endif
}
COMMAND(downloadgame, "");

void cleanupclient()
{
    abortconnect();
    disconnect(1);
    if(clienthost)
    {
        enet_host_destroy(clienthost);
        clienthost = NULL;
    }
}

// collect c2s messages conveniently

vector<uchar> messages;

void addmsg(int type, const char *fmt, ...)
{
    static uchar buf[MAXTRANS];
    ucharbuf p(buf, MAXTRANS);
    putint(p, type);
    int numi = 1, nums = 0;
    bool reliable = false;
    if(fmt)
    {
        va_list args;
        va_start(args, fmt);
        while(*fmt) switch(*fmt++)
        {
            case 'r': reliable = true; break;
            case 'v':
            {
                int n = va_arg(args, int);
                int *v = va_arg(args, int *);
                loopi(n) putint(p, v[i]);
                numi += n;
                break;
            }

            case 'i':
            {
                int n = isdigit(*fmt) ? *fmt++-'0' : 1;
                loopi(n) putint(p, va_arg(args, int));
                numi += n;
                break;
            }
            case 's':
            {
                const char *t = va_arg(args, const char *);
                if(t) sendstring(t, p); nums++; break;
            }
        }
        va_end(args);
    }
    int num = nums?0:numi, msgsize = msgsizelookup(type);
    if(msgsize && num!=msgsize) { fatal("inconsistent msg size for %d (%d != %d)", type, num, msgsize); }
    int len = p.length();
    messages.add(len&0xFF);
    messages.add((len>>8)|(reliable ? 0x80 : 0));
    loopi(len) messages.add(buf[i]);
}

static int lastupdate = -1000, lastping = 0;
bool sendmapidenttoserver = false;

void sendpackettoserv(int chan, ENetPacket *packet)
{
    if(curpeer) enet_peer_send(curpeer, chan, packet);
    else localclienttoserver(chan, packet);
}

void c2skeepalive()
{
    if(clienthost && (curpeer || connpeer)) enet_host_service(clienthost, NULL, 0);
}

extern string masterpwd;
bool sv_pos = true;

void c2sinfo(playerent *d)                  // send update to the server
{
    if(d->clientnum<0) return;              // we haven't had a welcome message from the server yet
    if(totalmillis-lastupdate<40) return;    // don't update faster than 25fps

    if(d->state==CS_ALIVE || d->state==CS_EDITING)
    {
        packetbuf q(100);
        int cn = d->clientnum,
            x = (int)(d->o.x*DMF),          // quantize coordinates to 1/16th of a cube, between 1 and 3 bytes
            y = (int)(d->o.y*DMF),
            z = (int)((d->o.z - d->eyeheight)*DMF),
            ya = (int)((512 * d->yaw) / 360.0f),
            pi = (int)((127 * d->pitch) / 90.0f),
            r = (int)(31*d->roll/20),
            dxt = (int)(d->vel.x*DVELF),
            dyt = (int)(d->vel.y*DVELF),
            dzt = (int)(d->vel.z*DVELF),
            dx = dxt - d->vel_t.i[0],
            dy = dyt - d->vel_t.i[1],
            dz = dzt - d->vel_t.i[2],
            // pack rest in 1 int: strafe:2, move:2, onfloor:1, onladder: 1
            f = (d->strafe&3) | ((d->move&3)<<2) | (((int)d->onfloor)<<4) | (((int)d->onladder)<<5) | ((d->lifesequence&1)<<6) | (((int)d->crouching)<<7),
            g = (dx?1:0) | ((dy?1:0)<<1) | ((dz?1:0)<<2) | ((r?1:0)<<3) | (((int)d->scoping)<<4) | (((int)d->shoot)<<5);
            d->vel_t.i[0] = dxt;
            d->vel_t.i[1] = dyt;
            d->vel_t.i[2] = dzt;
        int usefactor = sfactor < 7 ? 7 : sfactor, sizexy = 1 << (usefactor + 4);
        if(cn >= 0 && cn < 32 &&
            usefactor <= 7 + 3 &&       // map size 7..10
            x >= 0 && x < sizexy &&
            y >= 0 && y < sizexy &&
            z >= -2047 && z <= 2047 &&
            ya >= 0 && ya < 512 &&
            pi >= -128 && pi <= 127 &&
            r >= -32 && r <= 31 &&
            dx >= -8 && dx <= 7 && // FIXME
            dy >= -8 && dy <= 7 &&
            dz >= -8 && dz <= 7)
        { // compact POS packet
            bool noroll = !r, novel = !dx && !dy && !dz;
            bitbuf<packetbuf> b(q);
            putint(q, SV_POSC);
            b.putbits(5, cn);
            b.putbits(2, usefactor - 7);
            b.putbits(usefactor + 4, x);
            b.putbits(usefactor + 4, y);
            b.putbits(9, ya);
            b.putbits(8, pi + 128);
            b.putbits(1, noroll ? 1 : 0);
            if(!noroll) b.putbits(6, r + 32);
            b.putbits(1, novel ? 1 : 0);
            if(!novel)
            {
                b.putbits(4, dx + 8);
                b.putbits(4, dy + 8);
                b.putbits(4, dz + 8);
            }
            b.putbits(8, f);
            b.putbits(1, z < 0 ? 1 : 0);
            if(z < 0) z = -z;                 // z is encoded with 3..10 bits minimum (fitted to byte boundaries), or full 11 bits if necessary
            int s = (b.rembits() - 1 + 8) % 8;
            if(s < 3) s += 8;
            if(z >= (1 << s)) s = 11;
            b.putbits(1, s == 11 ? 1 : 0);
            b.putbits(s, z);
            b.putbits(1, d->scoping ? 1 : 0);
            b.putbits(1, d->shoot ? 1 : 0);
        }
        else
        { // classic POS packet
            putint(q, SV_POS);
            putint(q, d->clientnum);
            putuint(q, x);
            putuint(q, y);
            putuint(q, z);
            putuint(q, (int)d->yaw);
            putint(q, (int)d->pitch);
            putuint(q, g);
            if (r) putint(q, (int)(125*d->roll/20));
            if (dx) putint(q, dx);
            if (dy) putint(q, dy);
            if (dz) putint(q, dz);
            putuint(q, f);
        }
        sendpackettoserv(0, q.finalize());
        d->shoot = false;
    }

    if(sendmapidenttoserver || messages.length() || totalmillis-lastping>250)
    {
        packetbuf p(MAXTRANS);

        if(sendmapidenttoserver) // new map
        {
            p.reliable();
            putint(p, SV_MAPIDENT);
            putint(p, maploaded);
            putint(p, hdr.maprevision);
            sendmapidenttoserver = false;
        }
        int i = 0;
        while(i < messages.length()) // send messages collected during the previous frames
        {
            int len = messages[i] | ((messages[i+1]&0x7F)<<8);
            if(p.remaining() < len) break;
            if(messages[i+1]&0x80) p.reliable();
            p.put(&messages[i+2], len);
            i += 2 + len;
        }
        messages.remove(0, i);
        if(totalmillis-lastping>250)
        {
            putint(p, SV_PING);
            putint(p, totalmillis);
            lastping = totalmillis;
        }
        if(p.length()) sendpackettoserv(1, p.finalize());
    }

    if(clienthost) enet_host_flush(clienthost);
    lastupdate = totalmillis;
}

string &getversiondescription()
{
    char platform[] =

#ifdef WIN32
    "Windows"
#elif __APPLE__
    "Mac"
#elif __linux__
    "Linux"
#else
    "Unknown"
#endif

#ifdef __clang__
    ", clang"
#elif __GNUC__
    ", gcc"
#elif _MSC_VER
    ", msvc"
#endif

#ifdef _DEBUG
    ", debug"
#endif
    ;

    static string versiondescription;

#ifdef AF_REVISION
    formatstring(versiondescription)("revision %s (%s)", AF_REVISION, platform);
#else
    formatstring(versiondescription)("version %d (%s)", AF_VERSION, platform);
#endif

    return versiondescription;
}

int getbuildtype()
{
    return (isbigendian() ? 0x80 : 0 )|(adler((unsigned char *)guns, sizeof(guns)) % 31 << 8)|
        #ifdef WIN32
            0x40 |
        #endif
        #ifdef __APPLE__
            0x20 |
        #endif
        #ifdef _DEBUG
            0x08 |
        #endif
        #ifdef __GNUC__
            0x04 |
        #endif
            0;
}

ucharbuf auth_challenge;
SVARP(authid, "");
ucharbuf authkey;

void readauthkey()
{
    ERR_load_crypto_strings();

    stream *f = openfile(path("config/authkey", true), "r");
    if(!f) return;
    char buf[5000] = "";
    int bytes = f->read(buf, sizeof(buf));

    authkey.reset();
    authkey.buf = new uchar[bytes];
    authkey.maxlen = bytes;
    authkey.put((uchar *)buf, bytes);

    delete f;
}

void updateauthkey(const char *new_key_base64)
{
    size_t len;
    std::string encoded = std::string(new_key_base64);
    std::string decoded = base64_decode(encoded);
    len = decoded.length();
    authkey.reset();
    authkey.buf = new uchar[len];
    authkey.maxlen = len;
    authkey.put((uchar *)decoded.c_str(), len);
}

bool testauthkey()
{
    DSA *priv_dsa = DSA_new();
    BIO *bio = BIO_new(BIO_s_mem());
    BIO_write(bio, (const void *) authkey.buf, authkey.len);
    DSA *ret = PEM_read_bio_DSAPrivateKey(bio, &priv_dsa, NULL, NULL);

    bool valid = ret != NULL;

    DELETEP(bio);
    DELETEP(priv_dsa);

    return valid;
}

void writeauthkey()
{
    stream *f = openfile(path("config/authkey", true), "w");
    if(!f) return;
    f->write(authkey.buf, authkey.len);
    f->close();
    delete f;
}



void sendintro()
{
    extern char *authid;
    extern char *defaultgroup;

    packetbuf p(MAXTRANS, ENET_PACKET_FLAG_RELIABLE);
    putint(p, SV_CONNECT);
    sendstring(getversiondescription(), p);
    sendstring(player1->name, p);
    sendstring(defaultgroup, p);
    sendstring(genpwdhash(player1->name, clientpassword, sessionid), p);
    sendstring(!lang || strlen(lang) != 2 ? "" : lang, p);
    putint(p, connectrole);
    clientpassword[0] = '\0';
    connectrole = CR_DEFAULT;
    putint(p, player1->nextprimweap->type);
    loopi(2) putint(p, player1->skin(i));

    // authentication
    sendstring(authid, p);

    DSA *priv_dsa = DSA_new();
    BIO *bio = BIO_new(BIO_s_mem());
    BIO_write(bio, (const void *) authkey.buf,  authkey.len);
    DSA *ret = PEM_read_bio_DSAPrivateKey(bio, &priv_dsa, NULL, NULL);
    if(!ret)
    {
        char buffer[120];
        ERR_error_string(ERR_get_error(), buffer);

        clientlogf("failed to read private key! please issue a new key.");
        clientlogf("OpenSSL error: %s", buffer);
        putint(p, 0);
        DELETEP(priv_dsa);
        DELETEP(bio);
        sendpackettoserv(1, p.finalize());
        return;
    }

    uchar *signature = new uchar[DSA_size(priv_dsa)];
    uint siglen;
    int res = DSA_sign(0, auth_challenge.buf, auth_challenge.len, signature, &siglen, priv_dsa);
    putint(p, siglen);
    p.put(signature, siglen);

    sendpackettoserv(1, p.finalize());

    DELETEP(bio);
    DELETEP(priv_dsa);
    DELETEA(signature);
}

void gets2c()           // get updates from the server
{
    ENetEvent event;
    if(!clienthost || (!curpeer && !connpeer)) return;
    if(connpeer && totalmillis/3000 > connmillis/3000)
    {
        conoutf("attempting to connect...");
        connmillis = totalmillis;
        ++connattempts;
        if(connattempts > 3)
        {
            conoutf("\f3could not connect to server");
            abortconnect();
            return;
        }
    }
    while(clienthost!=NULL && enet_host_service(clienthost, &event, 0)>0)
    switch(event.type)
    {
        case ENET_EVENT_TYPE_CONNECT:
            disconnect(1);
            curpeer = connpeer;
            connpeer = NULL;
            connected = 1;
            conoutf("connected to server");
            exechook(HOOK_SP_MP, "onConnect", "%d", -1);
            throttle();
            if(editmode) toggleedit(true);
            break;

        case ENET_EVENT_TYPE_RECEIVE:
        {
            extern packetqueue pktlogger;
            pktlogger.queue(event.packet);

            if(discmillis) conoutf("attempting to disconnect...");
            else servertoclient(event.channelID, event.packet->data, (int)event.packet->dataLength);
            // destroyed in logger
            //enet_packet_destroy(event.packet);
            break;
        }

        case ENET_EVENT_TYPE_DISCONNECT:
        {
            extern const char *disc_reason(int reason);
            if(event.data>=DISC_NUM) event.data = DISC_NONE;
            if(event.peer==connpeer)
            {
                conoutf("\f3could not connect to server");
                abortconnect();
            }
            else
            {
                if(!discmillis || event.data) conoutf("\f3server network error, disconnecting (%s) ...", disc_reason(event.data));
                disconnect();
            }
            return;
        }

        default:
            break;
    }
}

// sendmap/getmap commands, should be replaced by more intuitive map downloading

vector<char *> securemaps;

void resetsecuremaps() { securemaps.deletearrays(); }
void securemap(char *map) { if(map) securemaps.add(newstring(map)); }
bool securemapcheck(const char *map, bool msg)
{
    if(strstr(map, "maps/")==map || strstr(map, "maps\\")==map) map += strlen("maps/");
    loopv(securemaps) if(!strcmp(securemaps[i], map))
    {
        if(msg)
        {
            conoutf("\f3Map \f4%s\f3 is secured. This means you CAN'T send, receive or overwrite it.", map);
            if(connected)
            {
                conoutf("\f3If you get this error often, you (or the server) may be running an outdated game.");
                conoutf("\f3You can check for updates at \f1https://actionfps.com");
            }
        }
        return true;
    }
    return false;
}

void sendmap(char *mapname)
{
    if(!*mapname) mapname = getclientmap();
    if(securemapcheck(mapname)) return;
    if(gamemode == GMODE_COOPEDIT && !strcmp(getclientmap(), mapname)) save_world(mapname, true, false); // skip optimisations, don't add undos

    int mapsize, cfgsize, cfgsizegz, revision;
    uchar *mapdata = readmap(path(mapname), &mapsize, &revision);
    if(!mapdata) return;
    uchar *cfgdata = readmcfggz(path(mapname), &cfgsize, &cfgsizegz);
    if(!cfgdata) { cfgsize = 0; cfgsizegz = 0; }

    packetbuf p(MAXTRANS + mapsize + cfgsizegz, ENET_PACKET_FLAG_RELIABLE);

    putint(p, SV_SENDMAP);
    sendstring(mapname, p);
    putint(p, mapsize);
    putint(p, cfgsize);
    putint(p, cfgsizegz);
    putint(p, revision);
    if(MAXMAPSENDSIZE - p.length() < mapsize + cfgsizegz || cfgsize > MAXCFGFILESIZE)
    {
        conoutf("map %s is too large to send", mapname);
        delete[] mapdata;
        if(cfgsize) delete[] cfgdata;
        return;
    }
    p.put(mapdata, mapsize);
    delete[] mapdata;
    if(cfgsizegz)
    {
        p.put(cfgdata, cfgsizegz);
        delete[] cfgdata;
    }

    sendpackettoserv(2, p.finalize());
    conoutf("sending map %s to server...", mapname);
}

void getmap(char *name, char *callback)
{
    if((!name || !*name)
        || (connected && !strcmp(name, getclientmap())) )
    {
        conoutf("requesting map from server...");
        packetbuf p(10, ENET_PACKET_FLAG_RELIABLE);
        putint(p, SV_RECVMAP);
        sendpackettoserv(2, p.finalize());
    }
    else
    {
        requirepackage(PCK_MAP, name);
        if(downloadpackages(false))
        {
            if(callback && *callback) execute(callback);
            conoutf("map %s installed successfully", name);
        }
        else conoutf("\f3map download failed.");
    }
}

void deleteservermap(char *mapname)
{
    const char *name = behindpath(mapname);
    if(!*name || securemapcheck(name)) return;
    addmsg(SV_REMOVEMAP, "rs", name);
}

string demosubpath;
void getdemo(int *idx, char *dsp)
{
    if(!multiplayer(false))
    {
        conoutf("\f3Getting demo from server is not available in singleplayer");
        return;
    }
    if(dsp && dsp[0]) formatstring(demosubpath)("%s/", dsp);
    else copystring(demosubpath, "");
    if(*idx<=0) conoutf("getting demo...");
    else conoutf("getting demo %d...", *idx);
    addmsg(SV_GETDEMO, "ri", *idx);
}

void listdemos()
{
    if(!multiplayer(false))
    {
        conoutf("\f3Listing demos from server is not available in singleplayer");
        return;
    }
    conoutf("listing demos...");
    addmsg(SV_LISTDEMOS, "r");
}

void shiftgametime(int newmillis)
{
    newmillis = max(0, newmillis);
    int gamemillis = gametimecurrent + (lastmillis - lastgametimeupdate);

    if(!watchingdemo) { conoutf("You have to be watching a demo to change game time"); return; }
    if(newmillis > gametimemaximum) { conoutf("Invalid time specified"); return; }

    if(newmillis < gamemillis)
    {
        // if rewinding
        if(!curdemofile || !curdemofile[0]) return;
        watchingdemo = false;
        callvote(SA_MAP, curdemofile, "-1", "0");
        nextmillis = newmillis;
    }
    else
    {
        nextmillis = newmillis - gamemillis;
    }
}

void setminutesremaining(int *minutes)
{
    shiftgametime(gametimemaximum - (*minutes)*60000);
}

void rewinddemo(int *seconds)
{
    int gamemillis = gametimecurrent+(lastmillis-lastgametimeupdate);
    shiftgametime(gamemillis - (*seconds)*1000);
}

COMMAND(sendmap, "s");
COMMAND(getmap, "ss");
COMMAND(deleteservermap, "s");
COMMAND(resetsecuremaps, "");
COMMAND(securemap, "s");
COMMAND(getdemo, "is");
COMMAND(listdemos, "");
COMMANDN(setmr, setminutesremaining, "i");
COMMANDN(rewind, rewinddemo, "i");
